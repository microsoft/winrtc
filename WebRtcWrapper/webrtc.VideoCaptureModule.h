// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoCaptureModule.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoCaptureModule : VideoCaptureModuleT<VideoCaptureModule>
{
  VideoCaptureModule() = default;

  int32_t StartCapture(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability const &capability);
  int32_t StopCapture();
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
