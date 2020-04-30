// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.AudioTrackSink.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct AudioTrackSink : AudioTrackSinkT<AudioTrackSink>, ::webrtc::AudioTrackSinkInterface
{
  AudioTrackSink() = default;

  winrt::event_token OnData(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSinkOnData const &handler);
  void OnData(winrt::event_token const &token) noexcept;

  void OnData(const void *audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
              size_t number_of_frames) override;

private:
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSinkOnData> on_audio_track_sink_on_data_event_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
