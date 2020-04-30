// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.AudioSource.h"
#include "webrtc.AudioSource.g.cpp"
// clang-format on

#include "cricket.AudioOptions.h"
#include "webrtc.AudioTrackSink.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

AudioSource::AudioSource(::rtc::scoped_refptr<::webrtc::AudioSourceInterface> webrtc_audio_source)
    : webrtc_audio_source_(webrtc_audio_source)
{
  webrtc_audio_source_->RegisterAudioObserver(this);
}

AudioSource::~AudioSource()
{
  webrtc_audio_source_->UnregisterAudioObserver(this);
}

void
AudioSource::OnSetVolume(double volume)
{
  on_set_volume_event_(volume);
}

float
AudioSource::Volume()
{
  // ::webrtc::AudioSourceInterface doesn't define a getter for Volume,
  // but WinRT doesn't allow set only props.
  throw hresult_not_implemented();
}

void
AudioSource::Volume(float value)
{
  webrtc_audio_source_->SetVolume(value);
}

Microsoft::WinRTC::WebRtcWrapper::cricket::AudioOptions
AudioSource::Options()
{
  return make<Microsoft::WinRTC::WebRtcWrapper::cricket::implementation::AudioOptions>(webrtc_audio_source_->options());
}

void
AudioSource::AddSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSink const &sink)
{
  webrtc_audio_source_->AddSink(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::AudioTrackSink>(sink));
}

void
AudioSource::RemoveSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSink const &sink)
{
  webrtc_audio_source_->RemoveSink(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::AudioTrackSink>(sink));
}

winrt::event_token
AudioSource::OnSetVolume(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSourceOnSetVolumeDelegate const &handler)
{
  return on_set_volume_event_.add(handler);
}

void
AudioSource::OnSetVolume(winrt::event_token const &token) noexcept
{
  on_set_volume_event_.remove(token);
}

::rtc::scoped_refptr<::webrtc::AudioSourceInterface>
AudioSource::get_webrtc_audio_source()
{
  return webrtc_audio_source_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
