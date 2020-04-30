// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "rtc.Thread.h"
#include "rtc.Thread.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
{
Thread::Thread(std::unique_ptr<::rtc::Thread> webrtc_thread) : webrtc_thread_(move(webrtc_thread))
{
}

Microsoft::WinRTC::WebRtcWrapper::rtc::Thread
Thread::CreateWithSocketServer()
{
  return make<Thread>(::rtc::Thread::CreateWithSocketServer());
}

Microsoft::WinRTC::WebRtcWrapper::rtc::Thread
Thread::Create()
{
  return make<Thread>(::rtc::Thread::Create());
}

bool
Thread::Start()
{
  return webrtc_thread_->Start();
}

hstring
Thread::Name()
{
  return to_hstring(webrtc_thread_->name());
}

void
Thread::Name(hstring const &value)
{
  webrtc_thread_->SetName(to_string(value), nullptr);
}

::rtc::Thread *
Thread::get_webrtc_thread_no_ref() const noexcept
{
  return webrtc_thread_.get();
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::rtc::implementation
