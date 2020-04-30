// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.AudioDecoderFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct AudioDecoderFactory : AudioDecoderFactoryT<AudioDecoderFactory>
{
  AudioDecoderFactory(::rtc::scoped_refptr<::webrtc::AudioDecoderFactory> audio_decoder_factory);

  ::webrtc::AudioDecoderFactory *get_webrtc_audio_decoder_factory_no_ref() const;

private:
  ::rtc::scoped_refptr<::webrtc::AudioDecoderFactory> webrtc_audio_decoder_factory_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

