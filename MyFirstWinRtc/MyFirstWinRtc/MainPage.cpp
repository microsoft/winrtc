// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
// clang-format on

using namespace winrt;
using namespace Windows::Security::Cryptography;
using namespace Windows::UI::Xaml;

namespace winrt::MyFirstWinRtc::implementation
{
MainPage::MainPage()
{
  InitializeComponent();

  auto room = to_hstring(CryptographicBuffer::GenerateRandomNumber());
  Control().Room(room);
}
} // namespace winrt::MyFirstWinRtc::implementation
