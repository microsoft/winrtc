// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoCaptureModule_DeviceInfo.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoCaptureModule_DeviceInfo : VideoCaptureModule_DeviceInfoT<VideoCaptureModule_DeviceInfo>
{
  VideoCaptureModule_DeviceInfo(::webrtc::VideoCaptureModule::DeviceInfo *webrtc_device_info);

  Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::DeviceIdentification>
  GetDeviceName();
  Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability>
  GetCapability(hstring const &device_unique_id);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability GetBestMatchedCapability(
      hstring const &device_unique_id,
      Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability const &requested);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation GetOrientation(hstring const &device_unique_id);

private:
  std::unique_ptr<::webrtc::VideoCaptureModule::DeviceInfo> webrtc_device_info_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
