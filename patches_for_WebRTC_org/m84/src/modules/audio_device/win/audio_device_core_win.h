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

#include "api/scoped_refptr.h"
#include "modules/audio_device/audio_device_generic.h"
#include "rtc_base/critical_section.h"
#include "rtc_base/win32.h"

#define THR(expr)   \
  do {              \
    hr = (expr);    \
    if (FAILED(hr)) \
      goto Cleanup; \
  } while (false)

template <typename T>
HRESULT WaitForAsyncOperation(
    Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncOperation<T>>&
        async_op) {
  HRESULT hr, hr_async_error;
  const DWORD timeout_ms = 2000;
  Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncInfo> async_info;

  // Creates the Event to be used to block and suspend until the async
  // operation finishes.
  Microsoft::WRL::Wrappers::Event event_completed =
      Microsoft::WRL::Wrappers::Event(
          ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET,
                          WRITE_OWNER | EVENT_ALL_ACCESS));
  THR(event_completed.IsValid() ? S_OK : E_HANDLE);

  // Defines the callback that will signal the event to unblock and resume.
  THR(async_op->put_Completed(
      Microsoft::WRL::Callback<
          ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>>(
          [&event_completed](
              ABI::Windows::Foundation::IAsyncOperation<T>*,
              ABI::Windows::Foundation::AsyncStatus async_status) -> HRESULT {
            HRESULT hr;

            THR(async_status == ABI::Windows::Foundation::AsyncStatus::Completed
                    ? S_OK
                    : E_ABORT);

          Cleanup:
            ::SetEvent(event_completed.Get());

            return hr;
          })
          .Get()));

  // Block and suspend thread until the async operation finishes or timeout.
  THR(::WaitForSingleObjectEx(event_completed.Get(), timeout_ms, FALSE) ==
              WAIT_OBJECT_0
          ? S_OK
          : E_FAIL);

  // Checks if async operation completed successfully.
  THR(async_op.template As<ABI::Windows::Foundation::IAsyncInfo>(&async_info));
  THR(async_info->get_ErrorCode(&hr_async_error));
  THR(hr_async_error);

Cleanup:
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
