#include "pch.h"
#include "webrtc.DataBuffer.h"
#include "webrtc.DataBuffer.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
DataBuffer::DataBuffer(const ::rtc::CopyOnWriteBuffer &data, bool binary)
    : data(data), binary_(binary)
    {
        ::webrtc::DataBuffer::DataBuffer(data, binary);
      
    }

 /*DataBuffer::DataBuffer(const string text) : data(text.data(), text.length()), binary(false)
    {
   ::webrtc::DataBuffer::DataBuffer(text);
    }*/
   
    bool DataBuffer::binary()
    {
        return binary_;
    }
    void DataBuffer::binary(bool value)
    {
      binary_ = value;
    }
}
