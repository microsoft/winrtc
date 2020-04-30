// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.MediaStreamTrack.h"
#include "webrtc.VideoTrack.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoTrack : VideoTrackT<VideoTrack, Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::MediaStreamTrack>
{
  VideoTrack(::rtc::scoped_refptr<::webrtc::VideoTrackInterface> video_track);

  ::rtc::scoped_refptr<::webrtc::VideoTrackInterface> get_webrtc_video_track();

private:
  ::rtc::scoped_refptr<::webrtc::VideoTrackInterface> webrtc_video_track_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
