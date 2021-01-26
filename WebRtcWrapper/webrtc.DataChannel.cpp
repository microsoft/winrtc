#include "pch.h"
#include "webrtc.DataChannel.h"
#include "webrtc.DataChannel.g.cpp"
#include "webrtc.DataChannelObserver.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
DataChannel::DataChannel(::rtc::scoped_refptr<::webrtc::DataChannelInterface> webrtc_data_channel)
    : webrtc_data_channel_(webrtc_data_channel)
{

}

    void
    DataChannel::RegisterObserver(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserver const &observer)
    {
      webrtc_data_channel_->RegisterObserver(
          get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataChannelObserver>(observer));
        /*-> get_webrtc_data_channel_observer().get());*/
    }

    hstring DataChannel::Label()
    {
      return  to_hstring(webrtc_data_channel_->label());
    }
    
    bool DataChannel::Reliable()
    {
        return webrtc_data_channel_->reliable();
    }
   
    hstring DataChannel::Ordered()
    {
      return to_hstring(webrtc_data_channel_->ordered());
    }
    void DataChannel::Ordered(hstring const& value)
    {
        webrtc_data_channel_->ordered();
    }
    hstring DataChannel::Negotiated()
    {
        return to_hstring(webrtc_data_channel_->negotiated());
    }
    void DataChannel::Negotiated(hstring const& value)
    {
        webrtc_data_channel_->negotiated();
    }
    int16_t DataChannel::MaxRetransmitTime()
    {
      return webrtc_data_channel_->maxRetransmitTime();
    }
    void DataChannel::MaxRetransmitTime(int16_t value)
    {
        webrtc_data_channel_->maxRetransmitTime();
    }
    int16_t DataChannel::MaxRetransmits()
    {
      return webrtc_data_channel_-> maxRetransmits();
    }
    void DataChannel::MaxRetransmits(int16_t value)
    {
        webrtc_data_channel_->maxRetransmits();
    }
    char
    DataChannel::DataString(DataState state)
    {
      switch (state)
      {
     /* case winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kConnecting:
        on_state_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kConnecting);
        break;
      case winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kOpen:
        webrtc_data_channel_(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kOpen);
        break;
      case winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kClosing:
        webrtc_data_channel_(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kClosing);
        break;
      case winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kClosed:
        webrtc_data_channel_(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataState::kClosed);
        break;*/
      default: {
        throw hresult_illegal_state_change();
      }
        ;
      }
      return 0;
    }
    int32_t DataChannel::Id()
    {
      return webrtc_data_channel_->id();
    }
    void DataChannel::Close()
    {
      return webrtc_data_channel_->Close();
    }
    hstring DataChannel::Send(Microsoft::WinRTC::WebRtcWrapper::webrtc::DataBuffer const& buffer)
    {
      /*webrtc_data_channel_->Send(buffer);
      ::webrtc::DataBuffer data = buffer;*/
      /* return to_hstring(Send_t(buffer));*/
    }
    hstring
    DataChannel::Send_t(::webrtc::DataBuffer const &buffer)
    {
      return to_hstring(webrtc_data_channel_->Send(buffer));
    }
    const ::webrtc::DataChannelInterface *
    DataChannel::get_webrtc_data_channel() const
    {
      return webrtc_data_channel_;
    }
    }
