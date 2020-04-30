// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.CreateSessionDescriptionObserver.h"
#include "webrtc.CreateSessionDescriptionObserver.g.cpp"
// clang-format on

#include "webrtc.SessionDescription.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

// The implementation::CreateSessionDescriptionObserver class doesn't
// inherits from ::webrtc::CreateSessionDescriptionObserver because the
// latter inherits from rtc::RefCountInterface.
// rtc::RefCountInterface has two methods named AddRef and Release.
// CreateSessionDescriptionObserverT<CreateSessionDescriptionObserver> also
// have two methods named AddRef and Release.
struct CreateSessionDescriptionObserver_Helper : public ::webrtc::CreateSessionDescriptionObserver
{
  CreateSessionDescriptionObserver_Helper(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::CreateSessionDescriptionObserver
          *create_session_description_observer)
      : create_session_description_observer_(create_session_description_observer)
  {
  }

  void
  OnSuccess(::webrtc::SessionDescriptionInterface *desc) override
  {
    create_session_description_observer_->on_success_event_(make<SessionDescription>(desc));
  }

  void
  OnFailure(::webrtc::RTCError error) override
  {
    create_session_description_observer_->on_failure_event_(to_hstring(error.message()));
  }

private:
  Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::CreateSessionDescriptionObserver
      *create_session_description_observer_;
};

//  Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserver

CreateSessionDescriptionObserver::CreateSessionDescriptionObserver()
    : webrtc_create_session_description_observer_helper_(
          new ::rtc::RefCountedObject<CreateSessionDescriptionObserver_Helper>(this))
{
}

CreateSessionDescriptionObserver::~CreateSessionDescriptionObserver()
{
}

winrt::event_token
CreateSessionDescriptionObserver::OnSuccess(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnSuccessDelegate const &handler)
{
  return on_success_event_.add(handler);
}

void
CreateSessionDescriptionObserver::OnSuccess(winrt::event_token const &token) noexcept
{
  on_success_event_.remove(token);
}

winrt::event_token
CreateSessionDescriptionObserver::OnFailure(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserverOnFailureDelegate const &handler)
{
  return on_failure_event_.add(handler);
}

void
CreateSessionDescriptionObserver::OnFailure(winrt::event_token const &token) noexcept
{
  on_failure_event_.remove(token);
}

::rtc::scoped_refptr<::webrtc::CreateSessionDescriptionObserver>
CreateSessionDescriptionObserver::get_webrtc_create_session_description_observer()
{
  return webrtc_create_session_description_observer_helper_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
