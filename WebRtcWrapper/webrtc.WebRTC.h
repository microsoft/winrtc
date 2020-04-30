// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.WebRTC.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct WebRTC
{
  WebRTC() = default;

  static Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioDecoderFactory CreateBuiltinAudioDecoderFactory();
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioEncoderFactory CreateBuiltinAudioEncoderFactory();
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoDecoderFactory CreateBuiltinVideoDecoderFactory();
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoEncoderFactory CreateBuiltinVideoEncoderFactory();
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::SessionDescription CreateSessionDescription(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType const &type, hstring const &sdp);
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::IceCandidate CreateIceCandidate(hstring const &sdp_mid,
                                                                                   int32_t sdp_mline_index,
                                                                                   hstring const &sdp);
  static Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType SdpTypeFromString(hstring const &type_str);
  static hstring SdpTypeToString(Microsoft::WinRTC::WebRtcWrapper::webrtc::SdpType const &sdp_type);
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct WebRTC : WebRTCT<WebRTC, implementation::WebRTC>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
