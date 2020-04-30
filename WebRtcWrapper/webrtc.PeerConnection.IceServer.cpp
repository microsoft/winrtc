// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnection.IceServer.h"
#include "webrtc.PeerConnection.IceServer.g.cpp"
// clang-format off

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
  IceServer::IceServer(const ::webrtc::PeerConnectionInterface::IceServer& webrtc_ice_server) :
    webrtc_ice_server_(webrtc_ice_server)
  { }

  Windows::Foundation::Collections::IVector<hstring> IceServer::Urls()
  {
    std::vector<hstring> urls_with_hstrings;

    for (auto const& url : webrtc_ice_server_.urls)
    {
      urls_with_hstrings.push_back(to_hstring(url));
    }

    return single_threaded_vector<hstring>(std::move(urls_with_hstrings));
  }

  void IceServer::Urls(Windows::Foundation::Collections::IVector<hstring> const& value)
  {
    webrtc_ice_server_.urls.clear();

    for (auto const& url : value.GetView())
    {
      webrtc_ice_server_.urls.push_back(to_string(url));
    }
  }

  hstring IceServer::Username()
  {
    return to_hstring(webrtc_ice_server_.username);
  }

  void IceServer::Username(hstring const& value)
  {
    webrtc_ice_server_.username = to_string(value);
  }

  hstring IceServer::Password()
  {
    return to_hstring(webrtc_ice_server_.password);
  }

  void IceServer::Password(hstring const& value)
  {
    webrtc_ice_server_.password = to_string(value);
  }

  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::TlsCertPolicy IceServer::TlsCertPolicy()
  {
    switch (webrtc_ice_server_.tls_cert_policy)
    {
    case ::webrtc::PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicyInsecureNoCheck:
    {
      return TlsCertPolicy::InsecureNoCheck;
    }
    case ::webrtc::PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicySecure:
    {
      return TlsCertPolicy::Secure;
    }
    default:
    {
      throw hresult_illegal_state_change();
    }
    }
  }

  void IceServer::TlsCertPolicy(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::TlsCertPolicy const& value)
  {
    switch (value)
    {
    case TlsCertPolicy::InsecureNoCheck:
    {
      webrtc_ice_server_.tls_cert_policy = ::webrtc::PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicyInsecureNoCheck;
      break;
    }
    case TlsCertPolicy::Secure:
    {
      webrtc_ice_server_.tls_cert_policy = ::webrtc::PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicySecure;
      break;
    }
    default:
    {
      throw hresult_illegal_state_change();
    }
    }
  }

  hstring IceServer::Hostname()
  {
    return to_hstring(webrtc_ice_server_.hostname);
  }

  void IceServer::Hostname(hstring const& value)
  {
    webrtc_ice_server_.hostname = to_string(value);
  }

  Windows::Foundation::Collections::IVector<hstring> IceServer::TlsAlpnProtocols()
  {
    std::vector<hstring> protocols_with_hstrings;

    for (auto const& protocol : webrtc_ice_server_.tls_alpn_protocols)
    {
      protocols_with_hstrings.push_back(to_hstring(protocol));
    }

    return single_threaded_vector<hstring>(std::move(protocols_with_hstrings));
  }

  void IceServer::TlsAlpnProtocols(Windows::Foundation::Collections::IVector<hstring> const& value)
  {
    webrtc_ice_server_.tls_alpn_protocols.clear();

    for (auto const& protocol : value.GetView())
    {
      webrtc_ice_server_.tls_alpn_protocols.push_back(to_string(protocol));
    }
  }

  Windows::Foundation::Collections::IVector<hstring> IceServer::TlsEllipticCurves()
  {
    std::vector<hstring> curves_with_hstrings;

    for (auto const& curve : webrtc_ice_server_.tls_elliptic_curves)
    {
      curves_with_hstrings.push_back(to_hstring(curve));
    }

    return single_threaded_vector<hstring>(std::move(curves_with_hstrings));
  }

  void IceServer::TlsEllipticCurves(Windows::Foundation::Collections::IVector<hstring> const& value)
  {
    webrtc_ice_server_.tls_elliptic_curves.clear();

    for (auto const& curve : value.GetView())
    {
      webrtc_ice_server_.tls_elliptic_curves.push_back(to_string(curve));
    }
  }

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
