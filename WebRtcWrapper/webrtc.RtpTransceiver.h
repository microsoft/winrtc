// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.RtpTransceiver.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct RtpTransceiver : RtpTransceiverT<RtpTransceiver>
{
  explicit RtpTransceiver(::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> webrtc_rtp_transceiver);

  Microsoft::WinRTC::WebRtcWrapper::webrtc::RtpReceiver Receiver();

  ::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> get_webrtc_rtp_transceiver();

private:
  ::rtc::scoped_refptr<::webrtc::RtpTransceiverInterface> webrtc_rtp_transceiver_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
