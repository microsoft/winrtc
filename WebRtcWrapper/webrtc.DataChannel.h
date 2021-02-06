#pragma once
#include "webrtc.DataChannel.g.h"
#include "webrtc.DataBuffer.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct DataChannel : DataChannelT<DataChannel>
    {
      DataChannel(::rtc::scoped_refptr < ::webrtc::DataChannelInterface> webrtc_data_channel);

        void RegisterObserver(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserver const& observer);
        hstring Label();
        
        bool Reliable();
        
        hstring Ordered();
        void Ordered(hstring const& value);
        hstring Negotiated();
        void Negotiated(hstring const& value);
        int16_t MaxRetransmitTime();
        void MaxRetransmitTime(int16_t value);
        int16_t MaxRetransmits();
        void MaxRetransmits(int16_t value);
        char DataString(DataState state);
        int32_t Id();
        void Close();
        hstring Send(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataBuffer const& buffer);
        hstring Send_t(::webrtc::DataBuffer const &buffer);

        const ::webrtc::DataChannelInterface *get_webrtc_data_channel() const;

        private:
        ::rtc::scoped_refptr<::webrtc::DataChannelInterface> webrtc_data_channel_;
    };
}
