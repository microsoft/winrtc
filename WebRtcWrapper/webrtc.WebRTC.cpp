// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.WebRTC.h"
#include "webrtc.WebRTC.g.cpp"
// clang-format on

#include "webrtc.AudioDecoderFactory.h"
#include "webrtc.AudioEncoderFactory.h"
#include "webrtc.IceCandidate.h"
#include "webrtc.SessionDescription.h"
#include "webrtc.VideoDecoderFactory.h"
#include "webrtc.VideoEncoderFactory.h"

using winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType;

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
hstring
WebRTC::SdpTypeToString(SdpType const &sdp_type)
{
  return SessionDescription::SdpTypeToString(sdp_type);
}

SdpType
WebRTC::SdpTypeFromString(hstring const &type_str)
{
  return SessionDescription::SdpTypeFromString(type_str);
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::SessionDescription
WebRTC::CreateSessionDescription(SdpType const &type, hstring const &sdp)
{
  return make<SessionDescription>(
      ::webrtc::CreateSessionDescription(SessionDescription::to_webrtc_sdp_type(type), to_string(sdp)));
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::IceCandidate
WebRTC::CreateIceCandidate(hstring const &sdp_mid, int32_t sdp_mline_index, hstring const &sdp)
{
  ::webrtc::SdpParseError sdp_parse_error;

  const ::webrtc::IceCandidateInterface *ice_candidate =
      ::webrtc::CreateIceCandidate(to_string(sdp_mid), sdp_mline_index, to_string(sdp), &sdp_parse_error);

  if (ice_candidate)
  {
    return make<IceCandidate>(ice_candidate);
  }
  else
  {
    std::wstringstream ss;
    ss << L"SdpParserError\r\n";
    ss << sdp_parse_error.description.c_str();
    ss << L"\r\n";
    ss << sdp_parse_error.line.c_str();
    ss << L"\r\n";
    throw hresult_invalid_argument(to_hstring(ss.str().c_str()));
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioDecoderFactory
WebRTC::CreateBuiltinAudioDecoderFactory()
{
  return make<AudioDecoderFactory>(::webrtc::CreateBuiltinAudioDecoderFactory());
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioEncoderFactory
WebRTC::CreateBuiltinAudioEncoderFactory()
{
  return make<AudioEncoderFactory>(::webrtc::CreateBuiltinAudioEncoderFactory());
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoEncoderFactory
WebRTC::CreateBuiltinVideoEncoderFactory()
{
  return make<VideoEncoderFactory>(::webrtc::CreateBuiltinVideoEncoderFactory());
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoDecoderFactory
WebRTC::CreateBuiltinVideoDecoderFactory()
{
  return make<VideoDecoderFactory>(::webrtc::CreateBuiltinVideoDecoderFactory());
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
