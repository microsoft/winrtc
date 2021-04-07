/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_DEVICE_AUDIO_DEVICE_CORE_WIN_H_
#define AUDIO_DEVICE_AUDIO_DEVICE_CORE_WIN_H_

#include <audioclient.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <windows.foundation.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/wrappers/corewrappers.h>

#include <memory>

#include "api/scoped_refptr.h"
#include "modules/audio_device/audio_device_generic.h"
#include "rtc_base/critical_section.h"
#include "rtc_base/logging.h"
#include "rtc_base/win32.h"

#define THR(expr)   \
  do {              \
    hr = (expr);    \
    if (FAILED(hr)) \
      goto Cleanup; \
  } while (false)

HRESULT WaitForASyncWithEvent(
    _In_ IAsyncInfo* async_info,
    _In_ std::shared_ptr<HANDLE> event_completed_handle_ptr,
    _In_ DWORD timeout_ms);

template <typename T>
HRESULT WaitForAsyncOperation(
    _In_ ::ABI::Windows::Foundation::IAsyncOperation<T>* p_async_op) {
  using ::ABI::Windows::Foundation::AsyncStatus;
  using ::ABI::Windows::Foundation::IAsyncInfo;
  using ::ABI::Windows::Foundation::IAsyncOperation;
  using ::ABI::Windows::Foundation::IAsyncOperationCompletedHandler;
  using ::Microsoft::WRL::Callback;
  using ::Microsoft::WRL::Wrappers::Event;

  HRESULT hr = S_OK;
  HRESULT hr_async_error = S_OK;
  const DWORD timeout_ms = 180000;
  ComPtr<IAsyncOperation<T>> async_op(p_async_op);
  ComPtr<IAsyncInfo> async_info;
  std::shared_ptr<HANDLE> event_completed_handle_ptr = NULL;

  APTTYPE apt_type;
  APTTYPEQUALIFIER apt_qualifier;
  hr = CoGetApartmentType(&apt_type, &apt_qualifier);
  // Please make the caller of this API run on a MTA appartment type.
  // The caller shouldn't be running on the UI thread (STA).
  if (SUCCEEDED(hr) && (apt_type != APTTYPE_MTA)) {
    RTC_LOG(LS_ERROR) << "Waiting in a non-MTA thread. Deadlocks might occur.";
  }

  // IAsyncInfo::get_Status is needed to check if operation started and if any
  // errors happened.
  if (SUCCEEDED(hr)) {
    hr = async_op.template As<IAsyncInfo>(&async_info);
  }

  // Creates the Event to be used to block and suspend until the async
  // operation finishes.
  if (SUCCEEDED(hr)) {
    // Using raw HANDLE because WRL::Wrappers::Event doesn't always work across
    // threads and there is no agile option.
    HANDLE event_completed_handle =
        ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET,
                        WRITE_OWNER | EVENT_ALL_ACCESS);
    if (event_completed_handle) {
      hr = S_OK;
      event_completed_handle_ptr = std::shared_ptr<HANDLE>(
          new HANDLE{event_completed_handle}, [](HANDLE* h) {
            ::CloseHandle(*h);
            delete h;
          });
    } else {
      hr = HRESULT_FROM_WIN32(GetLastError());
    }
  }

  if (SUCCEEDED(hr)) {
    hr = GetLastError() == ERROR_ALREADY_EXISTS
             ? HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)
             : S_OK;
  }

  if (SUCCEEDED(hr)) {
    // Defines the callback that will signal the event to unblock and resume.
    hr = async_op->put_Completed(
        Callback<IAsyncOperationCompletedHandler<T>>(
            [event_completed_handle_ptr](IAsyncOperation<T>*,
                                         AsyncStatus async_status) -> HRESULT {
              ::SetEvent(*event_completed_handle_ptr);

              return async_status == Completed ? S_OK : E_ABORT;
            })
            .Get());
  }

  // Block and suspend thread until the async operation finishes or timeout.
  if (SUCCEEDED(hr)) {
    hr = WaitForASyncWithEvent(async_info.Get(), event_completed_handle_ptr,
                               timeout_ms);
  }

  if (SUCCEEDED(hr)) {
    hr = async_info->get_ErrorCode(&hr_async_error);
  }

  if (SUCCEEDED(hr)) {
    hr = hr_async_error;
  }

  return hr;
}

namespace webrtc {

class AudioDeviceWindowsCore : public AudioDeviceGeneric {
 public:
  AudioDeviceWindowsCore();
  ~AudioDeviceWindowsCore();

  static bool CoreAudioIsSupported();

  // Retrieve the currently utilized audio layer
  virtual int32_t ActiveAudioLayer(
      AudioDeviceModule::AudioLayer& audioLayer) const;

  // Attach audio device buffer
  virtual void AttachAudioBuffer(AudioDeviceBuffer* audioBuffer);

  // Main initializaton and termination
  virtual InitStatus Init();
  virtual int32_t Terminate();
  virtual bool Initialized() const;

  // Device enumeration
  virtual int16_t PlayoutDevices();
  virtual int16_t RecordingDevices();
  virtual int32_t PlayoutDeviceName(uint16_t index,
                                    char name[kAdmMaxDeviceNameSize],
                                    char guid[kAdmMaxGuidSize]);
  virtual int32_t RecordingDeviceName(uint16_t index,
                                      char name[kAdmMaxDeviceNameSize],
                                      char guid[kAdmMaxGuidSize]);

  // Device selection
  virtual int32_t SetPlayoutDevice(uint16_t index);
  virtual int32_t SetPlayoutDevice(AudioDeviceModule::WindowsDeviceType device);
  virtual int32_t SetRecordingDevice(uint16_t index);
  virtual int32_t SetRecordingDevice(
      AudioDeviceModule::WindowsDeviceType device);

  // Audio transport initialization
  virtual int32_t PlayoutIsAvailable(bool& available);
  virtual int32_t InitPlayout();
  virtual bool PlayoutIsInitialized() const;
  virtual int32_t RecordingIsAvailable(bool& available);
  virtual int32_t InitRecording();
  virtual bool RecordingIsInitialized() const;

  // Audio transport control
  virtual int32_t StartPlayout();
  virtual int32_t StopPlayout();
  virtual bool Playing() const;
  virtual int32_t StartRecording();
  virtual int32_t StopRecording();
  virtual bool Recording() const;

  // Audio mixer initialization
  virtual int32_t InitSpeaker();
  virtual bool SpeakerIsInitialized() const;
  virtual int32_t InitMicrophone();
  virtual bool MicrophoneIsInitialized() const;

  // Speaker volume controls
  virtual int32_t SpeakerVolumeIsAvailable(bool& available);
  virtual int32_t SetSpeakerVolume(uint32_t volume);
  virtual int32_t SpeakerVolume(uint32_t& volume) const;
  virtual int32_t MaxSpeakerVolume(uint32_t& maxVolume) const;
  virtual int32_t MinSpeakerVolume(uint32_t& minVolume) const;

  // Microphone volume controls
  virtual int32_t MicrophoneVolumeIsAvailable(bool& available);
  virtual int32_t SetMicrophoneVolume(uint32_t volume);
  virtual int32_t MicrophoneVolume(uint32_t& volume) const;
  virtual int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const;
  virtual int32_t MinMicrophoneVolume(uint32_t& minVolume) const;

  // Speaker mute control
  virtual int32_t SpeakerMuteIsAvailable(bool& available);
  virtual int32_t SetSpeakerMute(bool enable);
  virtual int32_t SpeakerMute(bool& enabled) const;

  // Microphone mute control
  virtual int32_t MicrophoneMuteIsAvailable(bool& available);
  virtual int32_t SetMicrophoneMute(bool enable);
  virtual int32_t MicrophoneMute(bool& enabled) const;

  // Stereo support
  virtual int32_t StereoPlayoutIsAvailable(bool& available);
  virtual int32_t SetStereoPlayout(bool enable);
  virtual int32_t StereoPlayout(bool& enabled) const;
  virtual int32_t StereoRecordingIsAvailable(bool& available);
  virtual int32_t SetStereoRecording(bool enable);
  virtual int32_t StereoRecording(bool& enabled) const;

  // Delay information and control
  virtual int32_t PlayoutDelay(uint16_t& delayMS) const;

  // AEC support
  virtual bool BuiltInAECIsAvailable() const;
  virtual int32_t EnableBuiltInAEC(bool enable);

 private:
  struct AudioDeviceWindowsCoreInternal* _internal;
};
}  // namespace webrtc

#endif  // AUDIO_DEVICE_AUDIO_DEVICE_CORE_WIN_H_
