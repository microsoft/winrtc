// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include "pch.h"
#include "webrtc.DataBuffer_Helper.h"
#include "webrtc.DataBuffer_Helper.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
DataBuffer_Helper::DataBuffer_Helper(::webrtc::DataBuffer data_buffer_helper) : data_buffer_helper_(data_buffer_helper)
{
}
::webrtc::DataBuffer
DataBuffer_Helper::get_webrtc_data_buffer_helper() const
{
  return data_buffer_helper_;
}
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
