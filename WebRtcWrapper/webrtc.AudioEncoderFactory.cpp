// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.AudioEncoderFactory.h"
#include "webrtc.AudioEncoderFactory.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
AudioEncoderFactory::AudioEncoderFactory(
    ::rtc::scoped_refptr<::webrtc::AudioEncoderFactory> webrtc_audio_encoder_factory)
    : webrtc_audio_encoder_factory_(webrtc_audio_encoder_factory)
{
}

::webrtc::AudioEncoderFactory *
AudioEncoderFactory::get_webrtc_audio_encoder_factory_no_ref() const
{
  return webrtc_audio_encoder_factory_.get();
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
