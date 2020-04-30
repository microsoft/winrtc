// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.AudioDecoderFactory.h"
#include "webrtc.AudioDecoderFactory.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
AudioDecoderFactory::AudioDecoderFactory(::rtc::scoped_refptr<::webrtc::AudioDecoderFactory> audio_decoder_factory)
    : webrtc_audio_decoder_factory_(audio_decoder_factory)
{
}

::webrtc::AudioDecoderFactory *
AudioDecoderFactory::get_webrtc_audio_decoder_factory_no_ref() const
{
  return webrtc_audio_decoder_factory_.get();
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
