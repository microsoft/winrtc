// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.SetSessionDescriptionObserver.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct SetSessionDescriptionObserver : SetSessionDescriptionObserverT<SetSessionDescriptionObserver>
{
  SetSessionDescriptionObserver();
  ~SetSessionDescriptionObserver();

  winrt::event_token OnSuccess(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnSuccessDelegate const &handler);
  void OnSuccess(winrt::event_token const &token) noexcept;
  winrt::event_token OnFailure(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnFailureDelegate const &handler);
  void OnFailure(winrt::event_token const &token) noexcept;

  // FIXME(aurighet): Improve encapsulation
  void *webrtc_set_session_description_observer_helper_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnSuccessDelegate>
      on_success_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnFailureDelegate>
      on_failure_event_;

private:
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct SetSessionDescriptionObserver
    : SetSessionDescriptionObserverT<SetSessionDescriptionObserver, implementation::SetSessionDescriptionObserver>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
