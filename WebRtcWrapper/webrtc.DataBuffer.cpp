#include "pch.h"
#include "webrtc.DataBuffer.h"
#include "webrtc.DataBuffer.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
//DataBuffer::DataBuffer(const ::webrtc:: &data, bool binary)
//    : data(data), binary_(binary)
//    {
//        ::webrtc::DataBuffer::DataBuffer(data, binary);
//      
//    }

 /*DataBuffer::DataBuffer(const string text) : data(text.data(), text.length()), binary(false)
    {
   ::webrtc::DataBuffer::DataBuffer(text);
    }*/
   
    bool DataBuffer::binary()
    {
        return pData_->binary;
    }
    void DataBuffer::binary(bool value)
    {
      pData_->binary = value;
    }

    ::webrtc::DataBuffer &
    DataBuffer::get_webrtc_data_buffer()
    {
      return *pData_;
    }
    }
