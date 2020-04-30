// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnectionObserver.h"
#include "webrtc.PeerConnectionObserver.g.cpp"
// clang-format on

#include "webrtc.IceCandidate.h"
#include "webrtc.RtpTransceiver.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
event_token
PeerConnectionObserver::OnSignalingChange(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnSignalingChangeDelegate const &handler)
{
  return on_signaling_change_event_.add(handler);
}

void
PeerConnectionObserver::OnSignalingChange(event_token const &token) noexcept
{
  on_signaling_change_event_.remove(token);
}

event_token
PeerConnectionObserver::OnRenegotiationNeeded(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnRenegotiationNeededDelegate const &handler)
{
  return on_renegotiation_needed_event_.add(handler);
}

void
PeerConnectionObserver::OnRenegotiationNeeded(winrt::event_token const &token) noexcept
{
  on_renegotiation_needed_event_.remove(token);
}

event_token
PeerConnectionObserver::OnIceGatheringChange(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceGatheringChangeDelegate const &handler)
{
  return on_ice_gathering_change_event_.add(handler);
}

void
PeerConnectionObserver::OnIceGatheringChange(winrt::event_token const &token) noexcept
{
  on_ice_gathering_change_event_.remove(token);
}

event_token
PeerConnectionObserver::OnIceCandidate(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceCandidateDelegate const &handler)
{
  return on_ice_candidate_event_.add(handler);
}

void
PeerConnectionObserver::OnIceCandidate(event_token const &token) noexcept
{
  on_ice_candidate_event_.remove(token);
}

event_token
PeerConnectionObserver::OnTrack(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnTrackDelegate const &handler)
{
  return on_track_event_.add(handler);
}

void
PeerConnectionObserver::OnTrack(event_token const &token) noexcept
{
  on_track_event_.remove(token);
}

void
PeerConnectionObserver::OnSignalingChange(::webrtc::PeerConnectionInterface::SignalingState webrtc_new_state)
{
  switch (webrtc_new_state)
  {
  case ::webrtc::PeerConnectionInterface::SignalingState::kStable: {
    on_signaling_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::Stable);
    break;
  }
  case ::webrtc::PeerConnectionInterface::SignalingState::kHaveLocalOffer: {
    on_signaling_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::HaveLocalOffer);
    break;
  }
  case ::webrtc::PeerConnectionInterface::SignalingState::kHaveLocalPrAnswer: {
    on_signaling_change_event_(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::HaveLocalPrAnswer);
    break;
  }
  case ::webrtc::PeerConnectionInterface::SignalingState::kHaveRemoteOffer: {
    on_signaling_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::HaveRemoteOffer);
    break;
  }
  case ::webrtc::PeerConnectionInterface::SignalingState::kHaveRemotePrAnswer: {
    on_signaling_change_event_(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::HaveRemotePrAnswer);
    break;
  }
  case ::webrtc::PeerConnectionInterface::SignalingState::kClosed: {
    on_signaling_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionSignalingState::Closed);
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void PeerConnectionObserver::OnDataChannel(::rtc::scoped_refptr<::webrtc::DataChannelInterface> /*data_channel*/)
{
  throw hresult_not_implemented(); // FIXME(aurighet)
}

void
PeerConnectionObserver::OnRenegotiationNeeded()
{
  on_renegotiation_needed_event_();
}

void
PeerConnectionObserver::OnIceGatheringChange(::webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
  switch (new_state)
  {
  case ::webrtc::PeerConnectionInterface::kIceGatheringNew: {
    on_ice_gathering_change_event_(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceGatheringState::New);
    break;
  }
  case ::webrtc::PeerConnectionInterface::kIceGatheringGathering: {
    on_ice_gathering_change_event_(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceGatheringState::Gathering);
    break;
  }
  case ::webrtc::PeerConnectionInterface::kIceGatheringComplete: {
    on_ice_gathering_change_event_(
        Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceGatheringState::Complete);
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
PeerConnectionObserver::OnIceCandidate(const ::webrtc::IceCandidateInterface *candidate)
{
  if (candidate)
  {
    auto winrtc_ice_candidate = make<IceCandidate>(candidate);
    on_ice_candidate_event_(winrtc_ice_candidate);
  }
  else
  {
    throw hresult_invalid_argument();
  }
}

void
PeerConnectionObserver::OnTrack(::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> transceiver)
{
  auto winrtc_rtp_transceiver = make<RtpTransceiver>(transceiver);
  on_track_event_(winrtc_rtp_transceiver);
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
