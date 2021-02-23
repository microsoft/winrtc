#pragma once
#include "webrtc.DataChannelInit.g.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    struct DataChannelInit : DataChannelInitT<DataChannelInit>
    {
      DataChannelInit() = default;
      //explicit DataChannelInit(::rtc::scoped_refptr<::webrtc::DataChannelInit> &webrtc_data_channel_init);

        bool Reliable();
        void Reliable(bool value);
        bool Ordered();
        void Ordered(bool value);
        int32_t MaxRetransmitTime();
        void MaxRetransmitTime(int32_t value);
        int32_t MaxRetransmits();
        void MaxRetransmits(int32_t value);
        hstring Protocol();
        void Protocol(hstring const& value);
        bool Negotiated();
        void Negotiated(bool value);
        int32_t Id();
        void Id(int32_t value);

       const ::webrtc::DataChannelInit*  get_webrtc_data_channel_init();

      private:
        ::webrtc::DataChannelInit webrtc_data_channel_init_;
    };
}


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
    struct DataChannelInit : DataChannelInitT<DataChannelInit, implementation::DataChannelInit>
    {
    };
}