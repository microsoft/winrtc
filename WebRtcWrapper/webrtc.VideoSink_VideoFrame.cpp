// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoSink_VideoFrame.h"
#include "webrtc.VideoSink_VideoFrame.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoRenderer : public ::rtc::VideoSinkInterface<::webrtc::VideoFrame>
{
  winrt::Microsoft::Graphics::Canvas::CanvasDevice _canvasDevice;
  winrt::Windows::UI::Composition::CompositionDrawingSurface _surface{nullptr};
  winrt::Windows::UI::Composition::SpriteVisual visual_{nullptr};

  VideoRenderer(const winrt::Windows::UI::Composition::VisualCollection &visual_collection)
  {
    VideoRendererAsync(visual_collection);
  }

  ~VideoRenderer()
  {
  }

  void
  VideoRendererAsync(const winrt::Windows::UI::Composition::VisualCollection &visual_collection)
  {
    winrt::Windows::UI::Composition::Compositor compositor = visual_collection.Compositor();

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

    visual_ = compositor.CreateSpriteVisual();
    visual_.Brush(brush);

    visual_collection.InsertAtTop(visual_);
  }

  void
  OnFrame(const ::webrtc::VideoFrame &frame) override
  {
    ::rtc::scoped_refptr<::webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());

    ::webrtc::VideoRotation rotation = frame.rotation();
    if (rotation != ::webrtc::kVideoRotation_0)
    {
      buffer = ::webrtc::I420Buffer::Rotate(*buffer, rotation);
    }

    int32_t width = buffer->width();
    int32_t height = buffer->height();
    visual_.Size({static_cast<float>(width), static_cast<float>(height)});

    size_t bits = 32;
    size_t size = width * height * (bits >> 3);

    std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
    ::libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(),
                         buffer->StrideV(), data.get(), width * bits / 8, width, height);

    PaintFrameAsync(std::move(data), size, width, height);
  }

  void
  PaintFrameAsync(std::unique_ptr<uint8_t[]> data, size_t length, int32_t width, int32_t height)
  {
    auto raw = data.get();
    auto view = winrt::array_view<uint8_t const>(raw, raw + length);
    auto bitmap = winrt::Microsoft::Graphics::Canvas::CanvasBitmap::CreateFromBytes(
        _canvasDevice, view, width, height,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized);

    if (_surface.Size() != bitmap.Size())
      winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::Resize(_surface, bitmap.Size());

    winrt::Microsoft::Graphics::Canvas::CanvasDrawingSession drawingSession =
        winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::CreateDrawingSession(_surface);
    {
      drawingSession.Clear(winrt::Windows::UI::Colors::Transparent());
      drawingSession.DrawImage(bitmap);
    }
    drawingSession.Close();
  }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VideoSink_VideoFrame::VideoSink_VideoFrame(const winrt::Windows::UI::Composition::VisualCollection &visual_collection)
    : webrtc_video_sink_video_frame_(new VideoRenderer(visual_collection))
{
}

VideoSink_VideoFrame::~VideoSink_VideoFrame()
{
  delete static_cast<VideoRenderer *>(webrtc_video_sink_video_frame_);
}

::rtc::VideoSinkInterface<::webrtc::VideoFrame> *
VideoSink_VideoFrame::get_webrtc_video_sink_video_frame() const
{
  return static_cast<VideoRenderer *>(webrtc_video_sink_video_frame_);
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
