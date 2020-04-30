#pragma once
#include "webrtc.VideoFrame.VideoFrame.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::implementation
{
    struct VideoFrame : VideoFrameT<VideoFrame>
    {
      VideoFrame() = default;

      // VideoFrame(::webrtc::VideoFrame webrtc_videoframe);

        int32_t Width();
        int32_t Height();
        uint32_t Size();

        //::webrtc::VideoFrame webrtc_videoframe_;
    };
}
