// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnection.RTCConfiguration.h"
#include "webrtc.PeerConnection.RTCConfiguration.g.cpp"
// clang-format on

#include "webrtc.PeerConnection.IceServer.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
::webrtc::PeerConnectionInterface::RTCConfiguration
RTCConfiguration::get_webrtc_rtc_configuration() const
{
  return webrtc_rtc_configuration_;
}

Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer>
RTCConfiguration::IceServers()
{
  std::vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer> ice_servers;

  for (auto const &webrtc_ice_server : webrtc_rtc_configuration_.servers)
  {
    IceServer ice_server(webrtc_ice_server);
    ice_servers.push_back(ice_server);
  }

  return single_threaded_vector<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer>(
      std::move(ice_servers));
}

void
RTCConfiguration::IceServers(
    Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer> const
        &value)
{
  webrtc_rtc_configuration_.servers.clear();

  for (auto const &server : value.GetView())
  {
    webrtc_rtc_configuration_.servers.push_back(
        get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation::IceServer>(server)
            ->webrtc_ice_server_);
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceTransportType
RTCConfiguration::IceTransportType()
{
  switch (webrtc_rtc_configuration_.type)
  {
  case ::webrtc::PeerConnectionInterface::IceTransportsType::kAll: {
    return IceTransportType::All;
  }
  case ::webrtc::PeerConnectionInterface::IceTransportsType::kNoHost: {
    return IceTransportType::NoHost;
  }
  case ::webrtc::PeerConnectionInterface::IceTransportsType::kNone: {
    return IceTransportType::None;
  }
  case ::webrtc::PeerConnectionInterface::IceTransportsType::kRelay: {
    return IceTransportType::Relay;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
RTCConfiguration::IceTransportType(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceTransportType const &value)
{
  switch (value)
  {
  case IceTransportType::All: {
    webrtc_rtc_configuration_.type = ::webrtc::PeerConnectionInterface::IceTransportsType::kAll;
    break;
  }
  case IceTransportType::NoHost: {
    webrtc_rtc_configuration_.type = ::webrtc::PeerConnectionInterface::IceTransportsType::kNoHost;
    break;
  }
  case IceTransportType::None: {
    webrtc_rtc_configuration_.type = ::webrtc::PeerConnectionInterface::IceTransportsType::kNone;
    break;
  }
  case IceTransportType::Relay: {
    webrtc_rtc_configuration_.type = ::webrtc::PeerConnectionInterface::IceTransportsType::kRelay;
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::BundlePolicy
RTCConfiguration::BundlePolicy()
{
  switch (webrtc_rtc_configuration_.bundle_policy)
  {
  case ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyBalanced: {
    return BundlePolicy::Balanced;
  }
  case ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxBundle: {
    return BundlePolicy::MaxBundle;
  }
  case ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxCompat: {
    return BundlePolicy::MaxCompat;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
RTCConfiguration::BundlePolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::BundlePolicy const &value)
{
  switch (value)
  {
  case BundlePolicy::Balanced: {
    webrtc_rtc_configuration_.bundle_policy = ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyBalanced;
    break;
  }
  case BundlePolicy::MaxBundle: {
    webrtc_rtc_configuration_.bundle_policy = ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxBundle;
    break;
  }
  case BundlePolicy::MaxCompat: {
    webrtc_rtc_configuration_.bundle_policy = ::webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxCompat;
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RtcpMuxPolicy
RTCConfiguration::RtcpMuxPolicy()
{
  switch (webrtc_rtc_configuration_.rtcp_mux_policy)
  {
  case ::webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyNegotiate: {
    return RtcpMuxPolicy::Negotiate;
  }
  case ::webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyRequire: {
    return RtcpMuxPolicy::Require;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
RTCConfiguration::RtcpMuxPolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RtcpMuxPolicy const &value)
{
  switch (value)
  {
  case RtcpMuxPolicy::Negotiate: {
    webrtc_rtc_configuration_.rtcp_mux_policy =
        ::webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyNegotiate;
    break;
  }
  case RtcpMuxPolicy::Require: {
    webrtc_rtc_configuration_.rtcp_mux_policy = ::webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyRequire;
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

int32_t
RTCConfiguration::IceCandidatePoolSize()
{
  return webrtc_rtc_configuration_.ice_candidate_pool_size;
}

void
RTCConfiguration::IceCandidatePoolSize(int32_t value)
{
  webrtc_rtc_configuration_.ice_candidate_pool_size = value;
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::SdpSemantics
RTCConfiguration::SdpSemantics()
{
  switch (webrtc_rtc_configuration_.sdp_semantics)
  {
  case ::webrtc::SdpSemantics::kPlanB: {
    return SdpSemantics::PlanB;
  }
  case ::webrtc::SdpSemantics::kUnifiedPlan: {
    return SdpSemantics::UnifiedPlan;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

void
RTCConfiguration::SdpSemantics(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::SdpSemantics const &value)
{
  switch (value)
  {
  case SdpSemantics::PlanB: {
    webrtc_rtc_configuration_.sdp_semantics = ::webrtc::SdpSemantics::kPlanB;
    break;
  }
  case SdpSemantics::UnifiedPlan: {
    webrtc_rtc_configuration_.sdp_semantics = ::webrtc::SdpSemantics::kUnifiedPlan;
    break;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
