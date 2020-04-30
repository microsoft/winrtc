// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.RtpSender.h"
#include "webrtc.RtpSender.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
RtpSender::RtpSender(::rtc::scoped_refptr<::webrtc::RtpSenderInterface> webrtc_rtc_sender)
    : webrtc_rtc_sender_(webrtc_rtc_sender)
{
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
