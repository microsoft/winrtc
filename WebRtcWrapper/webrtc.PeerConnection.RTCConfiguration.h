// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnection.RTCConfiguration.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
struct RTCConfiguration : RTCConfigurationT<RTCConfiguration>
{
  RTCConfiguration() = default;

  Windows::Foundation::Collections::IVector<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer>
  IceServers();
  void IceServers(Windows::Foundation::Collections::IVector<
                  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceServer> const &value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceTransportType IceTransportType();
  void IceTransportType(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::IceTransportType const &value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::BundlePolicy BundlePolicy();
  void BundlePolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::BundlePolicy const &value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RtcpMuxPolicy RtcpMuxPolicy();
  void RtcpMuxPolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RtcpMuxPolicy const &value);
  int32_t IceCandidatePoolSize();
  void IceCandidatePoolSize(int32_t value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::SdpSemantics SdpSemantics();
  void SdpSemantics(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::SdpSemantics const &value);

  ::webrtc::PeerConnectionInterface::RTCConfiguration get_webrtc_rtc_configuration() const;

private:
  ::webrtc::PeerConnectionInterface::RTCConfiguration webrtc_rtc_configuration_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
{
struct RTCConfiguration : RTCConfigurationT<RTCConfiguration, implementation::RTCConfiguration>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
