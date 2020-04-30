// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.AudioTrack.g.h"
#include "webrtc.MediaStreamTrack.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct AudioTrack : AudioTrackT<AudioTrack, Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::MediaStreamTrack>
{
  AudioTrack(::rtc::scoped_refptr<::webrtc::AudioTrackInterface> webrtc_audio_track);

  ::rtc::scoped_refptr<::webrtc::AudioTrackInterface> get_webrtc_audio_track();

private:
  ::rtc::scoped_refptr<::webrtc::AudioTrackInterface> webrtc_audio_track_;
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
