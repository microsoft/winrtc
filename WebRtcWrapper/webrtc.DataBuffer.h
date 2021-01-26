#pragma once
#include "webrtc.DataBuffer.g.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    struct DataBuffer : DataBufferT<DataBuffer>
    {
        DataBuffer() = default;

        DataBuffer(const ::rtc::CopyOnWriteBuffer& data, bool binary) : data(data), binary_(binary)
        {
        }

        explicit DataBuffer(const std::string &text) : data(text.data(), text.length()), binary_(false)
        {
        }

        bool binary_;
        bool binary();
        void binary(bool value);

        ::rtc::CopyOnWriteBuffer data;
    };
}
namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
    struct DataBuffer : DataBufferT<DataBuffer, implementation::DataBuffer>
    {
    };
}
