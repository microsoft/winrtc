// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.SessionDescription.h"
#include "webrtc.SessionDescription.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

::webrtc::SdpType
SessionDescription::to_webrtc_sdp_type(Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType sdp_type)
{
  switch (sdp_type)
  {
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Offer: {
    return ::webrtc::SdpType::kOffer;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::PrAnswer: {
    return ::webrtc::SdpType::kPrAnswer;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Answer: {
    return ::webrtc::SdpType::kAnswer;
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Rollback: {
    return ::webrtc::SdpType::kRollback;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType
SessionDescription::to_winrtc_sdp_type(::webrtc::SdpType sdp_type)
{
  switch (sdp_type)
  {
  case ::webrtc::SdpType::kOffer: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Offer;
  }
  case ::webrtc::SdpType::kPrAnswer: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::PrAnswer;
  }
  case ::webrtc::SdpType::kAnswer: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Answer;
  }
  case ::webrtc::SdpType::kRollback: {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Rollback;
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

hstring
SessionDescription::Offer()
{
  return to_hstring(std::string(::webrtc::SessionDescriptionInterface::kOffer));
}

hstring
SessionDescription::PrAnswer()
{
  return to_hstring(std::string(::webrtc::SessionDescriptionInterface::kPrAnswer));
}

hstring
SessionDescription::Answer()
{
  return to_hstring(std::string(::webrtc::SessionDescriptionInterface::kAnswer));
}

hstring
SessionDescription::Rollback()
{
  return to_hstring(std::string(::webrtc::SessionDescriptionInterface::kRollback));
}

SessionDescription::SessionDescription(
    std::unique_ptr<::webrtc::SessionDescriptionInterface> webrtc_session_description)
    : webrtc_session_description_(webrtc_session_description.release()), owns_webrtc_session_description_(true)
{
}

SessionDescription::SessionDescription(::webrtc::SessionDescriptionInterface *webrtc_session_description)
    : webrtc_session_description_(webrtc_session_description), owns_webrtc_session_description_(false)
{
}

SessionDescription::~SessionDescription()
{
  if (owns_webrtc_session_description_)
  {
    delete webrtc_session_description_;
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType
SessionDescription::Type()
{
  return to_winrtc_sdp_type(webrtc_session_description_->GetType());
}

hstring
SessionDescription::SdpTypeToString(Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType const &sdp_type)
{
  switch (sdp_type)
  {
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Offer: {
    return Offer();
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::PrAnswer: {
    return PrAnswer();
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Answer: {
    return Answer();
  }
  case Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Rollback: {
    return Rollback();
  }
  default: {
    throw hresult_illegal_state_change();
  }
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType
SessionDescription::SdpTypeFromString(hstring const &type_str)
{
  if (type_str == Offer())
  {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Offer;
  }
  else if (type_str == PrAnswer())
  {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::PrAnswer;
  }
  else if (type_str == Answer())
  {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Answer;
  }
  else if (type_str == Rollback())
  {
    return Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType::Rollback;
  }
  else
  {
    throw hresult_illegal_state_change();
  }
}
::webrtc::SessionDescriptionInterface *
SessionDescription::get_webrtc_session_description() const
{
  return webrtc_session_description_;
}

hstring
SessionDescription::ToString()
{
  std::string out;
  if (webrtc_session_description_->ToString(&out))
  {
    return to_hstring(out);
  }
  else
  {
    throw hresult_illegal_state_change();
  }
}
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
