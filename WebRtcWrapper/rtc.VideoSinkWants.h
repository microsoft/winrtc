// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "rtc.VideoSinkWants.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{
struct VideoSinkWants : VideoSinkWantsT<VideoSinkWants>, ::rtc::VideoSinkWants
{
  VideoSinkWants() = default;

  bool RotationApplied();
  void RotationApplied(bool value);
  bool BlackFrames();
  void BlackFrames(bool value);
  int32_t MaxPixelCount();
  void MaxPixelCount(int32_t value);
  int32_t TargetPixelCount();
  void TargetPixelCount(int32_t value);
  int32_t MaxFramerateFps();
  void MaxFramerateFps(int32_t value);
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
{
struct VideoSinkWants : VideoSinkWantsT<VideoSinkWants, implementation::VideoSinkWants>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
