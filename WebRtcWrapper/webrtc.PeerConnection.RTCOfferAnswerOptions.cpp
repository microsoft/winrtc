// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnection.RTCOfferAnswerOptions.h"
#include "webrtc.PeerConnection.RTCOfferAnswerOptions.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
bool
RTCOfferAnswerOptions::VoiceActivityDetection()
{
  return voice_activity_detection;
}

void
RTCOfferAnswerOptions::VoiceActivityDetection(bool value)
{
  voice_activity_detection = value;
}

bool
RTCOfferAnswerOptions::IceRestart()
{
  return ice_restart;
}

void
RTCOfferAnswerOptions::IceRestart(bool value)
{
  ice_restart = value;
}

bool
RTCOfferAnswerOptions::UseRtpMux()
{
  return use_rtp_mux;
}

void
RTCOfferAnswerOptions::UseRtpMux(bool value)
{
  use_rtp_mux = value;
}

bool
RTCOfferAnswerOptions::RawPacketizationForVideo()
{
  return raw_packetization_for_video;
}

void
RTCOfferAnswerOptions::RawPacketizationForVideo(bool value)
{
  raw_packetization_for_video = value;
}

int32_t
RTCOfferAnswerOptions::NumSimulcastLayers()
{
  return num_simulcast_layers;
}

void
RTCOfferAnswerOptions::NumSimulcastLayers(int32_t value)
{
  num_simulcast_layers = value;
}

bool
RTCOfferAnswerOptions::UseObsoleteSctpSdp()
{
  return use_obsolete_sctp_sdp;
}

void
RTCOfferAnswerOptions::UseObsoleteSctpSdp(bool value)
{
  use_obsolete_sctp_sdp = value;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
