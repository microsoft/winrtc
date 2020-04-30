#include "pch.h"
#include "webrtc.VideoFrame.VideoFrame.h"
#include "webrtc.VideoFrame.VideoFrame.g.cpp"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::implementation
{
  //VideoFrame::VideoFrame(::webrtc::VideoFrame webrtc_videoframe) :
  //  webrtc_videoframe_(webrtc_videoframe)
  //{
  //}

  int32_t VideoFrame::Width()
  {
    throw 10;
    //return webrtc_videoframe_.width();
  }

  int32_t VideoFrame::Height()
  {
    throw 10;
    //return webrtc_videoframe_.height();
  }

  uint32_t VideoFrame::Size()
  {
    throw 10;
    //return webrtc_videoframe_.size();
  }
}
