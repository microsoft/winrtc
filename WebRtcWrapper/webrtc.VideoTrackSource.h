// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoTrackSource.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct VideoTrackSource : VideoTrackSourceT<VideoTrackSource>
{
  VideoTrackSource();
  ~VideoTrackSource();

  ::rtc::scoped_refptr<::webrtc::VideoTrackSourceInterface> get_webrtc_video_track_source();

private:
  ::rtc::scoped_refptr<::webrtc::VideoTrackSourceInterface> webrtc_video_track_source_;
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{

struct VideoTrackSource : VideoTrackSourceT<VideoTrackSource, implementation::VideoTrackSource>
{
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
