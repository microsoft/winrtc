// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.AudioSource.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct AudioSource : AudioSourceT<AudioSource>, ::webrtc::AudioSourceInterface::AudioObserver
{
  explicit AudioSource(::rtc::scoped_refptr<::webrtc::AudioSourceInterface> webrtc_audio_source);
  ~AudioSource();

  float Volume();
  void Volume(float value);
  Microsoft::WinRTC::WebRtcWrapper::cricket::AudioOptions Options();
  void AddSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSink const &sink);
  void RemoveSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSink const &sink);
  winrt::event_token OnSetVolume(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSourceOnSetVolumeDelegate const &handler);
  void OnSetVolume(winrt::event_token const &token) noexcept;

  void OnSetVolume(double volume) override;

  ::rtc::scoped_refptr<::webrtc::AudioSourceInterface> get_webrtc_audio_source();

private:
  ::rtc::scoped_refptr<::webrtc::AudioSourceInterface> webrtc_audio_source_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSourceOnSetVolumeDelegate> on_set_volume_event_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
