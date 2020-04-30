// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnectionDependencies.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct PeerConnectionDependencies : PeerConnectionDependenciesT<PeerConnectionDependencies>
{
  explicit PeerConnectionDependencies(Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserver const &observer);

  ::webrtc::PeerConnectionDependencies build_webrtc_peer_connection_dependencies();

private:
  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionObserver peer_connection_observer_{nullptr};
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct PeerConnectionDependencies
    : PeerConnectionDependenciesT<PeerConnectionDependencies, implementation::PeerConnectionDependencies>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
