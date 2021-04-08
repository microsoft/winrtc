/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_device/win/audio_device_core_win.h"

#include <Windows.Devices.Enumeration.h>
#include <Windows.Foundation.h>
#include <Windows.Media.Devices.h>
#include <assert.h>
#include <comdef.h>
#include <mmsystem.h>
#include <string.h>
#include <strsafe.h>
#include <uuids.h>
#include <windows.h>
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include <memory>

#include "modules/audio_device/audio_device_config.h"
#include "rtc_base/logging.h"
#include "rtc_base/platform_thread.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/thread_annotations.h"
#include "system_wrappers/include/sleep.h"

using ABI::Windows::Devices::Enumeration::DeviceClass;
using ABI::Windows::Devices::Enumeration::DeviceInformation;
using ABI::Windows::Devices::Enumeration::DeviceInformationCollection;
using ABI::Windows::Devices::Enumeration::IDeviceInformation;
using ABI::Windows::Devices::Enumeration::IDeviceInformationStatics;
using ABI::Windows::Foundation::GetActivationFactory;
using ABI::Windows::Foundation::IAsyncOperation;
using ABI::Windows::Foundation::Collections::IVectorView;
using ABI::Windows::Media::Devices::AudioDeviceRole;
using ABI::Windows::Media::Devices::IMediaDeviceStatics;
using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;
using Microsoft::WRL::Wrappers::RoInitializeWrapper;

#pragma comment(lib, "Mmdevapi.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "WindowsApp.lib")


// Macro that calls a COM method returning HRESULT value.
#define EXIT_ON_ERROR(hres) \
  do {                      \
    if (FAILED(hres))       \
      goto Exit;            \
  } while (0)

#ifndef MAXERRORLENGTH
const size_t MAXERRORLENGTH = 256;
#endif

const size_t MIN_CORE_SPEAKER_VOLUME = 0;
const size_t MAX_CORE_SPEAKER_VOLUME = 100;

HRESULT WaitForASyncWithEvent(_In_ IAsyncInfo* async_info,
                              _In_ std::shared_ptr<HANDLE> event_completed_handle_ptr,
                              _In_ DWORD timeout_ms) {
  HRESULT hr = S_OK;
  AsyncStatus async_status;

  if (SUCCEEDED(hr)) {
    hr = async_info ? S_OK : RPC_E_INVALID_PARAMETER;
  }

  // Gets the operation status to check if operation started before waiting.
  if (SUCCEEDED(hr)) {
    hr = async_info->get_Status(&async_status);
  }

  if (SUCCEEDED(hr)) {
    if (async_status == AsyncStatus::Started) {
      DWORD trigger_event =
          ::WaitForSingleObjectEx(*event_completed_handle_ptr, timeout_ms, FALSE);
      if (trigger_event == WAIT_OBJECT_0) {
        hr = S_OK;
      } else if (trigger_event == WAIT_TIMEOUT) {
        HRESULT hr2 = async_info->get_Status(&async_status);
        if (SUCCEEDED(hr2) && (async_status == AsyncStatus::Completed)) {
          // The async operation might be completed before the put_Completed
          // callback triggering event_completed_handle_ptr have chance to be
          // defined. In that case, the event timesout, but the async
          // operation might be successfully completed.
          RTC_LOG(LS_WARNING)
              << "Wait operation timedout, but async operation completed.";
          hr = S_OK;
        } else {
          RTC_LOG(LS_ERROR)
              << "Wait operation timedout. It took longer than " << timeout_ms
              << " ms. hr2: " << hr2 << " Async Status: " << async_status;
          hr = RPC_E_TIMEOUT;
        }
      } else {
        RTC_LOG(LS_ERROR) << "Wait operation did not succeeded. Error: "
                          << trigger_event << " " << GetLastError();
        hr = E_FAIL;
      }
    } else if (async_status == AsyncStatus::Completed) {
      RTC_LOG(LS_WARNING)
          << "Wait operation didn't wait because async operation "
             "has been completed already.";
      hr = S_OK;
    } else {
      RTC_LOG(LS_ERROR) << "Something happened to the async operation. Error: "
                        << async_status;
      hr = E_FAIL;
    }
  }

  return hr;
}

// ----------------------------------------------------------------------------
//  _TraceCOMError
// ----------------------------------------------------------------------------

void _TraceCOMError(HRESULT hr) {
  wchar_t errorText[MAXERRORLENGTH];

  const DWORD dwFlags =
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
  const DWORD dwLangID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

  // Gets the system's human readable message string for this HRESULT.
  // All error message in English by default.
  DWORD messageLength = ::FormatMessageW(dwFlags, 0, hr, dwLangID, errorText,
                                         MAXERRORLENGTH, nullptr);
  assert(messageLength <= MAXERRORLENGTH);
  if (messageLength > 0) {
      // Trims tailing white space (FormatMessage() leaves a trailing cr-lf.).
      for (; messageLength && ::isspace(errorText[messageLength - 1]);
           --messageLength) {
        errorText[messageLength - 1] = '\0';
      }
      RTC_LOG(LS_ERROR) << "Core Audio method failed (hr=" << hr
                        << "): " << rtc::ToUtf8(errorText);
  } else {
    RTC_LOG(LS_ERROR) << "Core Audio method failed (hr=" << hr << ")";
  }
}

struct CCompletionDelegate
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
          Microsoft::WRL::FtmBase,
          IActivateAudioInterfaceCompletionHandler> {
  const HANDLE _completedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

  HRESULT _hrActivateResult = E_ILLEGAL_METHOD_CALL;
  Microsoft::WRL::ComPtr<IUnknown> _uuid;

  ~CCompletionDelegate() { CloseHandle(_completedEvent); }

  // IActivateAudioInterfaceCompletionHandler
  STDMETHODIMP ActivateCompleted(
      IActivateAudioInterfaceAsyncOperation* operation) override {
    HRESULT hr = operation->GetActivateResult(&_hrActivateResult, &_uuid);
    SetEvent(_completedEvent);
    return hr;
  }

  STDMETHODIMP Wait(DWORD timeout) {
    switch (WaitForSingleObject(_completedEvent, timeout)) {
      case WAIT_OBJECT_0:
        return S_OK;
      case WAIT_TIMEOUT:
        return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
      case WAIT_FAILED:
        return HRESULT_FROM_WIN32(GetLastError());
      default:
        return E_FAIL;
    }
  }

  STDMETHODIMP GetActivateResult(IUnknown** activatedInterface) {
    _uuid.CopyTo(activatedInterface);
    _uuid = nullptr;

    HRESULT hr = _hrActivateResult;
    _hrActivateResult = E_ILLEGAL_METHOD_CALL;

    return hr;
  }
};

template <DeviceClass DEVICE_CLASS>
struct DeviceHelper {
 protected:
  RoInitializeWrapper ro_initialize_wrapper_;
  ComPtr<IDeviceInformationStatics> device_info_statics_;
  Microsoft::WRL::ComPtr<IDeviceInformation> _deviceInformation;

  bool _usingDeviceIndex = false;
  uint16_t _deviceIndex = -1;
  webrtc::AudioDeviceModule::WindowsDeviceType _deviceType =
      static_cast<webrtc::AudioDeviceModule::WindowsDeviceType>(0);

 public:
  //
  // Device enumeration
  //
  DeviceHelper() : ro_initialize_wrapper_(RO_INIT_MULTITHREADED) {
    GetActivationFactory(
        HStringReference(
            RuntimeClass_Windows_Devices_Enumeration_DeviceInformation)
            .Get(),
        &device_info_statics_);
  }

  int16_t Devices() {
    HRESULT hr = S_OK;

    ComPtr<IAsyncOperation<DeviceInformationCollection*>>
        async_op_device_info_collection;
    ComPtr<IVectorView<DeviceInformation*>> device_collection;
    unsigned int device_count = 0;

    THR((HRESULT)ro_initialize_wrapper_);

    // Call FindAllAsync and then start the async operation.
    THR(device_info_statics_->FindAllAsyncDeviceClass(
        DEVICE_CLASS, &async_op_device_info_collection));

    // Block and suspend thread until the async operation finishes or timeouts.
    THR(WaitForAsyncOperation(async_op_device_info_collection.Get()));

    // Returns device collection if async operation completed successfully.
    THR(async_op_device_info_collection->GetResults(&device_collection));

    // Gets the count of devices of this class
    THR(device_collection->get_Size(&device_count));

  Cleanup:
    return device_count;
  }

  int32_t DeviceName(uint16_t index,
                     char name[webrtc::kAdmMaxDeviceNameSize],
                     char guid[webrtc::kAdmMaxGuidSize]) {
    HRESULT hr;

    ComPtr<IAsyncOperation<DeviceInformationCollection*>>
        async_op_device_info_collection;
    ComPtr<IVectorView<DeviceInformation*>> device_collection;
    ComPtr<IDeviceInformation> device_info;

    unsigned int device_count = 0;

    // Call FindAllAsync and then start the async operation.
    THR(device_info_statics_->FindAllAsyncDeviceClass(
        DEVICE_CLASS, &async_op_device_info_collection));

    // Block and suspend thread until the async operation finishes or timeouts.
    THR(WaitForAsyncOperation(async_op_device_info_collection.Get()));

    // Returns device collection if async operation completed successfully.
    THR(async_op_device_info_collection->GetResults(&device_collection));

    // Gets the count of devices of this class
    THR(device_collection->get_Size(&device_count));

    // Special fix for the case when the user selects '-1' as index (<=>
    // Default / Communication Device)
    if (index == (uint16_t)(-1)) {
      index = 0;
      RTC_LOG(LS_VERBOSE)
          << "Default Communication endpoint device will be used";
    }

    if (index >= device_count) {
      return -1;
    }

    THR(device_collection->GetAt(index, &device_info));

    if (name != nullptr) {
      HString device_name;
      THR(device_info->get_Name(device_name.ReleaseAndGetAddressOf()));

      std::wstring deviceInformationName{device_name.GetRawBuffer(nullptr)};
      std::string nameUFT8 = rtc::ToUtf8(deviceInformationName);
      size_t length =
          std::min(nameUFT8.size(),
                   static_cast<size_t>(webrtc::kAdmMaxDeviceNameSize - 1));
      std::copy_n(nameUFT8.begin(), length, guid);
      name[length] = '\0';
    }

    if (guid != nullptr) {
      HString device_id;
      THR(device_info->get_Id(device_id.ReleaseAndGetAddressOf()));

      std::wstring deviceInformationId{device_id.GetRawBuffer(nullptr)};
      std::string idUFT8 = rtc::ToUtf8(deviceInformationId);
      size_t length = std::min(
          idUFT8.size(), static_cast<size_t>(webrtc::kAdmMaxGuidSize - 1));
      std::copy_n(idUFT8.begin(), length, guid);
      guid[length] = '\0';
    }
  Cleanup:
    return SUCCEEDED(hr) ? 0 : -1;
  }

  //
  // Device selection
  //

  int32_t SetDevice(uint16_t index) {
    HRESULT hr;

    ComPtr<IAsyncOperation<DeviceInformationCollection*>>
        async_op_device_info_collection;
    ComPtr<IVectorView<DeviceInformation*>> device_collection;

    unsigned int device_count = 0;

    // Call FindAllAsync and then start the async operation.
    THR(device_info_statics_->FindAllAsyncDeviceClass(
        DEVICE_CLASS, &async_op_device_info_collection));

    // Block and suspend thread until the async operation finishes or timeouts.
    THR(WaitForAsyncOperation(async_op_device_info_collection.Get()));

    // Returns device collection if async operation completed successfully.
    THR(async_op_device_info_collection->GetResults(&device_collection));

    // Special fix for the case when the user selects '-1' as index (<=>
    // Default / Communication Device)
    if (index == (uint16_t)(-1)) {
      index = 0;
      RTC_LOG(LS_VERBOSE)
          << "Default Communication endpoint device will be used";
    }

    // Gets the count of devices of this class
    THR(device_collection->get_Size(&device_count));

    if (index >= device_count) {
      return -1;
    }

    THR(device_collection->GetAt(index, &_deviceInformation));

    _deviceIndex = index;
    _usingDeviceIndex = true;

  Cleanup:
    return SUCCEEDED(hr) ? 0 : -1;
  }

  int32_t SetDevice(
      webrtc::AudioDeviceModule::WindowsDeviceType windowsDeviceType) {
    HRESULT hr;
    ComPtr<IMediaDeviceStatics> media_device;
    ComPtr<IAsyncOperation<DeviceInformation*>> async_operation;
    HString audio_device_id;
    unsigned int audio_device_id_len;
    AudioDeviceRole audioDeviceRole;

    switch (windowsDeviceType) {
      case webrtc::AudioDeviceModule::kDefaultCommunicationDevice:
        audioDeviceRole = AudioDeviceRole::AudioDeviceRole_Communications;
        break;

      case webrtc::AudioDeviceModule::kDefaultDevice:
        audioDeviceRole = AudioDeviceRole::AudioDeviceRole_Default;
        break;

      default:
        return -1;
    }

    THR(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Media_Devices_MediaDevice).Get(),
        &media_device));

    switch (DEVICE_CLASS) {
      case DeviceClass::DeviceClass_AudioCapture:
        THR(media_device->GetDefaultAudioCaptureId(
            audioDeviceRole, audio_device_id.ReleaseAndGetAddressOf()));
        break;

      case DeviceClass::DeviceClass_AudioRender:
        THR(media_device->GetDefaultAudioRenderId(
            audioDeviceRole, audio_device_id.ReleaseAndGetAddressOf()));
        break;

      default:
        return -1;
    }

    audio_device_id.GetRawBuffer(&audio_device_id_len);
    if (!audio_device_id_len) {
      return -1;
    }

    THR(device_info_statics_->CreateFromIdAsync(audio_device_id.Get(),
                                                &async_operation));

    // Block and suspend thread until the async operation finishes or timeouts.
    THR(WaitForAsyncOperation(async_operation.Get()));

    THR(async_operation->GetResults(
        _deviceInformation.ReleaseAndGetAddressOf()));

    _deviceType = windowsDeviceType;
    _usingDeviceIndex = false;

  Cleanup:
    return SUCCEEDED(hr) ? 0 : -1;
  }

  virtual ComPtr<IDeviceInformation> GetDevice() final {
    return _deviceInformation;
  }
};

template <DeviceClass DEVICE_CLASS>
struct AudioDeviceHelper : public DeviceHelper<DEVICE_CLASS> {
 protected:
  webrtc::AudioDeviceBuffer* _pAudioBuffer = nullptr;

  bool _mixerInitialized = false;
  bool _transportInitialized = false;
  bool _transporting = false;

  UINT _audioFrameSize = 0;
  uint32_t _sampleRate = 0;
  uint32_t _blockSize = 0;
  uint32_t _channels = 2;
  UINT64 _samples = 0;

  HANDLE _hSamplesReadyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  HANDLE _hStartedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  HANDLE _hShutdownEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  HANDLE _hThread = nullptr;

  Microsoft::WRL::ComPtr<IAudioClient2> _audioClient;
  Microsoft::WRL::ComPtr<ISimpleAudioVolume> _simpleAudioVolume;

  static DWORD WINAPI WSAPIThread(LPVOID context) {
    return reinterpret_cast<AudioDeviceHelper*>(context)->Transport();
  }

 public:
  virtual ~AudioDeviceHelper() {
    CloseHandle(_hSamplesReadyEvent);
    CloseHandle(_hThread);
    CloseHandle(_hStartedEvent);
    CloseHandle(_hShutdownEvent);
  }

  virtual void AttachAudioBuffer(webrtc::AudioDeviceBuffer* audioBuffer) {
    _pAudioBuffer = audioBuffer;
  }

  virtual DWORD Transport() = 0;

  //
  // Audio mixer initialization
  //

  virtual int32_t InitMixer() {
    if (MixerIsInitialized()) {
      return 0;
    }

    if (Transporting() || GetDevice() == nullptr) {
      return -1;
    }

    if (_usingDeviceIndex) {
      int16_t nDevices = Devices();
      if (_deviceIndex > (nDevices - 1)) {
        RTC_LOG(LS_ERROR) << "current device selection is invalid => unable to"
                          << " initialize";
        return -1;
      }
    }

    int32_t ret;
    if (_usingDeviceIndex) {
      ret = SetDevice(_deviceIndex);
    } else {
      ret = SetDevice(_deviceType);
    }
    if (ret != 0) {
      RTC_LOG(LS_ERROR) << "failed to initialize the enpoint device";
      return -1;
    }

    Microsoft::WRL::ComPtr<CCompletionDelegate> completionDelegate =
        Microsoft::WRL::Make<CCompletionDelegate>();
    {
      HString device_id;
      Microsoft::WRL::ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOp;
      GetDevice()->get_Id(device_id.ReleaseAndGetAddressOf());
      HRESULT hr = ActivateAudioInterfaceAsync(
          device_id.GetRawBuffer(nullptr), __uuidof(IAudioClient2), nullptr,
          completionDelegate.Get(), &asyncOp);
      if (FAILED(hr)) {
        return -1;
      }
    }

    {
      HRESULT hr = completionDelegate->Wait(INFINITE);
      if (FAILED(hr)) {
        return -1;
      }
    }

    Microsoft::WRL::ComPtr<IUnknown> punkAudioInterface;
    {
      HRESULT hr = completionDelegate->GetActivateResult(&punkAudioInterface);
      if (FAILED(hr)) {
        return -1;
      }
    }

    {
      HRESULT hr =
          punkAudioInterface->QueryInterface(IID_PPV_ARGS(&_audioClient));
      if (FAILED(hr)) {
        return -1;
      }
    }

    {
      AudioClientProperties properties = {};
      properties.cbSize = sizeof(AudioClientProperties);
      properties.eCategory = AudioCategory_Communications;

      HRESULT hr = _audioClient->SetClientProperties(&properties);
      if (FAILED(hr)) {
        return -1;
      }
    }

    _mixerInitialized = true;
    return 0;
  }

  virtual bool MixerIsInitialized() const final { return _mixerInitialized; }

  //
  // Audio transport initialization
  //

  virtual int32_t TransportIsAvailable(bool& available) { return true; }

  virtual int32_t InitTransport() = 0;

  virtual bool TransportIsInitialized() const final {
    return _transportInitialized;
  }

  //
  // Audio transport control
  //

  virtual int32_t StartTransport() {
    if (!TransportIsInitialized()) {
      return -1;
    }

    if (_hThread != nullptr) {
      return 0;
    }

    if (Transporting()) {
      return 0;
    }

    assert(_hThread == nullptr);
    _hThread = CreateThread(nullptr, 0, WSAPIThread, this, 0, nullptr);
    if (_hThread == nullptr) {
      RTC_LOG(LS_ERROR) << "failed to create the thread";
      return -1;
    }

    // Set thread priority to highest possible
    SetThreadPriority(_hThread, THREAD_PRIORITY_TIME_CRITICAL);

    DWORD ret = WaitForSingleObject(_hStartedEvent, 1000);
    if (ret != WAIT_OBJECT_0) {
      RTC_LOG(LS_VERBOSE) << "did not start up properly";

      // Stop transport thread and clean up
      _transporting = true;
      StopTransport();

      return -1;
    }
    RTC_LOG(LS_VERBOSE) << "audio stream has now started...";

    _transporting = true;

    return 0;
  }

  virtual int32_t StopTransport() {
    if (!Transporting()) {
      return 0;
    }

    if (_hThread == nullptr) {
      RTC_LOG(LS_VERBOSE)
          << "no rendering stream is active => close down WASAPI only";
      _transporting = false;
      return 0;
    }

    // stop the driving thread...
    RTC_LOG(LS_VERBOSE) << "closing down the webrtc_core_audio_thread...";
    SetEvent(_hShutdownEvent);

    DWORD ret = WaitForSingleObject(_hThread, 2000);
    if (ret != WAIT_OBJECT_0) {
      // the thread did not stop as it should
      RTC_LOG(LS_ERROR) << "failed to close down webrtc_core_audio_thread";
      CloseHandle(_hThread);
      _hThread = nullptr;
      _transporting = false;
      _transportInitialized = false;
      _mixerInitialized = false;
      return -1;
    }

    RTC_LOG(LS_VERBOSE) << "webrtc_core_audio_thread is now closed";

    // to reset this event manually at each time we finish with it,
    // in case that the render thread has exited before StopPlayout(),
    // this event might be caught by the new render thread within same VoE
    // instance.
    ResetEvent(_hShutdownEvent);

    _transporting = false;
    _transportInitialized = false;
    _mixerInitialized = false;

    CloseHandle(_hThread);
    _hThread = nullptr;

    return 0;
  }

  virtual bool Transporting() const final { return _transporting; }

  //
  // Speaker volume controls
  //

  virtual int32_t VolumeIsAvailable(bool& available) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    float volume;
    HRESULT hr = _simpleAudioVolume->GetMasterVolume(&volume);
    available = SUCCEEDED(hr);

    return 0;
  }

  virtual int32_t SetVolume(uint32_t volume) {
    if (!TransportIsInitialized()) {
      return -1;
    }

    if (volume < MIN_CORE_SPEAKER_VOLUME || volume > MAX_CORE_SPEAKER_VOLUME) {
      return -1;
    }

    const float fLevel = volume / static_cast<float>(MAX_CORE_SPEAKER_VOLUME);
    HRESULT hr = _simpleAudioVolume->SetMasterVolume(fLevel, nullptr);
    if (FAILED(hr)) {
      return -1;
    }

    return 0;
  }

  virtual int32_t Volume(uint32_t& volume) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    float fLevel;
    HRESULT hr = _simpleAudioVolume->GetMasterVolume(&fLevel);
    if (FAILED(hr)) {
      return -1;
    }

    volume = fLevel * MAX_CORE_SPEAKER_VOLUME;

    return 0;
  }

  virtual int32_t MaxVolume(uint32_t& maxVolume) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    maxVolume = MAX_CORE_SPEAKER_VOLUME;

    return 0;
  }

  virtual int32_t MinVolume(uint32_t& minVolume) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    minVolume = MIN_CORE_SPEAKER_VOLUME;

    return 0;
  }

  //
  // Mute control
  //

  virtual int32_t MuteIsAvailable(bool& available) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    BOOL mute;
    HRESULT hr = _simpleAudioVolume->GetMute(&mute);
    available = SUCCEEDED(hr);

    return 0;
  }

  virtual int32_t SetMute(bool enable) {
    if (!TransportIsInitialized()) {
      return -1;
    }

    BOOL mute = enable;
    HRESULT hr = _simpleAudioVolume->SetMute(mute, nullptr);
    if (FAILED(hr)) {
      return -1;
    }

    return 0;
  }

  virtual int32_t Mute(bool& enabled) const {
    if (!TransportIsInitialized()) {
      return -1;
    }

    BOOL mute;
    HRESULT hr = _simpleAudioVolume->GetMute(&mute);
    if (FAILED(hr)) {
      return -1;
    }

    enabled = mute == TRUE;

    return 0;
  }

  //
  // Stereo support
  //

  virtual int32_t StereoIsAvailable(bool& available) const {
    if (!MixerIsInitialized()) {
      return -1;
    }

    available = true;

    return 0;
  }

  virtual int32_t SetStereo(bool enable) = 0;

  virtual int32_t Stereo(bool& enabled) const {
    enabled = _channels == 2;

    return 0;
  }
};

struct CaptureDeviceInternal
    : public AudioDeviceHelper<DeviceClass::DeviceClass_AudioCapture> {
 protected:
  const rtc::CriticalSection* _critSect;
  const uint32_t* _sndCardDelay;

  Microsoft::WRL::ComPtr<IAudioCaptureClient> _audioCaptureClient;

  LARGE_INTEGER _perfCounterFreq;
  double _perfCounterFactor;

  uint16_t _channelsPrioList[3];

 public:
  CaptureDeviceInternal(rtc::CriticalSection* critSect, uint32_t* sndCardDelay)
      : _critSect(critSect), _sndCardDelay(sndCardDelay) {
    _perfCounterFreq.QuadPart = 1;
    _perfCounterFactor = 0.0;

    // list of number of channels to use on recording side
    _channelsPrioList[0] = 2;  // stereo is prio 1
    _channelsPrioList[1] = 1;  // mono is prio 2
    _channelsPrioList[2] = 4;  // quad is prio 3
  }

  void AttachAudioBuffer(webrtc::AudioDeviceBuffer* audioBuffer) override {
    AudioDeviceHelper::AttachAudioBuffer(audioBuffer);
    audioBuffer->SetRecordingSampleRate(0);
    audioBuffer->SetRecordingChannels(0);
  }

  DWORD Transport() override {
    bool keepRecording = true;
    HANDLE waitArray[2] = {_hShutdownEvent, _hSamplesReadyEvent};
    HRESULT hr = S_OK;

    LARGE_INTEGER t1;

    BYTE* syncBuffer = nullptr;
    UINT32 syncBufIndex = 0;

    _samples = 0;

    // Initialize COM as MTA in this thread.
    Microsoft::WRL::Wrappers::RoInitializeWrapper roInitializeWrapper(
        RO_INIT_MULTITHREADED);
    if (FAILED(roInitializeWrapper)) {
      RTC_LOG(LS_ERROR) << "failed to initialize COM in capture thread";
      return 1;
    }

    rtc::SetCurrentThreadName("webrtc_core_audio_capture_thread");

    // Get size of capturing buffer (length is expressed as the number of audio
    // frames the buffer can hold). This value is fixed during the capturing
    // session.
    //
    UINT32 bufferLength = 0;
    if (_audioClient == nullptr) {
      RTC_LOG(LS_ERROR)
          << "input state has been modified before capture loop starts.";
      return 1;
    }
    hr = _audioClient->GetBufferSize(&bufferLength);
    EXIT_ON_ERROR(hr);
    RTC_LOG(LS_VERBOSE) << "[CAPT] size of buffer       : " << bufferLength;

    // Allocate memory for sync buffer.
    // It is used for compensation between native 44.1 and internal 44.0 and
    // for cases when the capture buffer is larger than 10ms.
    //
    const UINT32 syncBufferSize = 2 * (bufferLength * _audioFrameSize);
    syncBuffer = new BYTE[syncBufferSize];
    if (syncBuffer == nullptr) {
      return (DWORD)E_POINTER;
    }
    RTC_LOG(LS_VERBOSE) << "[CAPT] size of sync buffer  : " << syncBufferSize
                        << " [bytes]";

    // Get maximum latency for the current stream (will not change for the
    // lifetime of the IAudioClient object).
    //
    REFERENCE_TIME latency;
    _audioClient->GetStreamLatency(&latency);
    RTC_LOG(LS_VERBOSE) << "[CAPT] max stream latency   : " << (DWORD)latency
                        << " (" << (double)(latency / 10000.0) << " ms)";

    // Get the length of the periodic interval separating successive processing
    // passes by the audio engine on the data in the endpoint buffer.
    //
    REFERENCE_TIME devPeriod = 0;
    REFERENCE_TIME devPeriodMin = 0;
    _audioClient->GetDevicePeriod(&devPeriod, &devPeriodMin);
    RTC_LOG(LS_VERBOSE) << "[CAPT] device period        : " << (DWORD)devPeriod
                        << " (" << (double)(devPeriod / 10000.0) << " ms)";

    double extraDelayMS = (double)((latency + devPeriod) / 10000.0);
    RTC_LOG(LS_VERBOSE) << "[CAPT] extraDelayMS         : " << extraDelayMS;

    double endpointBufferSizeMS =
        10.0 * ((double)bufferLength / (double)_blockSize);
    RTC_LOG(LS_VERBOSE) << "[CAPT] endpointBufferSizeMS : "
                        << endpointBufferSizeMS;

    // Start up the capturing stream.
    //
    hr = _audioClient->Start();
    EXIT_ON_ERROR(hr);

    // Set event which will ensure that the calling thread modifies the
    // recording state to true.
    //
    SetEvent(_hStartedEvent);

    // >> ---------------------------- THREAD LOOP ----------------------------

    while (keepRecording) {
      // Wait for a capture notification event or a shutdown event
      DWORD waitResult = WaitForMultipleObjects(2, waitArray, FALSE, 500);
      switch (waitResult) {
        case WAIT_OBJECT_0 + 0:  // _hShutdownCaptureEvent
          keepRecording = false;
          break;
        case WAIT_OBJECT_0 + 1:  // _hCaptureSamplesReadyEvent
          break;
        case WAIT_TIMEOUT:  // timeout notification
          RTC_LOG(LS_WARNING) << "capture event timed out after 0.5 seconds";
          goto Exit;
        default:  // unexpected error
          RTC_LOG(LS_WARNING) << "unknown wait termination on capture side";
          goto Exit;
      }

      while (keepRecording) {
        rtc::CritScope lock(_critSect);

        BYTE* pData = 0;
        UINT32 framesAvailable = 0;
        DWORD flags = 0;
        UINT64 recTime = 0;
        UINT64 recPos = 0;

        // Sanity check to ensure that essential states are not modified
        // during the unlocked period.
        if (_audioCaptureClient == nullptr || _audioClient == nullptr) {
          RTC_LOG(LS_ERROR)
              << "input state has been modified during unlocked period";
          goto Exit;
        }

        //  Find out how much capture data is available
        //
        hr = _audioCaptureClient->GetBuffer(
            &pData,            // packet which is ready to be read by used
            &framesAvailable,  // #frames in the captured packet (can be zero)
            &flags,            // support flags (check)
            &recPos,    // device position of first audio frame in data packet
            &recTime);  // value of performance counter at the time of recording
                        // the first audio frame

        if (SUCCEEDED(hr)) {
          if (AUDCLNT_S_BUFFER_EMPTY == hr) {
            // Buffer was empty => start waiting for a new capture notification
            // event
            break;
          }

          if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
            // Treat all of the data in the packet as silence and ignore the
            // actual data values.
            RTC_LOG(LS_WARNING) << "AUDCLNT_BUFFERFLAGS_SILENT";
            pData = nullptr;
          }

          assert(framesAvailable != 0);

          if (pData) {
            CopyMemory(&syncBuffer[syncBufIndex * _audioFrameSize], pData,
                       framesAvailable * _audioFrameSize);
          } else {
            ZeroMemory(&syncBuffer[syncBufIndex * _audioFrameSize],
                       framesAvailable * _audioFrameSize);
          }
          assert(syncBufferSize >= (syncBufIndex * _audioFrameSize) +
                                       framesAvailable * _audioFrameSize);

          // Release the capture buffer
          //
          hr = _audioCaptureClient->ReleaseBuffer(framesAvailable);
          EXIT_ON_ERROR(hr);

          _samples += framesAvailable;
          syncBufIndex += framesAvailable;

          QueryPerformanceCounter(&t1);

          // Get the current recording and playout delay.
          uint32_t sndCardRecDelay = (uint32_t)(
              ((((UINT64)t1.QuadPart * _perfCounterFactor) - recTime) / 10000) +
              (10 * syncBufIndex) / _blockSize - 10);
          uint32_t sndCardPlayDelay = *_sndCardDelay;

          while (syncBufIndex >= _blockSize) {
            if (_pAudioBuffer) {
              _pAudioBuffer->SetRecordedBuffer((const int8_t*)syncBuffer,
                                               _blockSize);
              _pAudioBuffer->SetVQEData(sndCardPlayDelay, sndCardRecDelay);

              _pAudioBuffer->SetTypingStatus(false);

              _pAudioBuffer->DeliverRecordedData();

              // Sanity check to ensure that essential states are not modified
              // during the unlocked period
              if (_audioCaptureClient == nullptr || _audioClient == nullptr) {
                RTC_LOG(LS_ERROR) << "input state has been modified during"
                                  << " unlocked period";
                goto Exit;
              }
            }

            // store remaining data which was not able to deliver as 10ms
            // segment
            MoveMemory(&syncBuffer[0],
                       &syncBuffer[_blockSize * _audioFrameSize],
                       (syncBufIndex - _blockSize) * _audioFrameSize);
            syncBufIndex -= _blockSize;
            sndCardRecDelay -= 10;
          }
        } else {
          // If GetBuffer returns AUDCLNT_E_BUFFER_ERROR, the thread consuming
          // the audio samples must wait for the next processing pass. The
          // client might benefit from keeping a count of the failed GetBuffer
          // calls. If GetBuffer returns this error repeatedly, the client can
          // start a new processing loop after shutting down the current client
          // by calling IAudioClient::Stop, IAudioClient::Reset, and releasing
          // the audio client.
          RTC_LOG(LS_ERROR)
              << "IAudioCaptureClient::GetBuffer returned"
              << " AUDCLNT_E_BUFFER_ERROR, hr = 0x" << rtc::ToHex(hr);
          goto Exit;
        }
      }
    }

    // ---------------------------- THREAD LOOP ---------------------------- <<

    if (_audioClient) {
      hr = _audioClient->Stop();
    }

  Exit:
    if (FAILED(hr)) {
      _audioClient->Stop();
      _TraceCOMError(hr);
    }

    if (keepRecording) {
      rtc::CritScope lock(_critSect);

      if (_audioClient != nullptr) {
        hr = _audioClient->Stop();
        if (FAILED(hr)) {
          _TraceCOMError(hr);
        }
        hr = _audioClient->Reset();
        if (FAILED(hr)) {
          _TraceCOMError(hr);
        }
      }

      RTC_LOG(LS_ERROR)
          << "Recording error: capturing thread has ended pre-maturely";
    } else {
      RTC_LOG(LS_VERBOSE) << "_Capturing thread is now terminated properly";
    }

    if (syncBuffer) {
      delete[] syncBuffer;
    }

    return (DWORD)hr;
  }

  int32_t InitTransport() override {
    if (Transporting()) {
      return -1;
    }

    if (TransportIsInitialized()) {
      return 0;
    }

    if (QueryPerformanceFrequency(&_perfCounterFreq) == 0) {
      return -1;
    }
    _perfCounterFactor = 10000000.0 / (double)_perfCounterFreq.QuadPart;

    if (GetDevice() == nullptr) {
      return -1;
    }

    // Initialize the microphone (devices might have been added or removed)
    if (InitMixer() == -1) {
      RTC_LOG(LS_WARNING) << "InitMixer() failed";
    }

    // Ensure that the updated capturing endpoint device is valid
    if (GetDevice() == nullptr) {
      return -1;
    }

    HRESULT hr = S_OK;
    WAVEFORMATEX* pWfxIn = nullptr;
    WAVEFORMATEXTENSIBLE Wfx = WAVEFORMATEXTENSIBLE();
    WAVEFORMATEX* pWfxClosestMatch = nullptr;

    // Retrieve the stream format that the audio engine uses for its internal
    // processing (mixing) of shared-mode streams.
    hr = _audioClient->GetMixFormat(&pWfxIn);
    if (SUCCEEDED(hr)) {
      RTC_LOG(LS_VERBOSE) << "Audio Engine's current capturing mix format:";
      // format type
      RTC_LOG(LS_VERBOSE) << "wFormatTag     : 0x"
                          << rtc::ToHex(pWfxIn->wFormatTag) << " ("
                          << pWfxIn->wFormatTag << ")";
      // number of channels (i.e. mono, stereo...)
      RTC_LOG(LS_VERBOSE) << "nChannels      : " << pWfxIn->nChannels;
      // sample rate
      RTC_LOG(LS_VERBOSE) << "nSamplesPerSec : " << pWfxIn->nSamplesPerSec;
      // for buffer estimation
      RTC_LOG(LS_VERBOSE) << "nAvgBytesPerSec: " << pWfxIn->nAvgBytesPerSec;
      // block size of data
      RTC_LOG(LS_VERBOSE) << "nBlockAlign    : " << pWfxIn->nBlockAlign;
      // number of bits per sample of mono data
      RTC_LOG(LS_VERBOSE) << "wBitsPerSample : " << pWfxIn->wBitsPerSample;
      RTC_LOG(LS_VERBOSE) << "cbSize         : " << pWfxIn->cbSize;
    }

    // Set wave format
    Wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    Wfx.Format.wBitsPerSample = 16;
    Wfx.Format.cbSize = 22;
    Wfx.dwChannelMask = 0;
    Wfx.Samples.wValidBitsPerSample = Wfx.Format.wBitsPerSample;
    Wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    const int freqs[7] = {48000, 44100, 16000, 96000, 32000, 24000, 8000};
    hr = S_FALSE;

    // Iterate over frequencies and channels, in order of priority
    for (unsigned int freq = 0; freq < sizeof(freqs) / sizeof(freqs[0]);
         freq++) {
      for (unsigned int chan = 0;
           chan < sizeof(_channelsPrioList) / sizeof(_channelsPrioList[0]);
           chan++) {
        Wfx.Format.nChannels = _channelsPrioList[chan];
        Wfx.Format.nSamplesPerSec = freqs[freq];
        Wfx.Format.nBlockAlign =
            Wfx.Format.nChannels * Wfx.Format.wBitsPerSample / 8;
        Wfx.Format.nAvgBytesPerSec =
            Wfx.Format.nSamplesPerSec * Wfx.Format.nBlockAlign;
        // If the method succeeds and the audio endpoint device supports the
        // specified stream format, it returns S_OK. If the method succeeds and
        // provides a closest match to the specified format, it returns S_FALSE.
        hr = _audioClient->IsFormatSupported(
            AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&Wfx, &pWfxClosestMatch);
        if (hr == S_OK) {
          break;
        } else {
          if (pWfxClosestMatch) {
            RTC_LOG(INFO) << "nChannels=" << Wfx.Format.nChannels
                          << ", nSamplesPerSec=" << Wfx.Format.nSamplesPerSec
                          << " is not supported. Closest match: "
                          << "nChannels=" << pWfxClosestMatch->nChannels
                          << ", nSamplesPerSec="
                          << pWfxClosestMatch->nSamplesPerSec;
            CoTaskMemFree(pWfxClosestMatch);
            pWfxClosestMatch = nullptr;
          } else {
            RTC_LOG(INFO) << "nChannels=" << Wfx.Format.nChannels
                          << ", nSamplesPerSec=" << Wfx.Format.nSamplesPerSec
                          << " is not supported. No closest match.";
          }
        }
      }
      if (hr == S_OK)
        break;
    }

    if (hr == S_OK) {
      _audioFrameSize = Wfx.Format.nBlockAlign;
      _sampleRate = Wfx.Format.nSamplesPerSec;
      _blockSize = Wfx.Format.nSamplesPerSec / 100;
      _channels = Wfx.Format.nChannels;

      RTC_LOG(LS_VERBOSE) << "VoE selected this capturing format:";
      RTC_LOG(LS_VERBOSE) << "wFormatTag        : 0x"
                          << rtc::ToHex(Wfx.Format.wFormatTag) << " ("
                          << Wfx.Format.wFormatTag << ")";
      RTC_LOG(LS_VERBOSE) << "nChannels         : " << Wfx.Format.nChannels;
      RTC_LOG(LS_VERBOSE) << "nSamplesPerSec    : "
                          << Wfx.Format.nSamplesPerSec;
      RTC_LOG(LS_VERBOSE) << "nAvgBytesPerSec   : "
                          << Wfx.Format.nAvgBytesPerSec;
      RTC_LOG(LS_VERBOSE) << "nBlockAlign       : " << Wfx.Format.nBlockAlign;
      RTC_LOG(LS_VERBOSE) << "wBitsPerSample    : "
                          << Wfx.Format.wBitsPerSample;
      RTC_LOG(LS_VERBOSE) << "cbSize            : " << Wfx.Format.cbSize;
      RTC_LOG(LS_VERBOSE) << "Additional settings:";
      RTC_LOG(LS_VERBOSE) << "_recAudioFrameSize: " << _audioFrameSize;
      RTC_LOG(LS_VERBOSE) << "_recBlockSize     : " << _blockSize;
      RTC_LOG(LS_VERBOSE) << "_recChannels      : " << _channels;
    }

    // Create a capturing stream.
    hr = _audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,  // share Audio Engine with other applications
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK |  // processing of the audio buffer
                                             // by the client will be event
                                             // driven
            AUDCLNT_STREAMFLAGS_NOPERSIST,   // volume and mute settings for an
                                             // audio session will not persist
                                             // across system restarts
        0,                    // required for event-driven shared mode
        0,                    // periodicity
        (WAVEFORMATEX*)&Wfx,  // selected wave format
        nullptr);             // session GUID

    if (hr != S_OK) {
      RTC_LOG(LS_ERROR) << "IAudioClient::Initialize() failed:";
    }
    EXIT_ON_ERROR(hr);

    if (_pAudioBuffer) {
      // Update the audio buffer with the selected parameters
      _pAudioBuffer->SetRecordingSampleRate(_sampleRate);
      _pAudioBuffer->SetRecordingChannels((uint8_t)_channels);
    } else {
      // We can enter this state during CoreAudioIsSupported() when no
      // AudioDeviceImplementation has been created, hence the AudioDeviceBuffer
      // does not exist. It is OK to end up here since we don't initiate any
      // media in CoreAudioIsSupported().
      RTC_LOG(LS_VERBOSE)
          << "AudioDeviceBuffer must be attached before streaming can start";
    }

    // Get the actual size of the shared (endpoint buffer).
    // Typical value is 960 audio frames <=> 20ms @ 48kHz sample rate.
    UINT bufferFrameCount(0);
    hr = _audioClient->GetBufferSize(&bufferFrameCount);
    if (SUCCEEDED(hr)) {
      RTC_LOG(LS_VERBOSE) << "IAudioClient::GetBufferSize() => "
                          << bufferFrameCount << " (<=> "
                          << bufferFrameCount * _audioFrameSize << " bytes)";
    }

    // Set the event handle that the system signals when an audio buffer is
    // ready to be processed by the client.
    hr = _audioClient->SetEventHandle(_hSamplesReadyEvent);
    EXIT_ON_ERROR(hr);

    // Get an IAudioCaptureClient interface.
    hr = _audioClient->GetService(__uuidof(IAudioCaptureClient),
                                  (void**)&_audioCaptureClient);
    EXIT_ON_ERROR(hr);

    // Mark capture side as initialized
    _transportInitialized = true;

    CoTaskMemFree(pWfxIn);
    CoTaskMemFree(pWfxClosestMatch);

    RTC_LOG(LS_VERBOSE) << "capture side is now initialized";
    return 0;

  Exit:
    _TraceCOMError(hr);
    CoTaskMemFree(pWfxIn);
    CoTaskMemFree(pWfxClosestMatch);
    return -1;
  }

  int32_t SetStereo(bool enable) override {
    if (!MixerIsInitialized()) {
      return -1;
    }

    if (enable) {
      _channelsPrioList[0] = 2;  // try stereo first
      _channelsPrioList[1] = 1;
      _channels = 2;
    } else {
      _channelsPrioList[0] = 1;  // try mono first
      _channelsPrioList[1] = 2;
      _channels = 1;
    }

    return 0;
  }
};

struct RenderDeviceInternal
    : public AudioDeviceHelper<DeviceClass::DeviceClass_AudioRender> {
 protected:
  const rtc::CriticalSection* _critSect;

  Microsoft::WRL::ComPtr<IAudioRenderClient> _audioRenderClient;

  uint32_t _deviceSampleRate;
  uint32_t _deviceBlockSize;

  uint16_t _channelsPrioList[2];

 public:
  uint32_t _sndCardDelay;

  RenderDeviceInternal(rtc::CriticalSection* critSect) : _critSect(critSect) {
    // list of number of channels to use on recording side
    _channelsPrioList[0] = 2;  // stereo is prio 1
    _channelsPrioList[1] = 1;  // mono is prio 2
  }

  void AttachAudioBuffer(webrtc::AudioDeviceBuffer* audioBuffer) override {
    AudioDeviceHelper::AttachAudioBuffer(audioBuffer);
    audioBuffer->SetPlayoutSampleRate(0);
    audioBuffer->SetPlayoutChannels(0);
  }

  DWORD Transport() override {
    bool keepPlaying = true;
    HANDLE waitArray[2] = {_hShutdownEvent, _hSamplesReadyEvent};
    HRESULT hr = S_OK;

    // Initialize COM as MTA in this thread.
    Microsoft::WRL::Wrappers::RoInitializeWrapper roInitializeWrapper(
        RO_INIT_MULTITHREADED);
    if (FAILED(roInitializeWrapper)) {
      RTC_LOG(LS_ERROR) << "failed to initialize COM in render thread";
      return 1;
    }

    rtc::SetCurrentThreadName("webrtc_core_audio_render_thread");

    IAudioClock* clock = nullptr;

    // Get size of rendering buffer (length is expressed as the number of audio
    // frames the buffer can hold). This value is fixed during the rendering
    // session.
    //
    UINT32 bufferLength = 0;
    hr = _audioClient->GetBufferSize(&bufferLength);
    EXIT_ON_ERROR(hr);
    RTC_LOG(LS_VERBOSE) << "[REND] size of buffer       : " << bufferLength;

    // Get the number of frames of padding (queued up to play) in the
    // endpoint buffer.
    UINT32 padding = 0;
    hr = _audioClient->GetCurrentPadding(&padding);
    EXIT_ON_ERROR(hr);
    RTC_LOG(LS_VERBOSE) << "[REND] padding       : " << padding;

    // Get maximum latency for the current stream (will not change for the
    // lifetime  of the IAudioClient object).
    //
    REFERENCE_TIME latency;
    _audioClient->GetStreamLatency(&latency);
    RTC_LOG(LS_VERBOSE) << "[REND] max stream latency   : " << (DWORD)latency
                        << " (" << (double)(latency / 10000.0) << " ms)";

    // Get the length of the periodic interval separating successive processing
    // passes by the audio engine on the data in the endpoint buffer.
    //
    // The period between processing passes by the audio engine is fixed for a
    // particular audio endpoint device and represents the smallest processing
    // quantum for the audio engine. This period plus the stream latency between
    // the buffer and endpoint device represents the minimum possible latency
    // that an audio application can achieve. Typical value: 100000 <=> 0.01 sec
    // = 10ms.
    //
    REFERENCE_TIME devPeriod = 0;
    REFERENCE_TIME devPeriodMin = 0;
    _audioClient->GetDevicePeriod(&devPeriod, &devPeriodMin);
    RTC_LOG(LS_VERBOSE) << "[REND] device period        : " << (DWORD)devPeriod
                        << " (" << (double)(devPeriod / 10000.0) << " ms)";

    // Derive initial rendering delay.
    // Example: 10*(960/480) + 15 = 20 + 15 = 35ms
    //
    int playout_delay =
        10 * (bufferLength / _blockSize) + (int)((latency + devPeriod) / 10000);
    _sndCardDelay = playout_delay;
    _samples = 0;
    RTC_LOG(LS_VERBOSE) << "[REND] initial delay        : " << playout_delay;

    double endpointBufferSizeMS =
        10.0 * ((double)bufferLength / (double)_deviceBlockSize);
    RTC_LOG(LS_VERBOSE) << "[REND] endpointBufferSizeMS : "
                        << endpointBufferSizeMS;

    // Derive the amount of available space in the output buffer
    uint32_t framesAvailable = bufferLength - padding;

    // Before starting the stream, fill the rendering buffer with silence.
    //
    BYTE* pData = nullptr;
    hr = _audioRenderClient->GetBuffer(framesAvailable, &pData);
    EXIT_ON_ERROR(hr);

    hr = _audioRenderClient->ReleaseBuffer(framesAvailable,
                                           AUDCLNT_BUFFERFLAGS_SILENT);
    EXIT_ON_ERROR(hr);

    _samples += bufferLength;

    hr = _audioClient->GetService(__uuidof(IAudioClock), (void**)&clock);
    if (FAILED(hr)) {
      RTC_LOG(LS_WARNING)
          << "failed to get IAudioClock interface from the IAudioClient";
    }

    // Start up the rendering audio stream.
    hr = _audioClient->Start();
    EXIT_ON_ERROR(hr);

    // Set event which will ensure that the calling thread modifies the playing
    // state to true.
    //
    SetEvent(_hStartedEvent);

    // >> ------------------ THREAD LOOP ------------------

    while (keepPlaying) {
      // Wait for a render notification event or a shutdown event
      DWORD waitResult = WaitForMultipleObjects(2, waitArray, FALSE, 500);
      switch (waitResult) {
        case WAIT_OBJECT_0 + 0:  // _hShutdownRenderEvent
          keepPlaying = false;
          break;
        case WAIT_OBJECT_0 + 1:  // _hRenderSamplesReadyEvent
          break;
        case WAIT_TIMEOUT:  // timeout notification
          RTC_LOG(LS_WARNING) << "render event timed out after 0.5 seconds";
          goto Exit;
        default:  // unexpected error
          RTC_LOG(LS_WARNING) << "unknown wait termination on render side";
          goto Exit;
      }

      while (keepPlaying) {
        rtc::CritScope lock(_critSect);

        // Sanity check to ensure that essential states are not modified
        // during the unlocked period.
        if (_audioRenderClient == nullptr || _audioClient == nullptr) {
          RTC_LOG(LS_ERROR)
              << "output state has been modified during unlocked period";
          goto Exit;
        }

        // Get the number of frames of padding (queued up to play) in the
        // endpoint buffer.
        hr = _audioClient->GetCurrentPadding(&padding);
        EXIT_ON_ERROR(hr);

        // Derive the amount of available space in the output buffer
        framesAvailable = bufferLength - padding;

        // Do we have 10 ms available in the render buffer?
        if (framesAvailable < _blockSize) {
          // Not enough space in render buffer to store next render packet.
          break;
        }

        // Write n*10ms buffers to the render buffer
        const uint32_t n10msBuffers = (framesAvailable / _blockSize);
        for (uint32_t n = 0; n < n10msBuffers; n++) {
          // Get pointer (i.e., grab the buffer) to next space in the shared
          // render buffer.
          hr = _audioRenderClient->GetBuffer(_blockSize, &pData);
          EXIT_ON_ERROR(hr);

          if (_pAudioBuffer) {
            // Request data to be played out (#bytes =
            // _playBlockSize*_audioFrameSize)
            int32_t nSamples = _pAudioBuffer->RequestPlayoutData(_blockSize);

            if (nSamples == -1) {
              RTC_LOG(LS_ERROR) << "failed to read data from render client";
              goto Exit;
            }

            // Sanity check to ensure that essential states are not modified
            // during the unlocked period
            if (_audioRenderClient == nullptr || _audioClient == nullptr) {
              RTC_LOG(LS_ERROR)
                  << "output state has been modified during unlocked"
                  << " period";
              goto Exit;
            }
            if (nSamples != static_cast<int32_t>(_blockSize)) {
              RTC_LOG(LS_WARNING) << "nSamples(" << nSamples
                                  << ") != _playBlockSize" << _blockSize << ")";
            }

            // Get the actual (stored) data
            nSamples = _pAudioBuffer->GetPlayoutData((int8_t*)pData);
          }

          DWORD dwFlags(0);
          hr = _audioRenderClient->ReleaseBuffer(_blockSize, dwFlags);
          // See http://msdn.microsoft.com/en-us/library/dd316605(VS.85).aspx
          // for more details regarding AUDCLNT_E_DEVICE_INVALIDATED.
          EXIT_ON_ERROR(hr);

          _samples += _blockSize;
        }

        // Check the current delay on the playout side.
        if (clock) {
          UINT64 pos = 0;
          UINT64 freq = 1;
          clock->GetPosition(&pos, nullptr);
          clock->GetFrequency(&freq);
          playout_delay = round(
              (double(_samples) / _deviceSampleRate - double(pos) / freq) *
              1000.0);
          _sndCardDelay = playout_delay;
        }
      }
    }

    // ------------------ THREAD LOOP ------------------ <<

    webrtc::SleepMs(static_cast<DWORD>(endpointBufferSizeMS + 0.5));
    hr = _audioClient->Stop();

  Exit:
    if (clock != nullptr) {
      clock->Release();
      clock = nullptr;
    }

    if (FAILED(hr)) {
      _audioClient->Stop();
      _TraceCOMError(hr);
    }

    if (keepPlaying) {
      rtc::CritScope lock(_critSect);

      if (_audioClient != nullptr) {
        hr = _audioClient->Stop();
        if (FAILED(hr)) {
          _TraceCOMError(hr);
        }
        hr = _audioClient->Reset();
        if (FAILED(hr)) {
          _TraceCOMError(hr);
        }
      }
      RTC_LOG(LS_ERROR)
          << "Playout error: rendering thread has ended pre-maturely";
    } else {
      RTC_LOG(LS_VERBOSE) << "_Rendering thread is now terminated properly";
    }

    return (DWORD)hr;
  }

  int32_t InitTransport() override {
    if (Transporting()) {
      return -1;
    }

    if (TransportIsInitialized()) {
      return 0;
    }

    if (GetDevice() == nullptr) {
      return -1;
    }

    // Initialize the microphone (devices might have been added or removed)
    if (InitMixer() == -1) {
      RTC_LOG(LS_WARNING) << "InitMixer() failed";
    }

    // Ensure that the updated rendering endpoint device is valid
    if (GetDevice() == nullptr) {
      return -1;
    }

    HRESULT hr = S_OK;
    WAVEFORMATEX* pWfxOut = nullptr;
    WAVEFORMATEX Wfx = WAVEFORMATEX();
    WAVEFORMATEX* pWfxClosestMatch = nullptr;

    // Retrieve the stream format that the audio engine uses for its internal
    // processing (mixing) of shared-mode streams.
    hr = _audioClient->GetMixFormat(&pWfxOut);
    if (SUCCEEDED(hr)) {
      RTC_LOG(LS_VERBOSE) << "Audio Engine's current rendering mix format:";
      // format type
      RTC_LOG(LS_VERBOSE) << "wFormatTag     : 0x"
                          << rtc::ToHex(pWfxOut->wFormatTag) << " ("
                          << pWfxOut->wFormatTag << ")";
      // number of channels (i.e. mono, stereo...)
      RTC_LOG(LS_VERBOSE) << "nChannels      : " << pWfxOut->nChannels;
      // sample rate
      RTC_LOG(LS_VERBOSE) << "nSamplesPerSec : " << pWfxOut->nSamplesPerSec;
      // for buffer estimation
      RTC_LOG(LS_VERBOSE) << "nAvgBytesPerSec: " << pWfxOut->nAvgBytesPerSec;
      // block size of data
      RTC_LOG(LS_VERBOSE) << "nBlockAlign    : " << pWfxOut->nBlockAlign;
      // number of bits per sample of mono data
      RTC_LOG(LS_VERBOSE) << "wBitsPerSample : " << pWfxOut->wBitsPerSample;
      RTC_LOG(LS_VERBOSE) << "cbSize         : " << pWfxOut->cbSize;
    }

    // Set wave format
    Wfx.wFormatTag = WAVE_FORMAT_PCM;
    Wfx.wBitsPerSample = 16;
    Wfx.cbSize = 0;

    const int freqs[7] = {48000, 44100, 16000, 96000, 32000, 24000, 8000};
    hr = S_FALSE;

    // Iterate over frequencies and channels, in order of priority
    for (unsigned int freq = 0; freq < sizeof(freqs) / sizeof(freqs[0]);
         freq++) {
      for (unsigned int chan = 0;
           chan < sizeof(_channelsPrioList) / sizeof(_channelsPrioList[0]);
           chan++) {
        Wfx.nChannels = _channelsPrioList[chan];
        Wfx.nSamplesPerSec = freqs[freq];
        Wfx.nBlockAlign = Wfx.nChannels * Wfx.wBitsPerSample / 8;
        Wfx.nAvgBytesPerSec = Wfx.nSamplesPerSec * Wfx.nBlockAlign;
        // If the method succeeds and the audio endpoint device supports the
        // specified stream format, it returns S_OK. If the method succeeds and
        // provides a closest match to the specified format, it returns S_FALSE.
        hr = _audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &Wfx,
                                             &pWfxClosestMatch);
        if (hr == S_OK) {
          break;
        } else {
          if (pWfxClosestMatch) {
            RTC_LOG(INFO) << "nChannels=" << Wfx.nChannels
                          << ", nSamplesPerSec=" << Wfx.nSamplesPerSec
                          << " is not supported. Closest match: "
                          << "nChannels=" << pWfxClosestMatch->nChannels
                          << ", nSamplesPerSec="
                          << pWfxClosestMatch->nSamplesPerSec;
            CoTaskMemFree(pWfxClosestMatch);
            pWfxClosestMatch = nullptr;
          } else {
            RTC_LOG(INFO) << "nChannels=" << Wfx.nChannels
                          << ", nSamplesPerSec=" << Wfx.nSamplesPerSec
                          << " is not supported. No closest match.";
          }
        }
      }
      if (hr == S_OK)
        break;
    }

    // TODO(andrew): what happens in the event of failure in the above loop?
    //   Is _ptrClientOut->Initialize expected to fail?
    //   Same in InitRecording().
    if (hr == S_OK) {
      _audioFrameSize = Wfx.nBlockAlign;
      // Block size is the number of samples each channel in 10ms.
      _blockSize = Wfx.nSamplesPerSec / 100;
      _sampleRate = Wfx.nSamplesPerSec;
      _deviceSampleRate = Wfx.nSamplesPerSec;  // The device itself continues to
                                               // run at 44.1 kHz.
      _deviceBlockSize = Wfx.nSamplesPerSec / 100;
      _channels = Wfx.nChannels;

      RTC_LOG(LS_VERBOSE) << "VoE selected this rendering format:";
      RTC_LOG(LS_VERBOSE) << "wFormatTag         : 0x"
                          << rtc::ToHex(Wfx.wFormatTag) << " ("
                          << Wfx.wFormatTag << ")";
      RTC_LOG(LS_VERBOSE) << "nChannels          : " << Wfx.nChannels;
      RTC_LOG(LS_VERBOSE) << "nSamplesPerSec     : " << Wfx.nSamplesPerSec;
      RTC_LOG(LS_VERBOSE) << "nAvgBytesPerSec    : " << Wfx.nAvgBytesPerSec;
      RTC_LOG(LS_VERBOSE) << "nBlockAlign        : " << Wfx.nBlockAlign;
      RTC_LOG(LS_VERBOSE) << "wBitsPerSample     : " << Wfx.wBitsPerSample;
      RTC_LOG(LS_VERBOSE) << "cbSize             : " << Wfx.cbSize;
      RTC_LOG(LS_VERBOSE) << "Additional settings:";
      RTC_LOG(LS_VERBOSE) << "_playAudioFrameSize: " << _audioFrameSize;
      RTC_LOG(LS_VERBOSE) << "_playBlockSize     : " << _blockSize;
      RTC_LOG(LS_VERBOSE) << "_playChannels      : " << _channels;
    }

    // Create a rendering stream.
    //
    // ****************************************************************************
    // For a shared-mode stream that uses event-driven buffering, the caller
    // must set both hnsPeriodicity and hnsBufferDuration to 0. The Initialize
    // method determines how large a buffer to allocate based on the scheduling
    // period of the audio engine. Although the client's buffer processing
    // thread is event driven, the basic buffer management process, as described
    // previously, is unaltered. Each time the thread awakens, it should call
    // IAudioClient::GetCurrentPadding to determine how much data to write to a
    // rendering buffer or read from a capture buffer. In contrast to the two
    // buffers that the Initialize method allocates for an exclusive-mode stream
    // that uses event-driven buffering, a shared-mode stream requires a single
    // buffer.
    // ****************************************************************************
    //
    REFERENCE_TIME hnsBufferDuration =
        0;  // ask for minimum buffer size (default)
    if (_deviceSampleRate == 44100) {
      // Ask for a larger buffer size (30ms) when using 44.1kHz as render rate.
      // There seems to be a larger risk of underruns for 44.1 compared
      // with the default rate (48kHz). When using default, we set the requested
      // buffer duration to 0, which sets the buffer to the minimum size
      // required by the engine thread. The actual buffer size can then be
      // read by GetBufferSize() and it is 20ms on most machines.
      hnsBufferDuration = 30 * 10000;
    }
    hr = _audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,  // share Audio Engine with other applications
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,  // processing of the audio buffer by
                                            // the client will be event driven
        hnsBufferDuration,  // requested buffer capacity as a time value (in
                            // 100-nanosecond units)
        0,                  // periodicity
        &Wfx,               // selected wave format
        nullptr);           // session GUID

    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR) << "IAudioClient::Initialize() failed:";
    }
    EXIT_ON_ERROR(hr);

    if (_pAudioBuffer) {
      // Update the audio buffer with the selected parameters
      _pAudioBuffer->SetPlayoutSampleRate(_sampleRate);
      _pAudioBuffer->SetPlayoutChannels((uint8_t)_channels);
    } else {
      // We can enter this state during CoreAudioIsSupported() when no
      // AudioDeviceImplementation has been created, hence the AudioDeviceBuffer
      // does not exist. It is OK to end up here since we don't initiate any
      // media in CoreAudioIsSupported().
      RTC_LOG(LS_VERBOSE)
          << "AudioDeviceBuffer must be attached before streaming can start";
    }

    // Get the actual size of the shared (endpoint buffer).
    // Typical value is 960 audio frames <=> 20ms @ 48kHz sample rate.
    UINT bufferFrameCount(0);
    hr = _audioClient->GetBufferSize(&bufferFrameCount);
    if (SUCCEEDED(hr)) {
      RTC_LOG(LS_VERBOSE) << "IAudioClient::GetBufferSize() => "
                          << bufferFrameCount << " (<=> "
                          << bufferFrameCount * _audioFrameSize << " bytes)";
    }

    // Set the event handle that the system signals when an audio buffer is
    // ready to be processed by the client.
    hr = _audioClient->SetEventHandle(_hSamplesReadyEvent);
    EXIT_ON_ERROR(hr);

    // Get an IAudioRenderClient interface.
    hr = _audioClient->GetService(__uuidof(IAudioRenderClient),
                                  (void**)&_audioRenderClient);
    EXIT_ON_ERROR(hr);

    // Get an ISimpleAudioVolume interface.
    hr = _audioClient->GetService(__uuidof(ISimpleAudioVolume),
                                  (void**)&_simpleAudioVolume);
    EXIT_ON_ERROR(hr);

    // Mark playout side as initialized
    _transportInitialized = true;

    CoTaskMemFree(pWfxOut);
    CoTaskMemFree(pWfxClosestMatch);

    RTC_LOG(LS_VERBOSE) << "render side is now initialized";
    return 0;

  Exit:
    _TraceCOMError(hr);
    CoTaskMemFree(pWfxOut);
    CoTaskMemFree(pWfxClosestMatch);
    return -1;
  }

  int32_t SetStereo(bool enable) override {
    if (!MixerIsInitialized()) {
      return -1;
    }

    if (enable) {
      _channelsPrioList[0] = 2;  // try stereo first
      _channelsPrioList[1] = 1;
      _channels = 2;
    } else {
      _channelsPrioList[0] = 1;  // try mono first
      _channelsPrioList[1] = 2;
      _channels = 1;
    }

    return 0;
  }
};

namespace webrtc {

struct AudioDeviceWindowsCoreInternal {
  rtc::CriticalSection _critSect;

  AudioDeviceBuffer* _pAudioBuffer = nullptr;
  bool _initialized = false;

  CaptureDeviceInternal* _pCaptureDeviceHelper = nullptr;
  RenderDeviceInternal* _pRenderDeviceHelper = nullptr;

  Microsoft::WRL::Wrappers::RoInitializeWrapper _roInitializeWrapper{
      RO_INIT_MULTITHREADED};
};

// ============================================================================
//                              Static Methods
// ============================================================================

// ----------------------------------------------------------------------------
//  CoreAudioIsSupported
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::CoreAudioIsSupported() {
  return true;
}

// ============================================================================
//                            Construction & Destruction
// ============================================================================

// ----------------------------------------------------------------------------
//  AudioDeviceWindowsCore() - ctor
// ----------------------------------------------------------------------------

AudioDeviceWindowsCore::AudioDeviceWindowsCore()
    : _internal(new AudioDeviceWindowsCoreInternal) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << " created";
  assert(SUCCEEDED(_internal->_roInitializeWrapper));
}

// ----------------------------------------------------------------------------
//  AudioDeviceWindowsCore() - dtor
// ----------------------------------------------------------------------------

AudioDeviceWindowsCore::~AudioDeviceWindowsCore() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << " destroyed";

  Terminate();

  delete _internal;
}

// ============================================================================
//                                     API
// ============================================================================

// ----------------------------------------------------------------------------
//  AttachAudioBuffer
// ----------------------------------------------------------------------------

void AudioDeviceWindowsCore::AttachAudioBuffer(AudioDeviceBuffer* audioBuffer) {
  _internal->_pAudioBuffer = audioBuffer;

  if (_internal->_pRenderDeviceHelper != nullptr) {
    _internal->_pRenderDeviceHelper->AttachAudioBuffer(
        _internal->_pAudioBuffer);
  }

  if (_internal->_pCaptureDeviceHelper != nullptr) {
    _internal->_pCaptureDeviceHelper->AttachAudioBuffer(
        _internal->_pAudioBuffer);
  }
}

// ----------------------------------------------------------------------------
//  ActiveAudioLayer
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::ActiveAudioLayer(
    AudioDeviceModule::AudioLayer& audioLayer) const {
  audioLayer = AudioDeviceModule::kWindowsCoreAudio;
  return 0;
}

// ----------------------------------------------------------------------------
//  Init
// ----------------------------------------------------------------------------

AudioDeviceGeneric::InitStatus AudioDeviceWindowsCore::Init() {
  rtc::CritScope lock(&_internal->_critSect);

  if (_internal->_initialized) {
    return InitStatus::OK;
  }

  if (FAILED(_internal->_roInitializeWrapper)) {
    return InitStatus::OTHER_ERROR;
  }

  _internal->_pRenderDeviceHelper =
      new (std::nothrow) RenderDeviceInternal(&_internal->_critSect);
  _internal->_pCaptureDeviceHelper = new (std::nothrow) CaptureDeviceInternal(
      &_internal->_critSect, &_internal->_pRenderDeviceHelper->_sndCardDelay);

  if (_internal->_pAudioBuffer != nullptr) {
    _internal->_pRenderDeviceHelper->AttachAudioBuffer(
        _internal->_pAudioBuffer);
    _internal->_pCaptureDeviceHelper->AttachAudioBuffer(
        _internal->_pAudioBuffer);
  }

  _internal->_initialized = true;

  return InitStatus::OK;
}

// ----------------------------------------------------------------------------
//  Terminate
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::Terminate() {
  rtc::CritScope lock(&_internal->_critSect);

  if (!_internal->_initialized) {
    return 0;
  }

  delete _internal->_pCaptureDeviceHelper;
  delete _internal->_pRenderDeviceHelper;

  _internal->_initialized = false;

  return 0;
}

// ----------------------------------------------------------------------------
//  Initialized
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::Initialized() const {
  return _internal->_initialized;
}

// ----------------------------------------------------------------------------
//  InitSpeaker
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::InitSpeaker() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->InitMixer();
}

// ----------------------------------------------------------------------------
//  InitMicrophone
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::InitMicrophone() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->InitMixer();
}

// ----------------------------------------------------------------------------
//  SpeakerIsInitialized
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::SpeakerIsInitialized() const {
  return _internal->_pRenderDeviceHelper->MixerIsInitialized();
}

// ----------------------------------------------------------------------------
//  MicrophoneIsInitialized
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::MicrophoneIsInitialized() const {
  return _internal->_pCaptureDeviceHelper->MixerIsInitialized();
}

// ----------------------------------------------------------------------------
//  SpeakerVolumeIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SpeakerVolumeIsAvailable(bool& available) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->VolumeIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetSpeakerVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetSpeakerVolume(uint32_t volume) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->SetVolume(volume);
}

// ----------------------------------------------------------------------------
//  SpeakerVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SpeakerVolume(uint32_t& volume) const {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->Volume(volume);
}

// ----------------------------------------------------------------------------
//  MaxSpeakerVolume
//
//  The internal range for Core Audio is 0.0 to 1.0, where 0.0 indicates
//  silence and 1.0 indicates full volume (no attenuation).
//  We add our (webrtc-internal) own max level to match the Wave API and
//  how it is used today in VoE.
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MaxSpeakerVolume(uint32_t& maxVolume) const {
  return _internal->_pRenderDeviceHelper->MaxVolume(maxVolume);
}

// ----------------------------------------------------------------------------
//  MinSpeakerVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MinSpeakerVolume(uint32_t& minVolume) const {
  return _internal->_pRenderDeviceHelper->MinVolume(minVolume);
}

// ----------------------------------------------------------------------------
//  SpeakerMuteIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SpeakerMuteIsAvailable(bool& available) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->MuteIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetSpeakerMute
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetSpeakerMute(bool enable) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->SetMute(enable);
}

// ----------------------------------------------------------------------------
//  SpeakerMute
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SpeakerMute(bool& enabled) const {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->Mute(enabled);
}

// ----------------------------------------------------------------------------
//  MicrophoneMuteIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MicrophoneMuteIsAvailable(bool& available) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->MuteIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetMicrophoneMute
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetMicrophoneMute(bool enable) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->SetMute(enable);
}

// ----------------------------------------------------------------------------
//  MicrophoneMute
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MicrophoneMute(bool& enabled) const {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->Mute(enabled);
}

// ----------------------------------------------------------------------------
//  StereoRecordingIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StereoRecordingIsAvailable(bool& available) {
  return _internal->_pCaptureDeviceHelper->StereoIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetStereoRecording
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetStereoRecording(bool enable) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->SetStereo(enable);
}

// ----------------------------------------------------------------------------
//  StereoRecording
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StereoRecording(bool& enabled) const {
  return _internal->_pCaptureDeviceHelper->Stereo(enabled);
}

// ----------------------------------------------------------------------------
//  StereoPlayoutIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StereoPlayoutIsAvailable(bool& available) {
  return _internal->_pRenderDeviceHelper->StereoIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetStereoPlayout
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetStereoPlayout(bool enable) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->SetStereo(enable);
}

// ----------------------------------------------------------------------------
//  StereoPlayout
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StereoPlayout(bool& enabled) const {
  return _internal->_pRenderDeviceHelper->Stereo(enabled);
}

// ----------------------------------------------------------------------------
//  MicrophoneVolumeIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MicrophoneVolumeIsAvailable(bool& available) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->VolumeIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  SetMicrophoneVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetMicrophoneVolume(uint32_t volume) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->SetVolume(volume);
}

// ----------------------------------------------------------------------------
//  MicrophoneVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MicrophoneVolume(uint32_t& volume) const {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->Volume(volume);
}

// ----------------------------------------------------------------------------
//  MaxMicrophoneVolume
//
//  The internal range for Core Audio is 0.0 to 1.0, where 0.0 indicates
//  silence and 1.0 indicates full volume (no attenuation).
//  We add our (webrtc-internal) own max level to match the Wave API and
//  how it is used today in VoE.
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MaxMicrophoneVolume(uint32_t& maxVolume) const {
  return _internal->_pCaptureDeviceHelper->MaxVolume(maxVolume);
}

// ----------------------------------------------------------------------------
//  MinMicrophoneVolume
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::MinMicrophoneVolume(uint32_t& minVolume) const {
  return _internal->_pCaptureDeviceHelper->MinVolume(minVolume);
}

// ----------------------------------------------------------------------------
//  PlayoutDevices
// ----------------------------------------------------------------------------

int16_t AudioDeviceWindowsCore::PlayoutDevices() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->Devices();
}

// ----------------------------------------------------------------------------
//  SetPlayoutDevice I (II)
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetPlayoutDevice(uint16_t index) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->SetDevice(index);
}

// ----------------------------------------------------------------------------
//  SetPlayoutDevice II (II)
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetPlayoutDevice(
    AudioDeviceModule::WindowsDeviceType device) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->SetDevice(device);
}

// ----------------------------------------------------------------------------
//  PlayoutDeviceName
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::PlayoutDeviceName(
    uint16_t index,
    char name[kAdmMaxDeviceNameSize],
    char guid[kAdmMaxGuidSize]) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->DeviceName(index, name, guid);
}

// ----------------------------------------------------------------------------
//  RecordingDeviceName
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::RecordingDeviceName(
    uint16_t index,
    char name[kAdmMaxDeviceNameSize],
    char guid[kAdmMaxGuidSize]) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->DeviceName(index, name, guid);
}

// ----------------------------------------------------------------------------
//  RecordingDevices
// ----------------------------------------------------------------------------

int16_t AudioDeviceWindowsCore::RecordingDevices() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->Devices();
}

// ----------------------------------------------------------------------------
//  SetRecordingDevice I (II)
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetRecordingDevice(uint16_t index) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->SetDevice(index);
}

// ----------------------------------------------------------------------------
//  SetRecordingDevice II (II)
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::SetRecordingDevice(
    AudioDeviceModule::WindowsDeviceType device) {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->SetDevice(device);
}

// ----------------------------------------------------------------------------
//  PlayoutIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::PlayoutIsAvailable(bool& available) {
  return _internal->_pRenderDeviceHelper->TransportIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  RecordingIsAvailable
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::RecordingIsAvailable(bool& available) {
  return _internal->_pCaptureDeviceHelper->TransportIsAvailable(available);
}

// ----------------------------------------------------------------------------
//  InitPlayout
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::InitPlayout() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->InitTransport();
}

// ----------------------------------------------------------------------------
//  InitRecording
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::InitRecording() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->InitTransport();
}

// ----------------------------------------------------------------------------
//  StartRecording
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StartRecording() {
  rtc::CritScope critScoped(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->StartTransport();
}

// ----------------------------------------------------------------------------
//  StopRecording
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StopRecording() {
  rtc::CritScope lock(&_internal->_critSect);
  return _internal->_pCaptureDeviceHelper->StopTransport();
}

// ----------------------------------------------------------------------------
//  RecordingIsInitialized
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::RecordingIsInitialized() const {
  return _internal->_pCaptureDeviceHelper->TransportIsInitialized();
}

// ----------------------------------------------------------------------------
//  Recording
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::Recording() const {
  return _internal->_pCaptureDeviceHelper->Transporting();
}

// ----------------------------------------------------------------------------
//  PlayoutIsInitialized
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::PlayoutIsInitialized() const {
  return _internal->_pRenderDeviceHelper->TransportIsInitialized();
}

// ----------------------------------------------------------------------------
//  StartPlayout
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StartPlayout() {
  rtc::CritScope critScoped(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->StartTransport();
}

// ----------------------------------------------------------------------------
//  StopPlayout
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::StopPlayout() {
  rtc::CritScope critScoped(&_internal->_critSect);
  return _internal->_pRenderDeviceHelper->StopTransport();
}

// ----------------------------------------------------------------------------
//  PlayoutDelay
// ----------------------------------------------------------------------------

int32_t AudioDeviceWindowsCore::PlayoutDelay(uint16_t& delayMS) const {
  rtc::CritScope critScoped(&_internal->_critSect);
  delayMS =
      static_cast<uint16_t>(_internal->_pRenderDeviceHelper->_sndCardDelay);
  return 0;
}

// ----------------------------------------------------------------------------
//  Playing
// ----------------------------------------------------------------------------

bool AudioDeviceWindowsCore::Playing() const {
  return _internal->_pRenderDeviceHelper->Transporting();
}

bool AudioDeviceWindowsCore::BuiltInAECIsAvailable() const {
  return false;
}

int32_t AudioDeviceWindowsCore::EnableBuiltInAEC(bool enable) {
  return -1;
}
}  // namespace webrtc
