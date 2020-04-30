// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnectionObserver.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct PeerConnectionObserver : PeerConnectionObserverT<PeerConnectionObserver>, public ::webrtc::PeerConnectionObserver
{
  PeerConnectionObserver() = default;

  winrt::event_token OnSignalingChange(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnSignalingChangeDelegate const &handler);
  void OnSignalingChange(winrt::event_token const &token) noexcept;
  winrt::event_token OnRenegotiationNeeded(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnRenegotiationNeededDelegate const &handler);
  void OnRenegotiationNeeded(winrt::event_token const &token) noexcept;
  winrt::event_token OnIceGatheringChange(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceGatheringChangeDelegate const &handler);
  void OnIceGatheringChange(winrt::event_token const &token) noexcept;
  winrt::event_token OnIceCandidate(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceCandidateDelegate const &handler);
  void OnIceCandidate(winrt::event_token const &token) noexcept;
  winrt::event_token OnTrack(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnTrackDelegate const &handler);
  void OnTrack(winrt::event_token const &token) noexcept;

  void OnSignalingChange(::webrtc::PeerConnectionInterface::SignalingState webrtc_new_state) override;
  void OnDataChannel(::rtc::scoped_refptr<::webrtc::DataChannelInterface> data_channel) override;
  void OnRenegotiationNeeded() override;
  void OnIceGatheringChange(::webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnIceCandidate(const ::webrtc::IceCandidateInterface *candidate) override;
  void OnTrack(::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> transceiver) override;

private:
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnSignalingChangeDelegate>
      on_signaling_change_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnRenegotiationNeededDelegate>
      on_renegotiation_needed_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceGatheringChangeDelegate>
      on_ice_gathering_change_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnIceCandidateDelegate>
      on_ice_candidate_event_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserverOnTrackDelegate> on_track_event_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct PeerConnectionObserver : PeerConnectionObserverT<PeerConnectionObserver, implementation::PeerConnectionObserver>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
