// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.CreateSessionDescriptionObserver.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct CreateSessionDescriptionObserver : CreateSessionDescriptionObserverT<CreateSessionDescriptionObserver>
{
  CreateSessionDescriptionObserver();
  ~CreateSessionDescriptionObserver();

  winrt::event_token OnSuccess(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnSuccessDelegate const &handler);
  void OnSuccess(winrt::event_token const &token) noexcept;
  winrt::event_token OnFailure(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnFailureDelegate const &handler);
  void OnFailure(winrt::event_token const &token) noexcept;

  ::rtc::scoped_refptr<::webrtc::CreateSessionDescriptionObserver> get_webrtc_create_session_description_observer();

  // FIXME(aurighet): Improve encapsulation
  ::rtc::scoped_refptr<::webrtc::CreateSessionDescriptionObserver> webrtc_create_session_description_observer_helper_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnSuccessDelegate>
      on_success_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnFailureDelegate>
      on_failure_event_;

private:
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct CreateSessionDescriptionObserver
    : CreateSessionDescriptionObserverT<CreateSessionDescriptionObserver,
                                        implementation::CreateSessionDescriptionObserver>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
