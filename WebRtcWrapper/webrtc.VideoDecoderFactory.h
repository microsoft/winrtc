// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoDecoderFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct VideoDecoderFactory : VideoDecoderFactoryT<VideoDecoderFactory>
{
  VideoDecoderFactory() = default;

  VideoDecoderFactory(::std::unique_ptr<::webrtc::VideoDecoderFactory> webrtc_video_decoder_factory);

  ::std::unique_ptr<::webrtc::VideoDecoderFactory> get_webrtc_video_decoder_factory();

private:
  ::std::unique_ptr<::webrtc::VideoDecoderFactory> webrtc_video_decoder_factory_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct VideoDecoderFactory : VideoDecoderFactoryT<VideoDecoderFactory, implementation::VideoDecoderFactory>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation