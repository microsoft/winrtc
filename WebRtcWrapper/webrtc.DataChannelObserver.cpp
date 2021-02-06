#include "pch.h"
#include "webrtc.DataChannelObserver.h"
#include "webrtc.DataChannelObserver.g.cpp"
#include "webrtc.DataBuffer.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    DataChannelObserver::DataChannelObserver()
    : webrtc_data_channel_observer_helper_(new DataChannelObeserver_Helper(this))
    { 
    }

    DataChannelObserver::~DataChannelObserver()
    {
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
    /*::rtc::scoped_refptr<::webrtc::DataChannelObserver>
    DataChannelObserver::get_webrtc_data_channel_observer()
    {
      return ::rtc::scoped_refptr<::webrtc::DataChannelObserver>(webrtc_data_channel_observer_helper_.get());
    }
   */
    
    
    /*void
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
