// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.VideoTrackSource.h"
#include "webrtc.VideoTrackSource.g.cpp"
// clang-format on

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{

// FIXME(auright): This entire class needs to be refactored.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class QuickVideoCapture : public ::rtc::VideoSourceInterface<::webrtc::VideoFrame>
{
public:
  class FramePreprocessor
  {
  public:
    virtual ~FramePreprocessor() = default;

    virtual ::webrtc::VideoFrame Preprocess(const ::webrtc::VideoFrame &frame) = 0;
  };

  ~QuickVideoCapture() override;

  void AddOrUpdateSink(::rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink,
                       const ::rtc::VideoSinkWants &wants) override;
  void RemoveSink(::rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink) override;
  void
  SetFramePreprocessor(std::unique_ptr<FramePreprocessor> preprocessor)
  {
    ::rtc::CritScope crit(&lock_);
    preprocessor_ = std::move(preprocessor);
  }

protected:
  void OnFrame(const ::webrtc::VideoFrame &frame);
  ::rtc::VideoSinkWants GetSinkWants();

private:
  void UpdateVideoAdapter();
  ::webrtc::VideoFrame MaybePreprocess(const ::webrtc::VideoFrame &frame);

  ::rtc::CriticalSection lock_;
  std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
  ::rtc::VideoBroadcaster broadcaster_;
  ::cricket::VideoAdapter video_adapter_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

QuickVideoCapture::~QuickVideoCapture() = default;

void
QuickVideoCapture::OnFrame(const ::webrtc::VideoFrame &original_frame)
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
                                                          .set_rotation(::webrtc::VideoRotation::kVideoRotation_0)
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

::rtc::VideoSinkWants
QuickVideoCapture::GetSinkWants()
{
  return broadcaster_.wants();
}

void
QuickVideoCapture::AddOrUpdateSink(::rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink,
                                   const ::rtc::VideoSinkWants &wants)
{
  broadcaster_.AddOrUpdateSink(sink, wants);
  UpdateVideoAdapter();
}

void
QuickVideoCapture::RemoveSink(::rtc::VideoSinkInterface<::webrtc::VideoFrame> *sink)
{
  broadcaster_.RemoveSink(sink);
  UpdateVideoAdapter();
}

void
QuickVideoCapture::UpdateVideoAdapter()
{
  video_adapter_.OnSinkWants(broadcaster_.wants());
}

::webrtc::VideoFrame
QuickVideoCapture::MaybePreprocess(const ::webrtc::VideoFrame &frame)
{
  ::rtc::CritScope crit(&lock_);
  if (preprocessor_ != nullptr)
  {
    return preprocessor_->Preprocess(frame);
  }
  else
  {
    return frame;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct QuickVideoCapturer : public ::rtc::VideoSinkInterface<::webrtc::VideoFrame>, public QuickVideoCapture
{
  ::rtc::scoped_refptr<::webrtc::VideoCaptureModule> _videoCaptureModule;

  QuickVideoCapturer()
  {
    std::unique_ptr<::webrtc::VideoCaptureModule::DeviceInfo> device_info(
        ::webrtc::VideoCaptureFactory::CreateDeviceInfo());

    char id[256];
    device_info->GetDeviceName(0, nullptr, 0, id, sizeof(id), nullptr, 0);

    ::webrtc::VideoCaptureCapability capability;
    device_info->GetCapability(id, 0, capability);

    _videoCaptureModule = ::webrtc::VideoCaptureFactory::Create(id);
    _videoCaptureModule->RegisterCaptureDataCallback(this);
    _videoCaptureModule->StartCapture(capability);
    _videoCaptureModule->CaptureStarted();
  }

  ~QuickVideoCapturer()
  {
    _videoCaptureModule->StopCapture();
    _videoCaptureModule->DeRegisterCaptureDataCallback();
  }

  void
  OnFrame(const ::webrtc::VideoFrame &frame) override
  {
    QuickVideoCapture::OnFrame(frame);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct QuickVideoTrackSource : public ::webrtc::VideoTrackSource
{
  QuickVideoCapturer video_capturer_;

  QuickVideoTrackSource() : ::webrtc::VideoTrackSource(false)
  {
  }

  ::rtc::VideoSourceInterface<::webrtc::VideoFrame> *
  source() override
  {
    return &video_capturer_;
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

VideoTrackSource::VideoTrackSource() : webrtc_video_track_source_(new ::rtc::RefCountedObject<QuickVideoTrackSource>())
{
}

VideoTrackSource::~VideoTrackSource()
{
}

::rtc::scoped_refptr<::webrtc::VideoTrackSourceInterface>
VideoTrackSource::get_webrtc_video_track_source()
{
  return webrtc_video_track_source_;
}

} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
