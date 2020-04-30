// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoCaptureCapability.h"
#include "webrtc.VideoCaptureCapability.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

VideoCaptureCapability::VideoCaptureCapability(const ::webrtc::VideoCaptureCapability &video_capability)
{
  width = video_capability.width;
  height = video_capability.height;
  maxFPS = video_capability.maxFPS;
  videoType = video_capability.videoType;
  interlaced = video_capability.interlaced;
}

int32_t
VideoCaptureCapability::Width()
{
  return width;
}

void
VideoCaptureCapability::Width(int32_t value)
{
  width = value;
}

int32_t
VideoCaptureCapability::Height()
{
  return height;
}

void
VideoCaptureCapability::Height(int32_t value)
{
  height = value;
}

int32_t
VideoCaptureCapability::MaxFPS()
{
  return maxFPS;
}

void
VideoCaptureCapability::MaxFPS(int32_t value)
{
  maxFPS = value;
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType
VideoCaptureCapability::VideoType()
{
  switch (videoType)
  {
  case ::webrtc::VideoType::kUnknown:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::Unknown;
  case ::webrtc::VideoType::kI420:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::I420;
  case ::webrtc::VideoType::kIYUV:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::IYUV;
  case ::webrtc::VideoType::kRGB24:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::RGB24;
  case ::webrtc::VideoType::kABGR:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ABGR;
  case ::webrtc::VideoType::kARGB:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB;
  case ::webrtc::VideoType::kARGB4444:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB4444;
  case ::webrtc::VideoType::kRGB565:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::RGB565;
  case ::webrtc::VideoType::kARGB1555:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB1555;
  case ::webrtc::VideoType::kYUY2:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::YUY2;
  case ::webrtc::VideoType::kYV12:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::YV12;
  case ::webrtc::VideoType::kUYVY:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::UYVY;
  case ::webrtc::VideoType::kMJPEG:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::MJPEG;
  case ::webrtc::VideoType::kNV21:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::NV21;
  case ::webrtc::VideoType::kNV12:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::NV12;
  case ::webrtc::VideoType::kBGRA:
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::BGRA;
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
VideoCaptureCapability::VideoType(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType const &value)
{
  switch (value)
  {
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::Unknown: {
    videoType = ::webrtc::VideoType::kUnknown;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::I420: {
    videoType = ::webrtc::VideoType::kI420;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::IYUV: {
    videoType = ::webrtc::VideoType::kIYUV;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::RGB24: {
    videoType = ::webrtc::VideoType::kRGB24;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ABGR: {
    videoType = ::webrtc::VideoType::kABGR;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB: {
    videoType = ::webrtc::VideoType::kARGB;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB4444: {
    videoType = ::webrtc::VideoType::kARGB4444;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::RGB565: {
    videoType = ::webrtc::VideoType::kRGB565;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::ARGB1555: {
    videoType = ::webrtc::VideoType::kARGB1555;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::YUY2: {
    videoType = ::webrtc::VideoType::kYUY2;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::YV12: {
    videoType = ::webrtc::VideoType::kYV12;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::UYVY: {
    videoType = ::webrtc::VideoType::kUYVY;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::MJPEG: {
    videoType = ::webrtc::VideoType::kMJPEG;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::NV21: {
    videoType = ::webrtc::VideoType::kNV21;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::NV12: {
    videoType = ::webrtc::VideoType::kNV12;
    break;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType::BGRA: {
    videoType = ::webrtc::VideoType::kBGRA;
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

bool
VideoCaptureCapability::Interlaced()
{
  return interlaced;
}

void
VideoCaptureCapability::Interlaced(bool value)
{
  interlaced = value;
}
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
