// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnection.IceServer.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
struct IceServer : IceServerT<IceServer>
{
  friend struct RTCConfiguration;

  IceServer() = default;
  explicit IceServer(const ::webrtc::PeerConnectionInterface::IceServer &webrtc_ice_server);

  Windows::Foundation::Collections::IVector<hstring> Urls();
  void Urls(Windows::Foundation::Collections::IVector<hstring> const &value);
  hstring Username();
  void Username(hstring const &value);
  hstring Password();
  void Password(hstring const &value);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::TlsCertPolicy TlsCertPolicy();
  void TlsCertPolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::TlsCertPolicy const &value);
  hstring Hostname();
  void Hostname(hstring const &value);
  Windows::Foundation::Collections::IVector<hstring> TlsAlpnProtocols();
  void TlsAlpnProtocols(Windows::Foundation::Collections::IVector<hstring> const &value);
  Windows::Foundation::Collections::IVector<hstring> TlsEllipticCurves();
  void TlsEllipticCurves(Windows::Foundation::Collections::IVector<hstring> const &value);

private:
  ::webrtc::PeerConnectionInterface::IceServer webrtc_ice_server_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
{
struct IceServer : IceServerT<IceServer, implementation::IceServer>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::factory_implementation
