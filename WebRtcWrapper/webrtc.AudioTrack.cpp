// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.AudioTrack.h"
#include "webrtc.AudioTrack.g.cpp"
// clang-format on

#include "webrtc.MediaStreamTrack.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
AudioTrack::AudioTrack(::rtc::scoped_refptr<::webrtc::AudioTrackInterface> webrtc_audio_track)
    : AudioTrackT<AudioTrack, Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::MediaStreamTrack>(
          webrtc_audio_track),
      webrtc_audio_track_(webrtc_audio_track)
{
}

::rtc::scoped_refptr<::webrtc::AudioTrackInterface>
AudioTrack::get_webrtc_audio_track()
{
  return webrtc_audio_track_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
