// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.MediaStreamTrack.h"
#include "webrtc.MediaStreamTrack.g.cpp"
// clang-format on

#include "webrtc.VideoSink_VideoFrame.h"
#include "rtc.VideoSinkWants.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

MediaStreamTrack::MediaStreamTrack(::rtc::scoped_refptr<::webrtc::MediaStreamTrackInterface> webrtc_media_stream_track)
    : webrtc_media_stream_track_(webrtc_media_stream_track)
{
  webrtc_media_stream_track_->RegisterObserver(this);
}

MediaStreamTrack::~MediaStreamTrack()
{
  webrtc_media_stream_track_->UnregisterObserver(this);
}

void
MediaStreamTrack::OnChanged()
{
  on_changed_event_();
}

hstring
MediaStreamTrack::AudioKind()
{
  return to_hstring(::webrtc::MediaStreamTrackInterface::kAudioKind);
}

hstring
MediaStreamTrack::VideoKind()
{
  return to_hstring(::webrtc::MediaStreamTrackInterface::kVideoKind);
}

hstring
MediaStreamTrack::Kind()
{
  return to_hstring(webrtc_media_stream_track_->kind());
}

hstring
MediaStreamTrack::Id()
{
  return to_hstring(webrtc_media_stream_track_->id());
}

bool
MediaStreamTrack::Enabled()
{
  return webrtc_media_stream_track_->enabled();
}

void
MediaStreamTrack::Enabled(bool value)
{
  webrtc_media_stream_track_->set_enabled(value);
}

::rtc::scoped_refptr<::webrtc::MediaStreamTrackInterface>
MediaStreamTrack::get_webrtc_media_stream_track()
{
  return webrtc_media_stream_track_;
}

winrt::event_token
MediaStreamTrack::OnChanged(Microsoft::WinRTC::WebRtcWrapper::webrtc::NotifierOnChangedDelegate const &handler)
{
  return on_changed_event_.add(handler);
}

void
MediaStreamTrack::OnChanged(winrt::event_token const &token) noexcept
{
  on_changed_event_.remove(token);
}

// FIXME(aurighet): AddOrUpdateSink belongs to VideoTrack
void
MediaStreamTrack::AddOrUpdateSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoSink_VideoFrame const &sink,
                                  Microsoft::WinRTC::WebRtcWrapper::rtc::VideoSinkWants const &wants)
{
  static_cast<::webrtc::VideoTrackInterface *>(webrtc_media_stream_track_.get())
      ->AddOrUpdateSink(get_self<implementation::VideoSink_VideoFrame>(sink)->get_webrtc_video_sink_video_frame(),
                        *get_self<Microsoft::WinRTC::WebRtcWrapper::rtc::implementation::VideoSinkWants>(wants));
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
