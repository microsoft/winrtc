/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#include "modules/video_coding/codecs/h264/win/encoder/h264_encoder_mf_impl.h"

#include <codecapi.h>
#include <mfapi.h>
#include "absl/strings/match.h"
#include "libyuv/convert.h"
#include "modules/video_coding/codecs/h264/win/utils/utils.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"

namespace webrtc {

// QP scaling thresholds.
static const int kLowH264QpThreshold = 24;
static const int kHighH264QpThreshold = 37;

// Used by histograms. Values of entries should not be changed.
enum H264EncoderMFImplEvent {
  kH264EncoderEventInit = 0,
  kH264EncoderEventError = 1,
  kH264EncoderEventMax = 16,
};
H264EncoderMFImpl::H264EncoderMFImpl(const cricket::VideoCodec& codec) {
  HRESULT hr = S_OK;

  RTC_CHECK(absl::EqualsIgnoreCase(codec.name, cricket::kH264CodecName));

  ON_SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET));
}

H264EncoderMFImpl::~H264EncoderMFImpl() {
  HRESULT hr = S_OK;

  Release();

  ON_SUCCEEDED(MFShutdown());
}

int H264EncoderMFImpl::InitEncoderWithSettings(
    const VideoCodec* codec_settings) {
  HRESULT hr = S_OK;

  rtc::CritScope lock(&crit_);

  // output media type (h264)
  ComPtr<IMFMediaType> mediaTypeOut;
  ON_SUCCEEDED(MFCreateMediaType(&mediaTypeOut));
  ON_SUCCEEDED(mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  ON_SUCCEEDED(mediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
  // Lumia 635 and Lumia 1520 Windows phones don't work well
  // with constrained baseline profile.
  // ON_SUCCEEDED(mediaTypeOut->SetUINT32(MF_MT_MPEG2_PROFILE,
  // eAVEncH264VProfile_ConstrainedBase));

  // Weight*Height*2 kbit represents a good balance between video quality and
  // the bandwidth that a 620 Windows phone can handle.
  ON_SUCCEEDED(mediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, target_bps_));
  ON_SUCCEEDED(mediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE,
                                       MFVideoInterlace_Progressive));
  ON_SUCCEEDED(MFSetAttributeSize(mediaTypeOut.Get(), MF_MT_FRAME_SIZE, width_,
                                  height_));
  ON_SUCCEEDED(MFSetAttributeRatio(mediaTypeOut.Get(), MF_MT_FRAME_RATE,
                                   max_frame_rate_, 1));

  // input media type (nv12)
  ComPtr<IMFMediaType> mediaTypeIn;
  ON_SUCCEEDED(MFCreateMediaType(&mediaTypeIn));
  ON_SUCCEEDED(mediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  ON_SUCCEEDED(mediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12));
  ON_SUCCEEDED(mediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE,
                                      MFVideoInterlace_Progressive));
  ON_SUCCEEDED(mediaTypeIn->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE));
  ON_SUCCEEDED(
      MFSetAttributeSize(mediaTypeIn.Get(), MF_MT_FRAME_SIZE, width_, height_));
  ON_SUCCEEDED(MFSetAttributeRatio(mediaTypeIn.Get(), MF_MT_FRAME_RATE,
                                   max_frame_rate_, 1));

  // Create the media sink
  ON_SUCCEEDED(Microsoft::WRL::MakeAndInitialize<H264MediaSink>(
      mediaSink_.ReleaseAndGetAddressOf()));

  // SinkWriter creation attributes
  ON_SUCCEEDED(MFCreateAttributes(
      sinkWriterCreationAttributes_.ReleaseAndGetAddressOf(), 1));
  ON_SUCCEEDED(sinkWriterCreationAttributes_->SetUINT32(
      MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE));
  ON_SUCCEEDED(sinkWriterCreationAttributes_->SetUINT32(
      MF_SINK_WRITER_DISABLE_THROTTLING, TRUE));
  ON_SUCCEEDED(sinkWriterCreationAttributes_->SetUINT32(MF_LOW_LATENCY, TRUE));

  // Create the sink writer
  ON_SUCCEEDED(MFCreateSinkWriterFromMediaSink(
      mediaSink_.Get(), sinkWriterCreationAttributes_.Get(),
      sinkWriter_.ReleaseAndGetAddressOf()));

  // Add the h264 output stream to the writer
  ON_SUCCEEDED(sinkWriter_->AddStream(mediaTypeOut.Get(), &streamIndex_));

  // SinkWriter encoder properties
  ON_SUCCEEDED(MFCreateAttributes(
      sinkWriterEncoderAttributes_.ReleaseAndGetAddressOf(), 1));
  ON_SUCCEEDED(
      sinkWriter_->SetInputMediaType(streamIndex_, mediaTypeIn.Get(), nullptr));

  // Register this as the callback for encoded samples.
  ON_SUCCEEDED(mediaSink_->RegisterEncodingCallback(this));

  ON_SUCCEEDED(sinkWriter_->BeginWriting());

  codec_ = *codec_settings;

  if (SUCCEEDED(hr)) {
    inited_ = true;
    lastTimeSettingsChanged_ = rtc::TimeMillis();
    return WEBRTC_VIDEO_CODEC_OK;
  } else {
    return hr;
  }
}

//
int32_t H264EncoderMFImpl::InitEncode(const VideoCodec* inst,
                                      const VideoEncoder::Settings& settings) {
  if (!inst || inst->codecType != kVideoCodecH264) {
    RTC_LOG(LS_ERROR) << "H264 UWP Encoder not registered as H264 codec";
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }
  if (inst->maxFramerate == 0) {
    RTC_LOG(LS_ERROR) << "H264 UWP Encoder has no framerate defined";
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }
  if (inst->width < 1 || inst->height < 1) {
    RTC_LOG(LS_ERROR) << "H264 UWP Encoder has no valid frame size defined";
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  width_ = inst->width;
  height_ = inst->height;
  target_bps_ = inst->startBitrate > 0 ? inst->startBitrate * 1000
                                       : width_ * height_ * 2.0;

  max_frame_rate_ = inst->maxFramerate;
  if (target_bps_ == 0) {
    target_bps_ = inst->startBitrate * 1000;
  } else {
    target_bps_ = inst->maxBitrate * 1000;
  }
  return InitEncoderWithSettings(inst);
}

int32_t H264EncoderMFImpl::Release() {
  // Use a temporary sink variable to prevent lock inversion
  // between the shutdown call and OnH264Encoded() callback.
  ComPtr<H264MediaSink> tmpMediaSink;

  {
    rtc::CritScope lock(&crit_);
    sinkWriter_.Reset();
    if (mediaSink_ != nullptr) {
      tmpMediaSink = mediaSink_;
    }
    sinkWriterCreationAttributes_.Reset();
    sinkWriterEncoderAttributes_.Reset();
    mediaSink_.Reset();
    startTime_ = 0;
    lastTimestampHns_ = 0;
    firstFrame_ = true;
    inited_ = false;
    framePendingCount_ = 0;
    _sampleAttributeQueue.clear();
    rtc::CritScope callbackLock(&callbackCrit_);
    encodedCompleteCallback_ = nullptr;
  }

  if (tmpMediaSink != nullptr) {
    tmpMediaSink->Shutdown();
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t H264EncoderMFImpl::RegisterEncodeCompleteCallback(
    EncodedImageCallback* callback) {
  rtc::CritScope lock(&callbackCrit_);
  encodedCompleteCallback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

void H264EncoderMFImpl::SetRates(const RateControlParameters& parameters) {
  RTC_LOG(LS_INFO) << "H264EncoderMFImpl::SetRates("
                   << parameters.bitrate.get_sum_bps() << "bps "
                   << parameters.framerate_fps << "fps)";

  // This may happen.  Ignore it.
  if (parameters.framerate_fps == 0) {
    max_frame_rate_ = codec_.maxFramerate;
  }

  rtc::CritScope lock(&crit_);
  if (sinkWriter_ == nullptr) {
    return;
  }

  bool bitrateUpdated = false;
  bool fpsUpdated = false;

#ifdef DYNAMIC_BITRATE
  if (target_bps_ != (parameters.bitrate.get_sum_bps() * 1000)) {
    target_bps_ = parameters.bitrate.get_sum_bps() * 1000;
    bitrateUpdated = true;
  }
#endif

#ifdef DYNAMIC_FPS
  // Fps changes seems to be expensive, make it granular to several frames per
  // second.
  if (max_frame_rate_ != parameters.framerate_fps &&
      std::abs((int)max_frame_rate_ - (int)parameters.framerate_fps) > 5) {
    max_frame_rate_ = parameters.framerate_fps;
    fpsUpdated = true;
  }
#endif

  if (bitrateUpdated || fpsUpdated) {
    if ((rtc::TimeMillis() - lastTimeSettingsChanged_) < 15000) {
      RTC_LOG(LS_INFO) << "Last time settings changed was too soon, skipping "
                          "this SetRates().\n";
      return;
    }

    EncodedImageCallback* tempCallback = encodedCompleteCallback_;
    Release();
    {
      rtc::CritScope lock(&callbackCrit_);
      encodedCompleteCallback_ = tempCallback;
    }
    InitEncoderWithSettings(&codec_);
  }
}

int32_t H264EncoderMFImpl::Encode(
    const VideoFrame& input_frame,
    const std::vector<VideoFrameType>* frame_types) {
  {
    rtc::CritScope lock(&crit_);
    if (!inited_) {
      return -1;
    }
  }

  bool is_key_frame_forced = false;

  if (frame_types != nullptr) {
    for (auto frameType : *frame_types) {
      if (frameType == VideoFrameType::kVideoFrameKey) {
        RTC_LOG(LS_INFO) << "Key frame requested in H264 encoder.";
        ComPtr<IMFSinkWriterEncoderConfig> encoderConfig;
        sinkWriter_.As(&encoderConfig);
        ComPtr<IMFAttributes> encoderAttributes;
        MFCreateAttributes(&encoderAttributes, 1);
        encoderAttributes->SetUINT32(CODECAPI_AVEncVideoForceKeyFrame, TRUE);
        encoderConfig->PlaceEncodingParameters(streamIndex_,
                                               encoderAttributes.Get());
        is_key_frame_forced = true;
        break;
      }
    }
  }

  HRESULT hr = S_OK;

  ComPtr<IMFSample> sample;
  {
    rtc::CritScope lock(&crit_);
    if (_sampleAttributeQueue.size() > 2) {
      return WEBRTC_VIDEO_CODEC_OK;
    }
    sample = FromVideoFrame(input_frame);
  }

  ON_SUCCEEDED(sinkWriter_->WriteSample(streamIndex_, sample.Get()));

  rtc::CritScope lock(&crit_);
  // Some threads online mention this is useful to do regularly.
  ++frameCount_;
  if (frameCount_ % 30 == 0) {
    ON_SUCCEEDED(sinkWriter_->NotifyEndOfSegment(streamIndex_));
  }

  ++framePendingCount_;

  if (is_key_frame_forced) {
    RTC_LOG(LS_INFO) << "Stop forcing key frames in the H264 encoder.";
    ComPtr<IMFSinkWriterEncoderConfig> encoderConfig;
    sinkWriter_.As(&encoderConfig);
    ComPtr<IMFAttributes> encoderAttributes;
    MFCreateAttributes(&encoderAttributes, 1);
    encoderAttributes->SetUINT32(CODECAPI_AVEncVideoForceKeyFrame, FALSE);
    encoderConfig->PlaceEncodingParameters(streamIndex_,
                                           encoderAttributes.Get());
  }

  return WEBRTC_VIDEO_CODEC_OK;
}

VideoEncoder::EncoderInfo H264EncoderMFImpl::GetEncoderInfo() const {
  EncoderInfo info;
  info.supports_native_handle = false;
  info.implementation_name = "WinRTC_MF_H264";
  info.scaling_settings =
      VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
  info.is_hardware_accelerated = true;
  info.has_internal_source = false;
  info.supports_simulcast = false;
  return info;
}

void H264EncoderMFImpl::OnH264Encoded(ComPtr<IMFSample> sample) {
  DWORD totalLength;
  HRESULT hr = S_OK;
  ON_SUCCEEDED(sample->GetTotalLength(&totalLength));

  ComPtr<IMFMediaBuffer> buffer;
  hr = sample->GetBufferByIndex(0, &buffer);

  if (SUCCEEDED(hr)) {
    BYTE* byteBuffer;
    DWORD maxLength;
    DWORD curLength;
    hr = buffer->Lock(&byteBuffer, &maxLength, &curLength);
    if (FAILED(hr)) {
      return;
    }
    if (curLength == 0) {
      RTC_LOG(LS_WARNING) << "Got empty sample.";
      buffer->Unlock();
      return;
    }
    std::vector<byte> sendBuffer;
    sendBuffer.resize(curLength);
    memcpy(sendBuffer.data(), byteBuffer, curLength);
    hr = buffer->Unlock();
    if (FAILED(hr)) {
      return;
    }

    // sendBuffer is not copied here.
    EncodedImage encodedImage(sendBuffer.data(), curLength, curLength);

    ComPtr<IMFAttributes> sampleAttributes;
    hr = sample.As(&sampleAttributes);
    if (SUCCEEDED(hr)) {
      UINT32 cleanPoint;
      hr = sampleAttributes->GetUINT32(MFSampleExtension_CleanPoint,
                                       &cleanPoint);
      if (SUCCEEDED(hr) && cleanPoint) {
        encodedImage._completeFrame = true;
        encodedImage._frameType = VideoFrameType::kVideoFrameKey;
      }
    }

    // Scan for and create mark all fragments.
    RTPFragmentationHeader fragmentationHeader;
    uint32_t fragIdx = 0;
    for (uint32_t i = 0; i < sendBuffer.size() - 5; ++i) {
      byte* ptr = sendBuffer.data() + i;
      int prefixLengthFound = 0;
      if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x00 &&
          ptr[3] == 0x01 &&
          ((ptr[4] & 0x1f) != 0x09 /* ignore access unit delimiters */)) {
        prefixLengthFound = 4;
      } else if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01 &&
                 ((ptr[3] & 0x1f) !=
                  0x09 /* ignore access unit delimiters */)) {
        prefixLengthFound = 3;
      }

      // Found a key frame, mark is as such in case
      // MFSampleExtension_CleanPoint wasn't set on the sample.
      if (prefixLengthFound > 0 && (ptr[prefixLengthFound] & 0x1f) == 0x05) {
        encodedImage._completeFrame = true;
        encodedImage._frameType = VideoFrameType::kVideoFrameKey;
      }

      if (prefixLengthFound > 0) {
        fragmentationHeader.VerifyAndAllocateFragmentationHeader(fragIdx + 1);
        fragmentationHeader.fragmentationOffset[fragIdx] =
            i + prefixLengthFound;
        fragmentationHeader.fragmentationLength[fragIdx] =
            0;  // We'll set that later
        // Set the length of the previous fragment.
        if (fragIdx > 0) {
          fragmentationHeader.fragmentationLength[fragIdx - 1] =
              i - fragmentationHeader.fragmentationOffset[fragIdx - 1];
        }
        ++fragIdx;
        i += 5;
      }
    }
    // Set the length of the last fragment.
    if (fragIdx > 0) {
      fragmentationHeader.fragmentationLength[fragIdx - 1] =
          sendBuffer.size() -
          fragmentationHeader.fragmentationOffset[fragIdx - 1];
    }

    {
      rtc::CritScope lock(&callbackCrit_);
      --framePendingCount_;
      if (encodedCompleteCallback_ == nullptr) {
        return;
      }

      LONGLONG sampleTimestamp;
      sample->GetSampleTime(&sampleTimestamp);

      CachedFrameAttributes frameAttributes;
      if (_sampleAttributeQueue.pop(sampleTimestamp, frameAttributes)) {
        encodedImage.SetTimestamp(frameAttributes.timestamp);
        encodedImage.ntp_time_ms_ = frameAttributes.ntpTime;
        encodedImage.capture_time_ms_ = frameAttributes.captureRenderTime;
        encodedImage._encodedWidth = frameAttributes.frameWidth;
        encodedImage._encodedHeight = frameAttributes.frameHeight;
      } else {
        // No point in confusing the callback with a frame that doesn't
        // have correct attributes.
        return;
      }

      if (encodedCompleteCallback_ != nullptr) {
        CodecSpecificInfo codecSpecificInfo;
        codecSpecificInfo.codecType = webrtc::kVideoCodecH264;
        codecSpecificInfo.codecSpecific.H264.packetization_mode =
            H264PacketizationMode::NonInterleaved;
        encodedCompleteCallback_->OnEncodedImage(
            encodedImage, &codecSpecificInfo, &fragmentationHeader);
      }
    }
  }
}

ComPtr<IMFSample> H264EncoderMFImpl::FromVideoFrame(const VideoFrame& frame) {
  HRESULT hr = S_OK;
  ComPtr<IMFSample> sample;
  ON_SUCCEEDED(MFCreateSample(sample.ReleaseAndGetAddressOf()));

  ComPtr<IMFAttributes> sampleAttributes;
  ON_SUCCEEDED(sample.As(&sampleAttributes));

  rtc::scoped_refptr<I420BufferInterface> frameBuffer =
      static_cast<I420BufferInterface*>(frame.video_frame_buffer().get());

  if (SUCCEEDED(hr)) {
    auto totalSize = frameBuffer->StrideY() * frameBuffer->height() +
                     frameBuffer->StrideU() * (frameBuffer->height() + 1) / 2 +
                     frameBuffer->StrideV() * (frameBuffer->height() + 1) / 2;

    ComPtr<IMFMediaBuffer> mediaBuffer;
    ON_SUCCEEDED(MFCreateMemoryBuffer(totalSize, mediaBuffer.ReleaseAndGetAddressOf()));

    BYTE* destBuffer = nullptr;
    if (SUCCEEDED(hr)) {
      DWORD cbMaxLength;
      DWORD cbCurrentLength;
      ON_SUCCEEDED(
          mediaBuffer->Lock(&destBuffer, &cbMaxLength, &cbCurrentLength));
    }

    if (SUCCEEDED(hr)) {
      BYTE* destUV =
          destBuffer + (frameBuffer->StrideY() * frameBuffer->height());
      libyuv::I420ToNV12(
          frameBuffer->DataY(), frameBuffer->StrideY(), frameBuffer->DataU(),
          frameBuffer->StrideU(), frameBuffer->DataV(), frameBuffer->StrideV(),
          destBuffer, frameBuffer->StrideY(), destUV, frameBuffer->StrideY(),
          frameBuffer->width(), frameBuffer->height());
    }

    {
      if (frameBuffer->width() != (int)width_ ||
          frameBuffer->height() != (int)height_) {
        EncodedImageCallback* tempCallback = encodedCompleteCallback_;
        Release();
        {
          rtc::CritScope lock(&callbackCrit_);
          encodedCompleteCallback_ = tempCallback;
        }

        width_ = frameBuffer->width();
        height_ = frameBuffer->height();
        InitEncoderWithSettings(&codec_);
        RTC_LOG(LS_WARNING) << "Resolution changed to: " << frameBuffer->width()
                            << "x" << frameBuffer->height();
      }
    }

    if (firstFrame_) {
      firstFrame_ = false;
      startTime_ = frame.timestamp();
    }

    auto timestampHns = ((frame.timestamp() - startTime_) / 90) * 1000 * 10;
    ON_SUCCEEDED(sample->SetSampleTime(timestampHns));

    if (SUCCEEDED(hr)) {
      auto durationHns = timestampHns - lastTimestampHns_;
      hr = sample->SetSampleDuration(durationHns);
    }

    if (SUCCEEDED(hr)) {
      lastTimestampHns_ = timestampHns;

      // Cache the frame attributes to get them back after the encoding.
      CachedFrameAttributes frameAttributes;
      frameAttributes.timestamp = frame.timestamp();
      frameAttributes.ntpTime = frame.ntp_time_ms();
      frameAttributes.captureRenderTime = frame.render_time_ms();
      frameAttributes.frameWidth = frame.width();
      frameAttributes.frameHeight = frame.height();
      _sampleAttributeQueue.push(timestampHns, frameAttributes);
    }

    ON_SUCCEEDED(mediaBuffer->SetCurrentLength(frameBuffer->width() *
                                               frameBuffer->height() * 3 / 2));

    if (destBuffer != nullptr) {
      mediaBuffer->Unlock();
    }

    ON_SUCCEEDED(sample->AddBuffer(mediaBuffer.Get()));

    if (lastFrameDropped_) {
      lastFrameDropped_ = false;
      sampleAttributes->SetUINT32(MFSampleExtension_Discontinuity, TRUE);
    }
  }

  return sample;
}

}  // namespace webrtc
