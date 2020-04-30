// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoSink_VideoFrame.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoSink_VideoFrame : VideoSink_VideoFrameT<VideoSink_VideoFrame>
{
  VideoSink_VideoFrame(const winrt::Windows::UI::Composition::VisualCollection &visual_collection);
  ~VideoSink_VideoFrame();

  ::rtc::VideoSinkInterface<::webrtc::VideoFrame> *get_webrtc_video_sink_video_frame() const;

private:
  void *webrtc_video_sink_video_frame_;
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct VideoSink_VideoFrame : VideoSink_VideoFrameT<VideoSink_VideoFrame, implementation::VideoSink_VideoFrame>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
