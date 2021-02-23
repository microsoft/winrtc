// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "AppRtc.g.h"

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
{
struct AppRtc : AppRtcT<AppRtc>
{
  struct Impl *_impl = nullptr;

  AppRtc();
  ~AppRtc();

  hstring _room;
  hstring Room() const;

  Windows::Foundation::IAsyncAction AppRtc::Init(Windows::UI::Xaml::UIElement canvas, hstring room);
};
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::factory_implementation
{
struct AppRtc : AppRtcT<AppRtc, implementation::AppRtc>
{
};
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::factory_implementation
