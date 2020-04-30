// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.AudioEncoderFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct AudioEncoderFactory : AudioEncoderFactoryT<AudioEncoderFactory>
{
  AudioEncoderFactory(::rtc::scoped_refptr<::webrtc::AudioEncoderFactory> webrtc_audio_encoder_factory);

  ::webrtc::AudioEncoderFactory *get_webrtc_audio_encoder_factory_no_ref() const;

private:
  ::rtc::scoped_refptr<::webrtc::AudioEncoderFactory> webrtc_audio_encoder_factory_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
