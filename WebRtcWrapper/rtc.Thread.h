// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "rtc.Thread.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{
struct Thread : ThreadT<Thread>
{
  Thread(std::unique_ptr<::rtc::Thread> webrtc_thread);

  static Microsoft::WinRTC::WebRtcWrapper::rtc::Thread CreateWithSocketServer();
  static Microsoft::WinRTC::WebRtcWrapper::rtc::Thread Create();
  bool Start();
  hstring Name();
  void Name(hstring const &value);

  ::rtc::Thread *get_webrtc_thread_no_ref() const noexcept;

private:
  std::unique_ptr<::rtc::Thread> webrtc_thread_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
{
struct Thread : ThreadT<Thread, implementation::Thread>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::factory_implementation
