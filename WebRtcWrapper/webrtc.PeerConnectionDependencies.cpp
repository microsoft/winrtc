// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnectionDependencies.h"
#include "webrtc.PeerConnectionDependencies.g.cpp"
// clang-format on

#include "webrtc.PeerConnectionObserver.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
PeerConnectionDependencies::PeerConnectionDependencies(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserver const &observer)
    : peer_connection_observer_(observer)
{
}

::webrtc::PeerConnectionDependencies
PeerConnectionDependencies::build_webrtc_peer_connection_dependencies()
{
  return ::webrtc::PeerConnectionDependencies(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::PeerConnectionObserver>(
          peer_connection_observer_));
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
