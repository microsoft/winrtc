// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "cricket.AudioOptions.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::implementation
{
struct AudioOptions : AudioOptionsT<AudioOptions>
{
  AudioOptions() = default;
  explicit AudioOptions(::cricket::AudioOptions cricket_audio_options);

  hstring ToString();

  const ::cricket::AudioOptions &get_cricket_audio_options() const;

private:
  ::cricket::AudioOptions cricket_audio_options_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::factory_implementation
{
struct AudioOptions : AudioOptionsT<AudioOptions, implementation::AudioOptions>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::cricket::factory_implementation
