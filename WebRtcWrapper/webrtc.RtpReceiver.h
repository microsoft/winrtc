// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.RtpReceiver.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct RtpReceiver : RtpReceiverT<RtpReceiver>
{
  explicit RtpReceiver(::rtc::scoped_refptr<::webrtc::RtpReceiverInterface> webrtc_rtp_receiver);

  Microsoft::WinRTC::WebRtcWrapper::webrtc::MediaStreamTrack Track();

private:
  ::rtc::scoped_refptr<::webrtc::RtpReceiverInterface> webrtc_rtp_receiver_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
