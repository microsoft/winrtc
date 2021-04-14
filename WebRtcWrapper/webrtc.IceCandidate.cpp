// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.IceCandidate.h"
#include "webrtc.IceCandidate.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
IceCandidate::IceCandidate(const ::webrtc::IceCandidateInterface *webrtc_ice_candidate)
    : webrtc_ice_candidate_(webrtc_ice_candidate)
{
  if (!webrtc_ice_candidate_)
  {
    throw hresult_invalid_argument();
  }
}

IceCandidate::~IceCandidate()
{
  // This class doesn't own the object
  // Do not delete webrtc_ice_candidate_
}

hstring
IceCandidate::SdpMid()
{
  return to_hstring(webrtc_ice_candidate_->sdp_mid());
}

void
IceCandidate::SdpMid(hstring const &value)
{
  webrtc_ice_candidate_->sdp_mid();
}

int32_t
IceCandidate::SdpMlineIndex()
{
  return webrtc_ice_candidate_->sdp_mline_index();
}

void
IceCandidate::SdpMlineIndex(int16_t value)
{
  webrtc_ice_candidate_->sdp_mline_index();
}

hstring
IceCandidate::ServerUrl()
{
  return to_hstring(webrtc_ice_candidate_->server_url());
}

void
IceCandidate::ServerUrl(hstring const &value)
{
  webrtc_ice_candidate_->server_url();
}

hstring
IceCandidate::ToString()
{
  std::string sdp;

  if (webrtc_ice_candidate_->ToString(&sdp))
  {
    return to_hstring(sdp);
  }
  else
  {
    throw hresult_illegal_method_call();
  }
}

void
IceCandidate::ToString(hstring const &value)
{
  std::string val = to_string(value);
  webrtc_ice_candidate_->ToString(&val);
}

const ::webrtc::IceCandidateInterface *
IceCandidate::get_webrtc_ice_candidate() const
{
  return webrtc_ice_candidate_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
