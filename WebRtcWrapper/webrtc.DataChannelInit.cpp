#include "pch.h"
#include "webrtc.DataChannelInit.h"
#include "webrtc.DataChannelInit.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
//DataChannelInit::DataChannelInit(::rtc::scoped_refptr<::webrtc::DataChannelInit> &webrtc_data_channel_init)
//        : webrtc_data_channel_init_(webrtc_data_channel_init)
//    {
//
//    }

    bool
    DataChannelInit::Reliable()
    {
      return webrtc_data_channel_init_.reliable;
    }

    void DataChannelInit::Reliable(bool value)
    {
      webrtc_data_channel_init_.reliable = value;
    }

    bool DataChannelInit::Ordered()
    {
      return webrtc_data_channel_init_.ordered;
    }

    void
    DataChannelInit::Ordered(bool value)
    {
      webrtc_data_channel_init_.ordered = value;
    }

    int32_t DataChannelInit::MaxRetransmitTime()
    {
      return webrtc_data_channel_init_.maxRetransmitTime.value();
    }

    void DataChannelInit::MaxRetransmitTime(int32_t value)
    {
      webrtc_data_channel_init_.maxRetransmitTime = value;
    }

    int32_t DataChannelInit::MaxRetransmits()
    {
      return webrtc_data_channel_init_.maxRetransmits.value();
    }

    void DataChannelInit::MaxRetransmits(int32_t value)
    {
      webrtc_data_channel_init_.maxRetransmits = value;
    }

    hstring DataChannelInit::Protocol()
    {
      return to_hstring(webrtc_data_channel_init_.protocol);
    }

    void DataChannelInit::Protocol(hstring const& value)
    {
      webrtc_data_channel_init_.protocol = to_string(value);
    }

    bool DataChannelInit::Negotiated()
    {
      return webrtc_data_channel_init_.negotiated;
    }

    void DataChannelInit::Negotiated(bool value)
    {
      webrtc_data_channel_init_.negotiated = value;
    }

    int32_t DataChannelInit::Id()
    {
      return webrtc_data_channel_init_.id;
    }

    void
    DataChannelInit::Id(int32_t value)
    {
      webrtc_data_channel_init_.id = value;
    }
    
    const ::webrtc::DataChannelInit*
    DataChannelInit::get_webrtc_data_channel_init() 
    {
      return &webrtc_data_channel_init_;
    }
}
