// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.SessionDescription.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct SessionDescription : SessionDescriptionT<SessionDescription>
{
  // This constructor takes ownership (it deletes ::webrtc::SessionDescriptionInterface)
  explicit SessionDescription(std::unique_ptr<::webrtc::SessionDescriptionInterface> webrtc_session_description);
  // This constructor works like a view (doesn't take ownership)
  explicit SessionDescription(::webrtc::SessionDescriptionInterface *webrtc_session_description);
  ~SessionDescription();

  static hstring Offer();
  static hstring PrAnswer();
  static hstring Answer();
  static hstring Rollback();

  Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType Type();

  static ::webrtc::SdpType to_webrtc_sdp_type(Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType sdp_type);
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType to_winrtc_sdp_type(::webrtc::SdpType sdp_type);
  static hstring SdpTypeToString(Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType const &sdp_type);
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType SdpTypeFromString(hstring const &type_str);

  hstring ToString();

  ::webrtc::SessionDescriptionInterface *get_webrtc_session_description() const;

private:
  ::webrtc::SessionDescriptionInterface *webrtc_session_description_;
  bool owns_webrtc_session_description_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct SessionDescription : SessionDescriptionT<SessionDescription, implementation::SessionDescription>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
