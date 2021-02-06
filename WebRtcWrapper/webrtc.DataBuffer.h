// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.DataBuffer.g.h"
#include "webrtc.DataBuffer_Helper.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct DataBuffer : DataBufferT<DataBuffer>
{
  //DataBuffer() = default;



  DataBuffer(hstring const &hstring_data) : pData_(new ::webrtc::DataBuffer(winrt::to_string(hstring_data)))
  {
  }

  DataBuffer(::webrtc::DataBuffer const &hstring_data) : pData_(const_cast<::webrtc::DataBuffer *>(&hstring_data))
  {
  }

  ~DataBuffer() override
  {
    delete pData_;
  }
 
  /*bool binary_;*/
  bool binary();
 /* {
    return pData_->binary;
  }*/
  void binary(bool value);

  ::webrtc::DataBuffer &get_webrtc_data_buffer();
  /*{
    return *pData_;
  }*/

private:
  ::webrtc::DataBuffer *pData_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
    struct DataBuffer : DataBufferT<DataBuffer, implementation::DataBuffer>
    {
    };
}
