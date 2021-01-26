#include "pch.h"
#include "webrtc.DataChannelInit.h"
#include "webrtc.DataChannelInit.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    DataChannelInit::DataChannelInit(const ::webrtc::DataChannelInit& webrtc_data_channel_init)
        : webrtc_data_channel_init_(webrtc_data_channel_init)
    {

    }

    bool
    DataChannelInit::Reliable()
    {
        return reliable;
    }

    void DataChannelInit::Reliable(bool value)
    {
      reliable = value;
    }

    bool DataChannelInit::Ordered()
    {
      return ordered;
    }

    int32_t DataChannelInit::MaxRetransmitTime()
    {
      return maxRetransmitTime.value();
    }

    void DataChannelInit::MaxRetransmitTime(int32_t value)
    {
      maxRetransmitTime = value;
    }

    int32_t DataChannelInit::MaxRetransmits()
    {
      return maxRetransmits.value();
    }

    void DataChannelInit::MaxRetransmits(int32_t value)
    {
      maxRetransmits = value;
    }

    hstring DataChannelInit::Protocol()
    {
      return to_hstring(protocol);
    }

    void DataChannelInit::Protocol(hstring const& value)
    {
       protocol = to_string(value);
    }

    bool DataChannelInit::Negotiated()
    {
      return negotiated;
    }

    void DataChannelInit::Negotiated(bool value)
    {
      negotiated = value;
    }

    int32_t DataChannelInit::Id()
    {
      return id;
    }
    
    const ::webrtc::DataChannelInit *
    DataChannelInit::get_webrtc_data_channel_init() const
    {
      /*return &webrtc_data_channel_init_;*/
    }
}
