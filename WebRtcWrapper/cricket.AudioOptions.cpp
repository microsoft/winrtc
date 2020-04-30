// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "cricket.AudioOptions.h"
#include "cricket.AudioOptions.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::implementation
{
AudioOptions::AudioOptions(::cricket::AudioOptions cricket_audio_options)
    : cricket_audio_options_(cricket_audio_options)
{
}

hstring
AudioOptions::ToString()
{
  return to_hstring(cricket_audio_options_.ToString());
}

const ::cricket::AudioOptions &
AudioOptions::get_cricket_audio_options() const
{
  return cricket_audio_options_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::implementation
