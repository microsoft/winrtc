// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "AppRtcControl.g.h"

#include "AppRtc.h"

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
{
struct AppRtcControl : AppRtcControlT<AppRtcControl>
{
  Microsoft::WinRtc::Simple::VideoConferencing::AppRtc clazz{nullptr};

  AppRtcControl();

  hstring Room();
  void Room(hstring value);

  void AppRTCButton_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
};
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::factory_implementation
{
struct AppRtcControl : AppRtcControlT<AppRtcControl, implementation::AppRtcControl>
{
};
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::factory_implementation
