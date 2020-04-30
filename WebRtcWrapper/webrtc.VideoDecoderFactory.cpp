// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoDecoderFactory.h"
#include "webrtc.VideoDecoderFactory.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
VideoDecoderFactory::VideoDecoderFactory(::std::unique_ptr<::webrtc::VideoDecoderFactory> webrtc_video_decoder_factory)
    : webrtc_video_decoder_factory_(move(webrtc_video_decoder_factory))
{
}

::std::unique_ptr<::webrtc::VideoDecoderFactory>
VideoDecoderFactory::get_webrtc_video_decoder_factory()
{
  if (webrtc_video_decoder_factory_)
  {
    return move(webrtc_video_decoder_factory_);
  }
  else
  {
    throw hresult_illegal_method_call();
  }
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
