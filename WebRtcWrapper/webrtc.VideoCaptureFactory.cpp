// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoCaptureFactory.h"
#include "webrtc.VideoCaptureFactory.g.cpp"
// clang-format on

#include "webrtc.VideoCaptureModule_DeviceInfo.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureModule
VideoCaptureFactory::Create(hstring const &device_unique_id)
{
  throw hresult_not_implemented();
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureModule_DeviceInfo
VideoCaptureFactory::CreateDeviceInfo()
{
  return make<VideoCaptureModule_DeviceInfo>(::webrtc::VideoCaptureFactory::CreateDeviceInfo());
}
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
