#include "pch.h"
#include "webrtc.VideoSink_VideoFrame_UIElement.h"
#include "webrtc.VideoSink_VideoFrame_UIElement.g.cpp"


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
struct VideoRenderer : public ::rtc::VideoSinkInterface<::webrtc::VideoFrame>
{
  winrt::Microsoft::Graphics::Canvas::CanvasDevice _canvasDevice;
  winrt::Windows::UI::Composition::CompositionDrawingSurface _surface{nullptr};
  winrt::Windows::UI::Composition::SpriteVisual visual{nullptr};

  const winrt::Windows::UI::Core::CoreDispatcher _uiThread;

  VideoRenderer(const winrt::Windows::UI::Xaml::UIElement &canvas) : _uiThread(canvas.Dispatcher())
  {
    VideoRendererAsync(canvas);
  }

  winrt::Windows::Foundation::IAsyncAction
  VideoRendererAsync(const winrt::Windows::UI::Xaml::UIElement canvas)
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

    visual = compositor.CreateSpriteVisual();
    visual.Brush(brush);
    visual.RelativeSizeAdjustment(winrt::Windows::Foundation::Numerics::float2::one());

    co_await winrt::resume_foreground(_uiThread);

    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(canvas, visual);
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
    visual.Size({static_cast<float>(width), static_cast<float>(height)});

    size_t bits = 32;
    size_t size = width * height * (bits >> 3);

    std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
    ::libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(),
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
VideoSink_VideoFrame_UIElement::VideoSink_VideoFrame_UIElement(Windows::UI::Xaml::UIElement const &canvas)
    : webrtc_video_sink_uielement_video_frame_(new VideoRenderer(canvas))
    {
    }
    VideoSink_VideoFrame_UIElement::~VideoSink_VideoFrame_UIElement()
    {
      delete static_cast<VideoRenderer *>(webrtc_video_sink_uielement_video_frame_);
    }
    ::rtc::VideoSinkInterface<::webrtc::VideoFrame> *
    VideoSink_VideoFrame_UIElement::get_webrtc_video_sink_uielement_video_frame() const
    {
      return static_cast<VideoRenderer *>(webrtc_video_sink_uielement_video_frame_);
    }
    }
