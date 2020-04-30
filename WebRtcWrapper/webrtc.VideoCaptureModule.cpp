// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoCaptureModule.h"
#include "webrtc.VideoCaptureModule.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
int32_t
VideoCaptureModule::StartCapture(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability const &capability)
{
  throw hresult_not_implemented();
}

int32_t
VideoCaptureModule::StopCapture()
{
  throw hresult_not_implemented();
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
