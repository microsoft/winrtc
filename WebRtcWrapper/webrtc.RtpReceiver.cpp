// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.RtpReceiver.h"
#include "webrtc.RtpReceiver.g.cpp"
// clang-format on

#include "webrtc.MediaStreamTrack.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
RtpReceiver::RtpReceiver(::rtc::scoped_refptr<::webrtc::RtpReceiverInterface> webrtc_rtp_receiver)
    : webrtc_rtp_receiver_(webrtc_rtp_receiver)
{
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::MediaStreamTrack
RtpReceiver::Track()
{
  return make<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::MediaStreamTrack>(
      webrtc_rtp_receiver_->track());
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
