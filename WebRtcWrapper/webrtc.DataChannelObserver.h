#pragma once
#include "webrtc.DataChannelObserver.g.h"
#include "webrtc.DataBuffer.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

    struct DataChannelObeserver_Helper;
    struct DataChannelObserver : DataChannelObserverT<DataChannelObserver>, public ::webrtc::DataChannelObserver
    {
        DataChannelObserver();
        ~DataChannelObserver() override;

        winrt::event_token OnStateChange(
            Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnStateChangeDelegate const& handler);
        void OnStateChange(winrt::event_token const& token) noexcept;
        winrt::event_token OnMessage(
            Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnMessageDelegate const& handler);
        void OnMessage(winrt::event_token const& token) noexcept;
        winrt::event_token OnBufferedAmountChange(
            Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnBufferedAmountChangeDelegate const& handler);
        void OnBufferedAmountChange(winrt::event_token const& token) noexcept;

        //::rtc::scoped_refptr<::webrtc::DataChannelObserver> get_webrtc_data_channel_observer();

        

       /* void OnStateChange() override;
        void OnMessage(::webrtc::DataChannelObserver::OnMessage message) override;
        void OnBufferedAmountChange(::webrtc::DataChannelObserver::OnBufferedAmountChange webrtc_buffer_change);*/


        /*::rtc::scoped_refptr<::webrtc::DataChannelObserver> get_webrtc_data_channel_observer();*/
        std::unique_ptr<DataChannelObeserver_Helper> webrtc_data_channel_observer_helper_;
        //private:
        winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnMessageDelegate> on_message_event_;
        winrt::event < Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnStateChangeDelegate> on_state_change_event_;
        winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnBufferedAmountChangeDelegate> on_buffer_amount_change_event_;

        private: 
    };

    struct DataChannelObeserver_Helper : public ::webrtc::DataChannelObserver
    {
      DataChannelObeserver_Helper(
          Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataChannelObserver *create_data_channel_observer)
          : create_data_channel_observe_(create_data_channel_observer)
      {
      }

      ~DataChannelObeserver_Helper() override
      {
      }

      void
      OnStateChange() override
      {
        create_data_channel_observe_->on_state_change_event_();
      }

      void
      OnMessage(const ::webrtc::DataBuffer &buffer) override
      {
        //throw hresult_not_implemented(); 
        create_data_channel_observe_->on_message_event_(
            *make_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataBuffer>(buffer).get());
      }

      void
      OnBufferedAmountChange(uint64_t sent_data_size) override
      {
        create_data_channel_observe_->on_buffer_amount_change_event_(static_cast<int64_t>(sent_data_size));
      }

    private:
      Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::DataChannelObserver *create_data_channel_observe_;
    };

    }