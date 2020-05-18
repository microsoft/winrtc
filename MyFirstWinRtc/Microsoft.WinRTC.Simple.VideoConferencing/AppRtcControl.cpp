// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "AppRtcControl.h"
#include "AppRtcControl.g.cpp"
// clang-format on

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
{
AppRtcControl::AppRtcControl()
{
  InitializeComponent();
  clazz = Microsoft::WinRtc::Simple::VideoConferencing::AppRtc();
}

hstring
AppRtcControl::Room()
{
  return clazz.Room();
}

void
AppRtcControl::Room(hstring value)
{
  clazz.Init(Canvas(), value);
}

void
AppRtcControl::AppRTCButton_Click(Windows::Foundation::IInspectable const & /*sender*/,
                                  Windows::UI::Xaml::RoutedEventArgs const & /*e*/)
{
  Windows::ApplicationModel::DataTransfer::DataPackage dataPackage;
  dataPackage.RequestedOperation(Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy);
  dataPackage.SetText(L"https://appr.tc/r/" + Room());

  Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(dataPackage);
}
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
