// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.RtpSender.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct RtpSender : RtpSenderT<RtpSender>
{
  explicit RtpSender(::rtc::scoped_refptr<::webrtc::RtpSenderInterface> webrtc_rtc_sender_);

private:
  ::rtc::scoped_refptr<::webrtc::RtpSenderInterface> webrtc_rtc_sender_;
};

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
