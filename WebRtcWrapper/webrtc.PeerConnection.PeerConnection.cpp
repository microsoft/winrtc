// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.PeerConnection.PeerConnection.h"
#include "webrtc.PeerConnection.PeerConnection.g.cpp"
// clang-format on

#include "webrtc.IceCandidate.h"
#include "webrtc.SetSessionDescriptionObserver.h"
#include "webrtc.SessionDescription.h"
#include "webrtc.AudioTrack.h"
#include "webrtc.VideoTrack.h"
#include "webrtc.RtpSender.h"
#include "webrtc.CreateSessionDescriptionObserver.h"
#include "webrtc.PeerConnection.RTCOfferAnswerOptions.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
{
PeerConnection::PeerConnection(::rtc::scoped_refptr<::webrtc::PeerConnectionInterface> webrtc_peer_connection)
    : webrtc_peer_connection_(webrtc_peer_connection)
{
}

bool
PeerConnection::AddIceCandidate(Microsoft::WinRTC::WebRtcWrapper::webrtc::IceCandidate const &value)
{
  return webrtc_peer_connection_->AddIceCandidate(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::IceCandidate>(value)
          ->get_webrtc_ice_candidate());
}

void
PeerConnection::SetLocalDescription(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserver const &observer,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SessionDescription const &desc)
{
  // FIXME: observer and desc don't have clear ownership. SetLocalDescription
  //  is transfering the underlying objects.
  webrtc_peer_connection_->SetLocalDescription(
      static_cast<::webrtc::SetSessionDescriptionObserver *>(
          get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SetSessionDescriptionObserver>(observer)
              ->webrtc_set_session_description_observer_helper_),
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SessionDescription>(desc)
          ->get_webrtc_session_description());
}

void
PeerConnection::SetRemoteDescription(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SetSessionDescriptionObserver const &observer,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::SessionDescription const &desc)
{
  // FIXME: observer and desc don't have clear ownership. SetRemoteDescription
  //  is transfering the underlying objects.
  webrtc_peer_connection_->SetRemoteDescription(
      static_cast<::webrtc::SetSessionDescriptionObserver *>(
          get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SetSessionDescriptionObserver>(observer)
              ->webrtc_set_session_description_observer_helper_),
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::SessionDescription>(desc)
          ->get_webrtc_session_description());
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::RtpSender
PeerConnection::AddAudioTrack(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrack const &track,
                              Windows::Foundation::Collections::IVector<hstring> const &stream_ids)
{
  std::vector<std::string> stream_ids_string;

  for (auto const &stream_id : stream_ids)
  {
    stream_ids_string.push_back(to_string(stream_id));
  }

  auto rtc_sender_error = webrtc_peer_connection_->AddTrack(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::AudioTrack>(track)->get_webrtc_audio_track(),
      stream_ids_string);

  if (rtc_sender_error.ok())
  {
    return make<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::RtpSender>(rtc_sender_error.MoveValue());
  }
  else
  {
    throw hresult_illegal_state_change();
  }
}

Microsoft::WinRTC::WebRtcWrapper::webrtc::RtpSender
PeerConnection::AddVideoTrack(Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoTrack const &track,
                              Windows::Foundation::Collections::IVector<hstring> const &stream_ids)
{
  std::vector<std::string> stream_ids_string;

  for (auto const &stream_id : stream_ids)
  {
    stream_ids_string.push_back(to_string(stream_id));
  }

  auto rtc_sender_error = webrtc_peer_connection_->AddTrack(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::VideoTrack>(track)->get_webrtc_video_track(),
      stream_ids_string);

  if (rtc_sender_error.ok())
  {
    return make<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::RtpSender>(rtc_sender_error.MoveValue());
  }
  else
  {
    throw hresult_illegal_state_change();
  }
}

void
PeerConnection::CreateOffer(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserver const &observer,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RTCOfferAnswerOptions const &options)
{
  webrtc_peer_connection_->CreateOffer(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::CreateSessionDescriptionObserver>(observer)
          ->get_webrtc_create_session_description_observer()
          .get(),
      *get_self<RTCOfferAnswerOptions>(options));
}

void
PeerConnection::CreateAnswer(
    Microsoft::WinRTC::WebRtcWrapper::webrtc::CreateSessionDescriptionObserver const &observer,
    Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::RTCOfferAnswerOptions const &options)
{
  webrtc_peer_connection_->CreateAnswer(
      get_self<Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation::CreateSessionDescriptionObserver>(observer)
          ->get_webrtc_create_session_description_observer()
          .get(),
      *get_self<RTCOfferAnswerOptions>(options));
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::PeerConnection::implementation
