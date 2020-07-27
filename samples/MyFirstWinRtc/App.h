// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "App.xaml.g.h"

namespace winrt::MyFirstWinRtc::implementation
{
struct App : AppT<App>
{
  App();

  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);
};
} // namespace winrt::MyFirstWinRtc::implementation
