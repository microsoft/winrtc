// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnection.RTCOfferAnswerOptions.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
struct RTCOfferAnswerOptions : RTCOfferAnswerOptionsT<RTCOfferAnswerOptions>,
                               ::webrtc::PeerConnectionInterface::RTCOfferAnswerOptions
{
  RTCOfferAnswerOptions() = default;

  bool VoiceActivityDetection();
  void VoiceActivityDetection(bool value);
  bool IceRestart();
  void IceRestart(bool value);
  bool UseRtpMux();
  void UseRtpMux(bool value);
  bool RawPacketizationForVideo();
  void RawPacketizationForVideo(bool value);
  int32_t NumSimulcastLayers();
  void NumSimulcastLayers(int32_t value);
  bool UseObsoleteSctpSdp();
  void UseObsoleteSctpSdp(bool value);
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
{
struct RTCOfferAnswerOptions : RTCOfferAnswerOptionsT<RTCOfferAnswerOptions, implementation::RTCOfferAnswerOptions>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
