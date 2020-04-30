// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnectionFactory.h"
#include "webrtc.PeerConnectionFactory.g.cpp"
// clang-format on

#include "cricket.AudioOptions.h"
#include "webrtc.AudioDecoderFactory.h"
#include "webrtc.AudioEncoderFactory.h"
#include "webrtc.AudioSource.h"
#include "webrtc.AudioTrack.h"
#include "webrtc.PeerConnection.PeerConnection.h"
#include "webrtc.PeerConnection.RTCConfiguration.h"
#include "webrtc.PeerConnectionDependencies.h"
#include "webrtc.VideoEncoderFactory.h"
#include "webrtc.VideoDecoderFactory.h"
#include "webrtc.VideoTrack.h"
#include "webrtc.VideoTrackSource.h"
#include "rtc.Thread.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
PeerConnectionFactory::PeerConnectionFactory(
    Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &network_thread,
    Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &worker_thread,
    Microsoft::WinRTC::WebRtcWrapper::rtc::Thread const &signaling_thread,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioEncoderFactory const &audio_encoder_factory,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioDecoderFactory const &audio_decoder_factory,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoEncoderFactory const &video_encoder_factory,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoDecoderFactory const &video_decoder_factory)
    : webrtc_peer_connection_factory_(::webrtc::CreatePeerConnectionFactory(
          get_self<Microsoft::WinRTC::WebRtcWrapper::rtc::implementation::Thread>(network_thread)
              ->get_webrtc_thread_no_ref(),
          get_self<Microsoft::WinRTC::WebRtcWrapper::rtc::implementation::Thread>(worker_thread)
              ->get_webrtc_thread_no_ref(),
          get_self<Microsoft::WinRTC::WebRtcWrapper::rtc::implementation::Thread>(signaling_thread)
              ->get_webrtc_thread_no_ref(),
          nullptr /* default_adm */,
          get_self<AudioEncoderFactory>(audio_encoder_factory)->get_webrtc_audio_encoder_factory_no_ref(),
          get_self<AudioDecoderFactory>(audio_decoder_factory)->get_webrtc_audio_decoder_factory_no_ref(),
          get_self<VideoEncoderFactory>(video_encoder_factory)->get_webrtc_video_encoder_factory(),
          get_self<VideoDecoderFactory>(video_decoder_factory)->get_webrtc_video_decoder_factory(),
          nullptr /* audio_mixer */, nullptr /* audio_processing */))
{
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::PeerConnection
PeerConnectionFactory::CreatePeerConnection(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RTCConfiguration const &configuration,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnectionDependencies const &dependencies)
{
  return make<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation::PeerConnection>(
      webrtc_peer_connection_factory_->CreatePeerConnection(
          get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation::RTCConfiguration>(
              configuration)
              ->get_webrtc_rtc_configuration(),
          get_self<PeerConnectionDependencies>(dependencies)->build_webrtc_peer_connection_dependencies()));
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSource
PeerConnectionFactory::CreateAudioSource(Microsoft::WinRTC::WebRtcWrapper::cricket::AudioOptions const &options)
{
  return make<AudioSource>(webrtc_peer_connection_factory_->CreateAudioSource(
      get_self<Microsoft::WinRTC::WebRtcWrapper::cricket::implementation::AudioOptions>(options)
          ->get_cricket_audio_options()));
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrack
PeerConnectionFactory::CreateAudioTrack(hstring const &label,
                                        Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioSource const &audio_source)
{
  return make<AudioTrack>(webrtc_peer_connection_factory_->CreateAudioTrack(
      to_string(label), get_self<AudioSource>(audio_source)->get_webrtc_audio_source()));
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoTrack
PeerConnectionFactory::CreateVideoTrack(
    hstring const &label, Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoTrackSource const &video_track_source)
{
  return make<VideoTrack>(webrtc_peer_connection_factory_->CreateVideoTrack(
      to_string(label), get_self<VideoTrackSource>(video_track_source)->get_webrtc_video_track_source()));
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
