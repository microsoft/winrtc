#pragma once
#include "webrtc.DataChannelObserver.g.h"

// Note: Remove this static_assert after copying these generated source files to your project.
// This assertion exists to avoid compiling these generated source files directly.


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
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

        ::webrtc::DataChannelObserver* webrtc_data_channel_observer_helper_;

        /*void OnStateChange() override;
        void OnMessage(::webrtc::DataChannelObserver::OnMessage message);
        void OnBufferedAmountChange(::webrtc::DataChannelObserver::OnBufferedAmountChange webrtc_buffer_change);*/
        ::rtc::scoped_refptr<::webrtc::DataChannelObserver> get_webrtc_data_channel_observer();

        //private:
        winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnMessageDelegate> on_message_event_;
        winrt::event < Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnStateChangeDelegate> on_state_change_event_;
        winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::DataChannelObserverOnBufferedAmountChangeDelegate> on_buffer_amount_change_event_;

        private: 
    };
}