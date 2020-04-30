// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoCaptureCapability.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

// The following APIs are in the works.
struct VideoCaptureCapability : VideoCaptureCapabilityT<VideoCaptureCapability>, ::webrtc::VideoCaptureCapability
{
  VideoCaptureCapability() = default;
  explicit VideoCaptureCapability(const ::webrtc::VideoCaptureCapability& video_capability);

  int32_t Width();
  void Width(int32_t value);
  int32_t Height();
  void Height(int32_t value);
  int32_t MaxFPS();
  void MaxFPS(int32_t value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType VideoType();
  void VideoType(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoType const &value);
  bool Interlaced();
  void Interlaced(bool value);
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct VideoCaptureCapability : VideoCaptureCapabilityT<VideoCaptureCapability, implementation::VideoCaptureCapability>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
