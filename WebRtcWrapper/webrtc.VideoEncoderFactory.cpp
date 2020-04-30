// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoEncoderFactory.h"
#include "webrtc.VideoEncoderFactory.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
VideoEncoderFactory::VideoEncoderFactory(::std::unique_ptr<::webrtc::VideoEncoderFactory> webrtc_video_encoder_factory)
    : webrtc_video_encoder_factory_(move(webrtc_video_encoder_factory))
{
}

::std::unique_ptr<::webrtc::VideoEncoderFactory>
VideoEncoderFactory::get_webrtc_video_encoder_factory()
{
  if (webrtc_video_encoder_factory_)
  {
    return move(webrtc_video_encoder_factory_);
  }
  else
  {
    throw hresult_illegal_method_call();
  }
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
