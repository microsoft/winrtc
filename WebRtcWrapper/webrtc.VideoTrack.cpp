// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoTrack.h"
#include "webrtc.VideoTrack.g.cpp"
// clang-format on

#include "webrtc.VideoSink_VideoFrame.h"
#include "rtc.VideoSinkWants.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
VideoTrack::VideoTrack(::rtc::scoped_refptr<::webrtc::VideoTrackInterface> video_track)
    : VideoTrackT<VideoTrack, Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::MediaStreamTrack>(video_track),
      webrtc_video_track_(video_track)
{
}

::rtc::scoped_refptr<::webrtc::VideoTrackInterface>
VideoTrack::get_webrtc_video_track()
{
  return webrtc_video_track_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
