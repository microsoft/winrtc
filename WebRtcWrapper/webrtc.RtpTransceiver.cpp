// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.RtpTransceiver.h"
#include "webrtc.RtpTransceiver.g.cpp"
// clang-format on

#include "webrtc.RtpReceiver.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
RtpTransceiver::RtpTransceiver(::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> webrtc_rtp_transceiver)
    : webrtc_rtp_transceiver_(std::move(webrtc_rtp_transceiver))
{
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::RtpReceiver
RtpTransceiver::Receiver()
{
  return make<RtpReceiver>(webrtc_rtp_transceiver_->receiver());
}

::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface>
RtpTransceiver::get_webrtc_rtp_transceiver()
{
  return webrtc_rtp_transceiver_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
