// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.IceCandidate.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct IceCandidate : IceCandidateT<IceCandidate>
{
  IceCandidate(const ::webrtc::IceCandidateInterface *webrtc_ice_candidate);
  ~IceCandidate();

  hstring SdpMid();
  int32_t SdpMlineIndex();
  hstring ServerUrl();
  hstring ToString();

  const ::webrtc::IceCandidateInterface *get_webrtc_ice_candidate() const;

private:
  const ::webrtc::IceCandidateInterface *webrtc_ice_candidate_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
