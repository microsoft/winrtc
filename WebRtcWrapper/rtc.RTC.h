// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "rtc.RTC.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{
struct RTC
{
  RTC() = default;

  static void InitializeSSL();
  static void CleanupSSL();
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
{
struct RTC : RTCT<RTC, implementation::RTC>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
