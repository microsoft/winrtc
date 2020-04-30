// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.PeerConnectionFactory.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

struct PeerConnectionFactory : PeerConnectionFactoryT<PeerConnectionFactory>
{
  PeerConnectionFactory(Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &network_thread,
                        Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &worker_thread,
                        Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &signaling_thread,
                        Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioEncoderFactory const &audio_encoder_factory,
                        Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioDecoderFactory const &audio_decoder_factory,
                        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoEncoderFactory const &video_encoder_factory,
                        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoDecoderFactory const &video_decoder_factory);

  Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::PeerConnection CreatePeerConnection(
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RTCConfiguration const &configuration,
      Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionDependencies const &dependencies);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSource CreateAudioSource(
      Microsoft::WinRTC::WebRtcWrapper::cricket::AudioOptions const &options);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrack CreateAudioTrack(
      hstring const &label, Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSource const &audio_source);
  Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoTrack CreateVideoTrack(
      hstring const &label, Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoTrackSource const &video_track_source);

private:
  ::rtc::scoped_refptr<::webrtc::PeerConnectionFactoryInterface> webrtc_peer_connection_factory_;
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
struct PeerConnectionFactory : PeerConnectionFactoryT<PeerConnectionFactory, implementation::PeerConnectionFactory>
{
};
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
