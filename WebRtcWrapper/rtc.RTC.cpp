// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "rtc.RTC.h"
#include "rtc.RTC.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{

void
RTC::InitializeSSL()
{
  ::rtc::InitializeSSL();
}

void
RTC::CleanupSSL()
{
  ::rtc::CleanupSSL();
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
