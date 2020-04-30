// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.VideoEncoderFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct VideoEncoderFactory : VideoEncoderFactoryT<VideoEncoderFactory>
{
  VideoEncoderFactory(::std::unique_ptr<::webrtc::VideoEncoderFactory> webrtc_video_encoder_factory);

  ::std::unique_ptr<::webrtc::VideoEncoderFactory> get_webrtc_video_encoder_factory();

private:
  ::std::unique_ptr<::webrtc::VideoEncoderFactory> webrtc_video_encoder_factory_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
