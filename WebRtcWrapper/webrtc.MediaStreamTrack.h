// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.MediaStreamTrack.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct MediaStreamTrack : MediaStreamTrackT<MediaStreamTrack>, ::webrtc::ObserverInterface
{
  explicit MediaStreamTrack(::rtc::scoped_refptr<::webrtc::MediaStreamTrackInterface> webrtc_media_stream_track);
  ~MediaStreamTrack();

  static hstring AudioKind();
  static hstring VideoKind();

  hstring Kind();
  hstring Id();
  bool Enabled();
  void Enabled(bool value);
  winrt::event_token OnChanged(Microsoft::WinRTC::WebRtcWrapper::webrtc::NotifierOnChangedDelegate const &handler);
  void OnChanged(winrt::event_token const &token) noexcept;
  void AddOrUpdateSink(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoSink_VideoFrame const &sink,
                       Microsoft::WinRTC::WebRtcWrapper::rtc::VideoSinkWants const &wants);

  ::rtc::scoped_refptr<::webrtc::MediaStreamTrackInterface> get_webrtc_media_stream_track();

  void OnChanged() override;

private:
  ::rtc::scoped_refptr<::webrtc::MediaStreamTrackInterface> webrtc_media_stream_track_;
  winrt::event<Microsoft::WinRTC::WebRtcWrapper::webrtc::NotifierOnChangedDelegate> on_changed_event_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct MediaStreamTrack : MediaStreamTrackT<MediaStreamTrack, implementation::MediaStreamTrack>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
