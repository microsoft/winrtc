// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoCaptureModule_DeviceInfo.h"
#include "webrtc.VideoCaptureModule_DeviceInfo.g.cpp"
// clang-format on

#include "webrtc.VideoCaptureCapability.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
VideoCaptureModule_DeviceInfo::VideoCaptureModule_DeviceInfo(
    ::webrtc::VideoCaptureModule::DeviceInfo *webrtc_device_info)
    : webrtc_device_info_(webrtc_device_info)
{
}

Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::DeviceIdentification>
VideoCaptureModule_DeviceInfo::GetDeviceName()
{
  const uint32_t BUFFER_SIZE = 512;
  char device_name[BUFFER_SIZE];
  char device_unique_id[BUFFER_SIZE];
  char product_unique_id[BUFFER_SIZE];
  Microsoft::WinRTC::WebRtcWrapper::webrtc::DeviceIdentification device_identification;
  std::vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::DeviceIdentification> devices;

  const uint32_t number_of_devices = webrtc_device_info_->NumberOfDevices();
  for (uint32_t i = 0; i < number_of_devices; ++i)
  {

    if (webrtc_device_info_->GetDeviceName(i, device_name, BUFFER_SIZE, device_unique_id, BUFFER_SIZE,
                                           product_unique_id, BUFFER_SIZE) != 0)
    {
      throw hresult_illegal_state_change();
    }

    device_identification.DeviceName = to_hstring(device_name);
    device_identification.DeviceUniqueId = to_hstring(device_unique_id);
    device_identification.ProductUniqueId = to_hstring(product_unique_id);

    devices.push_back(device_identification);
  }

  return single_threaded_vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::DeviceIdentification>(move(devices));
}

Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability>
VideoCaptureModule_DeviceInfo::GetCapability(hstring const &device_unique_id)
{
  std::vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability> capabilities;
  std::string device_id = to_string(device_unique_id);
  const uint32_t number_of_capabilities = webrtc_device_info_->NumberOfCapabilities(device_id.c_str());

  for (uint32_t i = 0; i < number_of_capabilities; ++i)
  {
    ::webrtc::VideoCaptureCapability video_capability;
    if (webrtc_device_info_->GetCapability(device_id.c_str(), i, video_capability) != 0)
    {
      throw hresult_illegal_state_change();
    }
    capabilities.emplace_back(make<VideoCaptureCapability>(video_capability));
  }

  return single_threaded_vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability>(move(capabilities));
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability
VideoCaptureModule_DeviceInfo::GetBestMatchedCapability(
    hstring const &device_unique_id, Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoCaptureCapability const &requested)
{
  std::string device_id = to_string(device_unique_id);
  ::webrtc::VideoCaptureCapability best_video_capability;

  if (webrtc_device_info_->GetBestMatchedCapability(device_id.c_str(), *get_self<VideoCaptureCapability>(requested),
                                                    best_video_capability) != 0)
  {
    throw hresult_illegal_state_change();
  }

  return make<VideoCaptureCapability>(best_video_capability);
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation
VideoCaptureModule_DeviceInfo::GetOrientation(hstring const &device_unique_id)
{
  std::string device_id = to_string(device_unique_id);
  ::webrtc::VideoRotation orientation;

  if (webrtc_device_info_->GetOrientation(device_id.c_str(), orientation) != 0)
  {
    throw hresult_illegal_state_change();
  }

  switch (orientation)
  {
  case ::webrtc::VideoRotation::kVideoRotation_0: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation::VideoRotation_0;
  }
  case ::webrtc::VideoRotation::kVideoRotation_90: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation::VideoRotation_90;
  }
  case ::webrtc::VideoRotation::kVideoRotation_180: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation::VideoRotation_180;
  }
  case ::webrtc::VideoRotation::kVideoRotation_270: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoRotation::VideoRotation_270;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
