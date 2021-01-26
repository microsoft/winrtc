#include "pch.h"
#include "webrtc.DataChannelObserver.h"
#include "webrtc.DataChannelObserver.g.cpp"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct CreateDataChannelObeserver_Helper : public ::webrtc::DataChannelObserver
{
    CreateDataChannelObeserver_Helper(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataChannelObserver *create_data_channel_observer)
      : create_data_channel_observe_(create_data_channel_observer)
    {
    }

    ~CreateDataChannelObeserver_Helper() override
    {
    }


    void 
    OnStateChange() override
    {
      create_data_channel_observe_->on_state_change_event_();
    }

    void
    OnMessage(::webrtc::DataBuffer *buffer) 
    {
      create_data_channel_observe_->on_message_event_(make<DataBuffer>(buffer));
    }

    void
    OnBufferedAmountChange(int64_t sent_data_size)
    {
      create_data_channel_observe_->on_buffer_amount_change_event_(make<int64_t>(sent_data_size));
    }


 private:
  Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataChannelObserver *create_data_channel_observe_;
};

   /* DataChannelObserver::DataChannelObserver()
    : webrtc_data_channel_observer_helper_(new CreateDataChannelObeserver_Helper(this))
    {
    }*/

    DataChannelObserver::~DataChannelObserver()
    {
      delete static_cast<CreateDataChannelObeserver_Helper *>(webrtc_data_channel_observer_helper_);
    }
    
    winrt::event_token DataChannelObserver::OnStateChange(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnStateChangeDelegate const& handler)
    {
        return on_state_change_event_.add(handler);
    }
    void DataChannelObserver::OnStateChange(winrt::event_token const& token) noexcept
    {
      on_state_change_event_.remove(token);
    }
    winrt::event_token DataChannelObserver::OnMessage(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnMessageDelegate const& handler)
    {
        return on_message_event_.add(handler);
    }
    void DataChannelObserver::OnMessage(winrt::event_token const& token) noexcept
    {
        on_message_event_.remove(token);
    }
    winrt::event_token DataChannelObserver::OnBufferedAmountChange(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnBufferedAmountChangeDelegate const& handler)
    {
        return on_buffer_amount_change_event_.add(handler);
    }
    void DataChannelObserver::OnBufferedAmountChange(winrt::event_token const& token) noexcept
    {
        on_buffer_amount_change_event_.remove(token);
    }
    ::rtc::scoped_refptr<::webrtc::DataChannelObserver>
    DataChannelObserver::get_webrtc_data_channel_observer()
    {
      return webrtc_data_channel_observer_helper_;
    }
   /* void
    DataChannelObserver::OnStateChange()
    {
      on_state_change_event_(); 
    }
    void
    DataChannelObserver::OnMessage(::webrtc::DataChannelObserver::OnMessage message)
    {
      on_message_event_(message);
    }
    void
    DataChannelObserver::OnBufferedAmountChange(
        ::webrtc::DataChannelObserver::OnBufferedAmountChange webrtc_buffer_change)
    {
      on_buffer_amount_change_event_(webrtc_buffer_change);
    }*/

    


}
