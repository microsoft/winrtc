// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoCaptureFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoCaptureFactory
{
  VideoCaptureFactory() = default;

  static Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureModule Create(hstring const &device_unique_id);
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureModule_DeviceInfo CreateDeviceInfo();
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct VideoCaptureFactory : VideoCaptureFactoryT<VideoCaptureFactory, implementation::VideoCaptureFactory>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
