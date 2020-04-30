// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "rtc.VideoSinkWants.h"
#include "rtc.VideoSinkWants.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{

bool
VideoSinkWants::RotationApplied()
{
  return rotation_applied;
}

void
VideoSinkWants::RotationApplied(bool value)
{
  rotation_applied = value;
}

bool
VideoSinkWants::BlackFrames()
{
  return black_frames;
}

void
VideoSinkWants::BlackFrames(bool value)
{
  black_frames = value;
}

int32_t
VideoSinkWants::MaxPixelCount()
{
  return max_pixel_count;
}

void
VideoSinkWants::MaxPixelCount(int32_t value)
{
  max_pixel_count = value;
}

int32_t
VideoSinkWants::TargetPixelCount()
{
  return target_pixel_count.value_or(max_pixel_count);
}

void
VideoSinkWants::TargetPixelCount(int32_t value)
{
  target_pixel_count = value;
}
int32_t
VideoSinkWants::MaxFramerateFps()
{
  return max_framerate_fps;
}

void
VideoSinkWants::MaxFramerateFps(int32_t value)
{
  max_framerate_fps = value;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
