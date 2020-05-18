// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "AppRtc.h"
#include "AppRtc.g.cpp"
// clang-format on

#include <stddef.h>

#include <memory>

#include "api/media_stream_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/peer_connection_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "pc/video_track_source.h"
#include "rtc_base/rtc_certificate_generator.h"
#include "rtc_base/ssl_adapter.h"

#include "api/video/i420_buffer.h"
#include "modules/video_capture/video_capture_factory.h"
#include "modules/video_capture/windows/device_info_winrt.h"
#include "libyuv.h"

#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "rtc_base/critical_section.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class TestVideoCapturer : public ::rtc::VideoSourceInterface<::webrtc::VideoFrame>
{
public:
  class FramePreprocessor
  {
  public:
    virtual ~FramePreprocessor() = default;

    virtual ::webrtc::VideoFrame Preprocess(const ::webrtc::VideoFrame &frame) = 0;
  };

  ~TestVideoCapturer() override;

  void AddOrUpdateSink(rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink, const rtc::VideoSinkWants &wants) override;
  void RemoveSink(rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink) override;
  void
  SetFramePreprocessor(std::unique_ptr<FramePreprocessor> preprocessor)
  {
    rtc::CritScope crit(&lock_);
    preprocessor_ = std::move(preprocessor);
  }

protected:
  void OnFrame(const ::webrtc::VideoFrame &frame);
  rtc::VideoSinkWants GetSinkWants();

private:
  void UpdateVideoAdapter();
  ::webrtc::VideoFrame MaybePreprocess(const ::webrtc::VideoFrame &frame);

  rtc::CriticalSection lock_;
  std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
  rtc::VideoBroadcaster broadcaster_;
  cricket::VideoAdapter video_adapter_;
};

TestVideoCapturer::~TestVideoCapturer() = default;

void
TestVideoCapturer::OnFrame(const ::webrtc::VideoFrame &original_frame)
{
  int cropped_width = 0;
  int cropped_height = 0;
  int out_width = 0;
  int out_height = 0;

  ::webrtc::VideoFrame frame = MaybePreprocess(original_frame);

  if (!video_adapter_.AdaptFrameResolution(frame.width(), frame.height(), frame.timestamp_us() * 1000, &cropped_width,
                                           &cropped_height, &out_width, &out_height))
  {
    // Drop frame in order to respect frame rate constraint.
    return;
  }

  if (out_height != frame.height() || out_width != frame.width())
  {
    // Video adapter has requested a down-scale. Allocate a new buffer and
    // return scaled version.
    // For simplicity, only scale here without cropping.
    ::rtc::scoped_refptr<::webrtc::I420Buffer> scaled_buffer = ::webrtc::I420Buffer::Create(out_width, out_height);
    scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
    ::webrtc::VideoFrame::Builder new_frame_builder = ::webrtc::VideoFrame::Builder()
                                                          .set_video_frame_buffer(scaled_buffer)
                                                          .set_rotation(::webrtc::kVideoRotation_0)
                                                          .set_timestamp_us(frame.timestamp_us())
                                                          .set_id(frame.id());
    if (frame.has_update_rect())
    {
      ::webrtc::VideoFrame::UpdateRect new_rect = frame.update_rect().ScaleWithFrame(
          frame.width(), frame.height(), 0, 0, frame.width(), frame.height(), out_width, out_height);
      new_frame_builder.set_update_rect(new_rect);
    }
    broadcaster_.OnFrame(new_frame_builder.build());
  }
  else
  {
    // No adaptations needed, just return the frame as is.
    broadcaster_.OnFrame(frame);
  }
}

rtc::VideoSinkWants
TestVideoCapturer::GetSinkWants()
{
  return broadcaster_.wants();
}

void
TestVideoCapturer::AddOrUpdateSink(rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink,
                                   const rtc::VideoSinkWants &wants)
{
  broadcaster_.AddOrUpdateSink(sink, wants);
  UpdateVideoAdapter();
}

void
TestVideoCapturer::RemoveSink(rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink)
{
  broadcaster_.RemoveSink(sink);
  UpdateVideoAdapter();
}

void
TestVideoCapturer::UpdateVideoAdapter()
{
  video_adapter_.OnSinkWants(broadcaster_.wants());
}

::webrtc::VideoFrame
TestVideoCapturer::MaybePreprocess(const ::webrtc::VideoFrame &frame)
{
  rtc::CritScope crit(&lock_);
  if (preprocessor_ != nullptr)
  {
    return preprocessor_->Preprocess(frame);
  }
  else
  {
    return frame;
  }
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

struct SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
{
  void
  OnSuccess() override
  {
  }

  void
  OnFailure(webrtc::RTCError error) override
  {
  }
};

struct SignalingImpl
{
  const winrt::hstring _eua = L"Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko";
  const winrt::hstring _ref = L"https://appr.tc";
  const winrt::Windows::Networking::Sockets::MessageWebSocket _messageWebSocket;

  winrt::hstring _rom;
  winrt::hstring _cid;
  winrt::hstring _wss;
  winrt::hstring _ice;

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection = nullptr;

  SignalingImpl(winrt::hstring room) : _rom(room)
  {
    // Create an HttpClient object.
    ///////////////////////////////////////////////////////////////////////////
    winrt::Windows::Web::Http::HttpClient httpClient;

    // Add a user-agent header to the GET request.
    auto headers{httpClient.DefaultRequestHeaders()};
    headers.UserAgent().TryParseAdd(_eua);

    winrt::Windows::Foundation::Uri url{_ref + L"/join/" + _rom};
    winrt::Windows::Web::Http::HttpStringContent content{L""};

    // Send the POST request.
    auto res = httpClient.PostAsync(url, content).get();
    auto body = res.Content().ReadAsStringAsync().get();

    auto json = winrt::Windows::Data::Json::JsonObject::Parse(body);
    auto params = json.GetNamedObject(L"params");

    _cid = params.GetNamedString(L"client_id");
    _wss = params.GetNamedString(L"wss_url");
    _ice = params.GetNamedString(L"ice_server_url");
    ///////////////////////////////////////////////////////////////////////////
  }

  //~SignalingImpl() {
  //	if (_peerConnection != nullptr) {
  //		winrt::Windows::Storage::Streams::DataWriter dataWriter{ _messageWebSocket.OutputStream() };
  //		dataWriter.WriteString(L"{\"cmd\":\"send\",\"msg\":\"{\\\"type\\\":\\\"bye\\\"}\"}");
  //		dataWriter.StoreAsync().get();
  //		dataWriter.DetachStream();
  //	}

  //	{
  //		// Create an HttpClient object.
  //		winrt::Windows::Web::Http::HttpClient httpClient;

  //		// Add a user-agent header to the GET request.
  //		auto headers{ httpClient.DefaultRequestHeaders() };
  //		headers.UserAgent().TryParseAdd(_eua);

  //		winrt::Windows::Foundation::Uri url{ _ref + L"/leave/" + _rom  + L"/" + _cid };
  //		winrt::Windows::Web::Http::HttpStringContent content{ L"" };

  //		// Send the POST request.
  //		httpClient.PostAsync(url, content).get();
  //	}
  //}

  void
  PopulateServers(std::vector<webrtc::PeerConnectionInterface::IceServer> &servers)
  {
    ///////////////////////////////////////////////////////////////////////////

    // Create an HttpClient object.
    winrt::Windows::Web::Http::HttpClient httpClient;

    // Add a user-agent header to the GET request.
    auto headers{httpClient.DefaultRequestHeaders()};
    headers.Referer(winrt::Windows::Foundation::Uri{_ref});

    winrt::Windows::Foundation::Uri url{_ice};
    winrt::Windows::Web::Http::HttpStringContent content{L""};

    // Send the POST request.
    auto res = httpClient.PostAsync(url, content).get();
    auto body = res.Content().ReadAsStringAsync().get();

    auto jsonRoot = winrt::Windows::Data::Json::JsonObject::Parse(body);
    for (auto jsonIceServer : jsonRoot.GetNamedArray(L"iceServers"))
    {
      webrtc::PeerConnectionInterface::IceServer iceServer;

      auto jsonObject = jsonIceServer.GetObject();

      for (auto jsonURL : jsonObject.GetNamedArray(L"urls"))
        iceServer.urls.push_back(winrt::to_string(jsonURL.GetString()));

      iceServer.username =
          winrt::to_string(jsonObject.GetNamedString(L"username", winrt::to_hstring(iceServer.username)));
      iceServer.password =
          winrt::to_string(jsonObject.GetNamedString(L"credential", winrt::to_hstring(iceServer.password)));

      servers.push_back(iceServer);
    }
    ///////////////////////////////////////////////////////////////////////////
  }

  void
  RegisterPeerConnection(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection)
  {
    _peerConnection = peerConnection;

    // In this example, we send/receive a string, so we need to set the MessageType to Utf8.
    _messageWebSocket.SetRequestHeader(L"Origin", _ref);
    _messageWebSocket.MessageReceived({this, &SignalingImpl::MessageReceived});
    _messageWebSocket.ConnectAsync(winrt::Windows::Foundation::Uri{_wss}).get();

    winrt::Windows::Storage::Streams::DataWriter dataWriter{_messageWebSocket.OutputStream()};
    dataWriter.WriteString(L"{\"cmd\":\"register\",\"roomid\":\"" + _rom + L"\",\"clientid\":\"" + _cid + L"\"}");
    dataWriter.StoreAsync().get();
    dataWriter.DetachStream();
  }

  void
  MessageReceived(winrt::Windows::Networking::Sockets::MessageWebSocket const & /* sender */,
                  winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs const &args)
  {
    winrt::Windows::Storage::Streams::DataReader dataReader{args.GetDataReader()};

    auto message = dataReader.ReadString(dataReader.UnconsumedBufferLength());
    auto json = winrt::Windows::Data::Json::JsonObject::Parse(message);
    auto msg = winrt::Windows::Data::Json::JsonObject::Parse(json.GetNamedString(L"msg"));

    auto type = msg.GetNamedString(L"type");

    if (type == L"answer")
    {
      auto sdp = msg.GetNamedString(L"sdp");

      std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
          webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, winrt::to_string(sdp));
      _peerConnection->SetRemoteDescription(new rtc::RefCountedObject<SetSessionDescriptionObserver>(),
                                            session_description.release());

      return;
    }

    if (type == L"candidate")
    {
      auto id = msg.GetNamedString(L"id");
      auto label = msg.GetNamedNumber(L"label");
      auto candidate = msg.GetNamedString(L"candidate");

      webrtc::IceCandidateInterface *ice_candidate = webrtc::CreateIceCandidate(
          winrt::to_string(id), static_cast<int>(label), winrt::to_string(candidate), nullptr);
      assert(ice_candidate);

      bool ok = _peerConnection->AddIceCandidate(ice_candidate);
      assert(ok);

      return;
    }

    throw winrt::hresult_not_implemented();
  }

  void
  SendOffer(winrt::hstring sdp) const
  {
    // Create an HttpClient object.
    winrt::Windows::Web::Http::HttpClient httpClient;

    // Add a user-agent header to the GET request.
    auto headers{httpClient.DefaultRequestHeaders()};
    headers.UserAgent().TryParseAdd(_eua);

    auto json = winrt::Windows::Data::Json::JsonValue::CreateStringValue(winrt::to_hstring(sdp));

    winrt::Windows::Foundation::Uri url{_ref + L"/message/" + _rom + L"/" + _cid};
    winrt::Windows::Web::Http::HttpStringContent content{L"{\"sdp\":" + json.Stringify() + L",\"type\":\"offer\"}"};

    // Send the POST request.
    httpClient.PostAsync(url, content);
  }

  void
  SendCandidate(winrt::hstring candidate) const
  {
    // Create an HttpClient object.
    winrt::Windows::Web::Http::HttpClient httpClient;

    // Add a user-agent header to the GET request.
    auto headers{httpClient.DefaultRequestHeaders()};
    headers.UserAgent().TryParseAdd(_eua);

    auto json = winrt::Windows::Data::Json::JsonValue::CreateStringValue(candidate);

    winrt::Windows::Foundation::Uri url{_ref + L"/message/" + _rom + L"/" + _cid};
    winrt::Windows::Web::Http::HttpStringContent content{
        L"{\"type\":\"candidate\",\"label\":0,\"id\":\"0\",\"candidate\":" + json.Stringify() + L"}"};

    // Send the POST request.
    httpClient.PostAsync(url, content);
  }
};

struct VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
  const winrt::Windows::UI::Core::CoreDispatcher _uiThread;

  VideoRenderer(winrt::Windows::UI::Xaml::UIElement canvas) : _uiThread(canvas.Dispatcher())
  {
    VideoRendererAsync(canvas);
  }

  winrt::Microsoft::Graphics::Canvas::CanvasDevice _canvasDevice;
  winrt::Windows::UI::Composition::CompositionDrawingSurface _surface{nullptr};

  winrt::Windows::Foundation::IAsyncAction
  VideoRendererAsync(winrt::Windows::UI::Xaml::UIElement canvas)
  {
    co_await winrt::resume_foreground(_uiThread);

    winrt::Windows::UI::Composition::Compositor compositor = winrt::Windows::UI::Xaml::Window::Current().Compositor();

    co_await winrt::resume_background();

    winrt::Windows::UI::Composition::CompositionGraphicsDevice compositionGraphicsDevice =
        winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::CreateCompositionGraphicsDevice(
            compositor, _canvasDevice);
    _surface = compositionGraphicsDevice.CreateDrawingSurface(
        {0, 0}, winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    winrt::Windows::UI::Composition::CompositionSurfaceBrush brush = compositor.CreateSurfaceBrush(_surface);
    brush.HorizontalAlignmentRatio(.5);
    brush.VerticalAlignmentRatio(.5);
    brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::Uniform);

    winrt::Windows::UI::Composition::SpriteVisual visual = compositor.CreateSpriteVisual();
    visual.Brush(brush);
    visual.RelativeSizeAdjustment(winrt::Windows::Foundation::Numerics::float2::one());

    co_await winrt::resume_foreground(_uiThread);

    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(canvas, visual);
  }

  void
  OnFrame(const webrtc::VideoFrame &frame) override
  {
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());

    webrtc::VideoRotation rotation = frame.rotation();
    if (rotation != webrtc::kVideoRotation_0)
    {
      buffer = webrtc::I420Buffer::Rotate(*buffer, rotation);
    }

    int32_t width = buffer->width();
    int32_t height = buffer->height();

    size_t bits = 32;
    size_t size = width * height * (bits >> 3);

    std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
    libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(),
                       buffer->StrideV(), data.get(), width * bits / 8, width, height);

    PaintFrameAsync(std::move(data), size, width, height);
  }

  winrt::Windows::Foundation::IAsyncAction
  PaintFrameAsync(std::unique_ptr<uint8_t[]> data, size_t length, int32_t width, int32_t height)
  {
    co_await winrt::resume_background();

    auto raw = data.get();
    auto view = winrt::array_view<uint8_t const>(raw, raw + length);
    auto bitmap = winrt::Microsoft::Graphics::Canvas::CanvasBitmap::CreateFromBytes(
        _canvasDevice, view, width, height,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized);

    if (_surface.Size() != bitmap.Size())
      winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::Resize(_surface, bitmap.Size());

    co_await winrt::resume_foreground(_uiThread);

    winrt::Microsoft::Graphics::Canvas::CanvasDrawingSession drawingSession =
        winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::CreateDrawingSession(_surface);
    {
      drawingSession.Clear(winrt::Windows::UI::Colors::Transparent());
      drawingSession.DrawImage(bitmap);
    }
    drawingSession.Close();
  }
};

struct VideoCapturer : public rtc::VideoSinkInterface<webrtc::VideoFrame>, public ::TestVideoCapturer
{
  rtc::scoped_refptr<webrtc::VideoCaptureModule> _videoCaptureModule;

  VideoCapturer()
  {
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> device_info(
        webrtc::VideoCaptureFactory::CreateDeviceInfo());

    // Returns the id for the first camera connected to the computer.
    char id[256];
    device_info->GetDeviceName(0, nullptr, 0, id, sizeof(id), nullptr, 0);

    webrtc::VideoCaptureCapability capability;
    device_info->GetCapability(id, 0, capability);

    // Sets up camera and starts capturing.
    _videoCaptureModule = webrtc::VideoCaptureFactory::Create(id);
    _videoCaptureModule->RegisterCaptureDataCallback(this);
    _videoCaptureModule->StartCapture(capability);
    _videoCaptureModule->CaptureStarted();
  }

  ~VideoCapturer()
  {
    _videoCaptureModule->StopCapture();
    _videoCaptureModule->DeRegisterCaptureDataCallback();
  }

  void
  OnFrame(const webrtc::VideoFrame &frame) override
  {
    TestVideoCapturer::OnFrame(frame);
  }
};

struct VideoTrackSource : public webrtc::VideoTrackSource
{
  VideoCapturer _videoCapturer;

  VideoTrackSource() : webrtc::VideoTrackSource(false)
  {
  }

  rtc::VideoSourceInterface<webrtc::VideoFrame> *
  source() override
  {
    return &_videoCapturer;
  }
};

struct PeerConnectionObserver : public webrtc::PeerConnectionObserver
{
  const VideoRenderer _videoRenderer;
  const SignalingImpl *_signaling;

  PeerConnectionObserver(winrt::Windows::UI::Xaml::UIElement canvas, SignalingImpl *signaling)
      : _videoRenderer(canvas), _signaling(signaling)
  {
  }

  virtual ~PeerConnectionObserver() = default;

  // Triggered when the SignalingState changed.
  void
  OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override
  {
  }

  // Triggered when a remote peer opens a data channel.
  void
  OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override
  {
  }

  // Triggered when renegotiation is needed. For example, an ICE restart
  // has begun.
  void
  OnRenegotiationNeeded() override
  {
  }

  // Called any time the IceGatheringState changes.
  void
  OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override
  {
  }

  // A new ICE candidate has been gathered.
  void
  OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override
  {
    std::string cnddt;
    candidate->ToString(&cnddt);

    _signaling->SendCandidate(winrt::to_hstring(cnddt));
  }

  // This is called when signaling indicates a transceiver will be receiving
  // media from the remote endpoint. This is fired during a call to
  // SetRemoteDescription. The receiving track can be accessed by:
  // |transceiver->receiver()->track()| and its associated streams by
  // |transceiver->receiver()->streams()|.
  // Note: This will only be called if Unified Plan semantics are specified.
  // This behavior is specified in section 2.2.8.2.5 of the "Set the
  // RTCSessionDescription" algorithm:
  // https://w3c.github.io/webrtc-pc/#set-description
  void
  OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override
  {
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = transceiver->receiver()->track();
    if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
      static_cast<webrtc::VideoTrackInterface *>(track.get())
          ->AddOrUpdateSink(const_cast<VideoRenderer *>(&_videoRenderer), rtc::VideoSinkWants());
    }
  }
};

struct CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver
{
  const rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;
  const SignalingImpl *_signaling;

  CreateSessionDescriptionObserver(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection,
                                   SignalingImpl *signaling)
      : _peerConnection(peerConnection), _signaling(signaling)
  {
  }

  void
  OnSuccess(webrtc::SessionDescriptionInterface *desc) override
  {
    _peerConnection->SetLocalDescription(new rtc::RefCountedObject<SetSessionDescriptionObserver>(), desc);

    std::string sdp;
    desc->ToString(&sdp);

    _signaling->SendOffer(winrt::to_hstring(sdp));
  }

  void
  OnFailure(webrtc::RTCError error) override
  {
  }
};

namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
{
struct Impl
{
  const char *_streamId = "stream_id";
  const char *_audioLabel = "audio_label";
  const char *_videoLabel = "video_label";

  SignalingImpl _signaling;

  std::unique_ptr<rtc::Thread> _networkThread;
  std::unique_ptr<rtc::Thread> _workerThread;
  std::unique_ptr<rtc::Thread> _signalingThread;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peerConnectionFactory;

  std::unique_ptr<PeerConnectionObserver> _peerConnectionObserver;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;

  Impl(winrt::hstring room) : _signaling(room)
  {
    _networkThread = rtc::Thread::CreateWithSocketServer();
    _networkThread->SetName("pc_network_thread", nullptr);
    _networkThread->Start();

    _workerThread = rtc::Thread::Create();
    _workerThread->SetName("pc_worker_thread", nullptr);
    _workerThread->Start();

    _signalingThread = rtc::Thread::Create();
    _signalingThread->SetName("pc_signaling_thread", nullptr);
    _signalingThread->Start();

    _peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
        _networkThread.get(), _workerThread.get(), _signalingThread.get(), nullptr /* default_adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(), webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(), webrtc::CreateBuiltinVideoDecoderFactory(),
        nullptr /* audio_mixer */, nullptr /* audio_processing */);
  }

  void
  CreatePeerConnection(winrt::Windows::UI::Xaml::UIElement canvas)
  {
    _peerConnectionObserver.reset(new PeerConnectionObserver(canvas, &_signaling));

    webrtc::PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    config.enable_dtls_srtp = true;

    _signaling.PopulateServers(config.servers);

    _peerConnection =
        _peerConnectionFactory->CreatePeerConnection(config, nullptr, nullptr, _peerConnectionObserver.get());
    assert(_peerConnection);

    _signaling.RegisterPeerConnection(_peerConnection);
  }

  void
  AddAudioTrack()
  {
    auto audioSrc = _peerConnectionFactory->CreateAudioSource(cricket::AudioOptions());
    auto audioTrack = _peerConnectionFactory->CreateAudioTrack(_audioLabel, audioSrc);

    auto ret = _peerConnection->AddTrack(audioTrack, {_streamId});
    assert(ret.ok());
  }

  void
  AddVideoTrack()
  {
    auto videoSrc = new rtc::RefCountedObject<VideoTrackSource>();
    auto videoTrack = _peerConnectionFactory->CreateVideoTrack(_videoLabel, videoSrc);

    auto ret = _peerConnection->AddTrack(videoTrack, {_streamId});
    assert(ret.ok());
  }

  void
  CreateOffer()
  {
    _peerConnection->CreateOffer(
        new rtc::RefCountedObject<CreateSessionDescriptionObserver>(_peerConnection, &_signaling),
        webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
  }
};

AppRtc::AppRtc()
{
  rtc::InitializeSSL();
}

AppRtc::~AppRtc()
{
  if (_impl != nullptr)
  {
    delete _impl;
  }

  rtc::CleanupSSL();
}

hstring
AppRtc::Room() const
{
  return _room;
}

Windows::Foundation::IAsyncAction
AppRtc::Init(Windows::UI::Xaml::UIElement canvas, hstring room)
{
  if (room.empty())
  {
    throw hresult_invalid_argument();
  }

  _room = room;

  // This is necessary to avoid STA
  co_await resume_background();

  _impl = new Impl(_room);

  ///////////////////////////////////////////////////////////////////////
  // CreatePeerConnection
  ///////////////////////////////////////////////////////////////////////

  _impl->CreatePeerConnection(canvas);

  ///////////////////////////////////////////////////////////////////////
  // AddTracks
  ///////////////////////////////////////////////////////////////////////

  _impl->AddAudioTrack();
  _impl->AddVideoTrack();

  ///////////////////////////////////////////////////////////////////////
  // CreateOffer
  ///////////////////////////////////////////////////////////////////////

  _impl->CreateOffer();
}
} // namespace winrt::Microsoft::WinRtc::Simple::VideoConferencing::implementation
