// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.SetSessionDescriptionObserver.h"
#include "webrtc.SetSessionDescriptionObserver.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
// ----------------------------------------------------------------------------
//  ::webrtc::SetSessionDescriptionObserver
// ----------------------------------------------------------------------------

// The implementation::SetSessionDescriptionObserver class doesn't
// inherits from ::webrtc::SetSessionDescriptionObserver because the
// latter inherits from rtc::RefCountInterface.
// rtc::RefCountInterface has two methods named AddRef and Release.
// SetSessionDescriptionObserverT<SetSessionDescriptionObserver> also
// have two methods named AddRef and Release.
struct SetSessionDescriptionObserver_Helper : public ::webrtc::SetSessionDescriptionObserver
{
  SetSessionDescriptionObserver_Helper(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SetSessionDescriptionObserver
          *set_session_description_observer)
      : set_session_description_observer_(set_session_description_observer)
  {
  }

  void
  AddRef() const
  {
  }

  ::rtc::RefCountReleaseStatus
  Release() const
  {
    // The life cycle of this object is attached to
    // implementation::SetSessionDescriptionObserver
    return ::rtc::RefCountReleaseStatus::kDroppedLastRef;
  }

  void
  OnSuccess() override
  {
    set_session_description_observer_->on_success_event_();
  }

  void
  OnFailure(::webrtc::RTCError error) override
  {
    set_session_description_observer_->on_failure_event_(to_hstring(error.message()));
  }

private:
  Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SetSessionDescriptionObserver
      *set_session_description_observer_;
};

//  Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserver

SetSessionDescriptionObserver::SetSessionDescriptionObserver()
    : webrtc_set_session_description_observer_helper_(new SetSessionDescriptionObserver_Helper(this))
{
}

SetSessionDescriptionObserver::~SetSessionDescriptionObserver()
{
  delete static_cast<SetSessionDescriptionObserver_Helper *>(webrtc_set_session_description_observer_helper_);
}

event_token
SetSessionDescriptionObserver::OnSuccess(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnSuccessDelegate const &handler)
{
  return on_success_event_.add(handler);
}

void
SetSessionDescriptionObserver::OnSuccess(winrt::event_token const &token) noexcept
{
  on_success_event_.remove(token);
}

event_token
SetSessionDescriptionObserver::OnFailure(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserverOnFailureDelegate const &handler)
{
  return on_failure_event_.add(handler);
}

void
SetSessionDescriptionObserver::OnFailure(winrt::event_token const &token) noexcept
{
  on_failure_event_.remove(token);
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
