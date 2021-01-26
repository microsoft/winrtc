#pragma once
#include "webrtc.DataChannelInit.g.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    struct DataChannelInit : DataChannelInitT<DataChannelInit>, ::webrtc::DataChannelInit
    {
      DataChannelInit() = default;
      explicit DataChannelInit(const ::webrtc::DataChannelInit &webrtc_data_channel_init);

        bool Reliable();
        void Reliable(bool value);
        bool Ordered();
        int32_t MaxRetransmitTime();
        void MaxRetransmitTime(int32_t value);
        int32_t MaxRetransmits();
        void MaxRetransmits(int32_t value);
        hstring Protocol();
        void Protocol(hstring const& value);
        bool Negotiated();
        void Negotiated(bool value);
        int32_t Id();

        const ::webrtc::DataChannelInit *get_webrtc_data_channel_init() const;

      private:
        ::webrtc::DataChannelInit webrtc_data_channel_init_;
    };
}
