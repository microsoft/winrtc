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

#ifndef MODULES_VIDEO_CODING_CODECS_H264_H264_ENCODER_MF_IMPL_H_
#define MODULES_VIDEO_CODING_CODECS_H264_H264_ENCODER_MF_IMPL_H_

#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl.h>
#include <vector>
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/codecs/h264/win/encoder/h264_media_sink.h"
#include "modules/video_coding/codecs/h264/win/encoder/ih264_encoding_callback.h"
#include "modules/video_coding/codecs/h264/win/utils/sample_attribute_queue.h"

namespace webrtc {

class H264EncoderMFImpl : public VideoEncoder, IH264EncodingCallback {
 public:
 public:
  explicit H264EncoderMFImpl(const cricket::VideoCodec& codec);
  ~H264EncoderMFImpl() override;

  int32_t InitEncode(const VideoCodec* codec_settings,
                     const VideoEncoder::Settings& settings) override;
  int32_t Release() override;
  //
  int32_t RegisterEncodeCompleteCallback(
      EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;

  // The result of encoding - an EncodedImage and RTPFragmentationHeader - are
  // passed to the encode complete callback.
  int32_t Encode(const VideoFrame& frame,
                 const std::vector<VideoFrameType>* frame_types) override;

  EncoderInfo GetEncoderInfo() const override;

  // === IH264EncodingCallback overrides ===
  void OnH264Encoded(::Microsoft::WRL::ComPtr<IMFSample> sample) override;
  //

 private:
  VideoCodec codec_;

  int InitEncoderWithSettings(const VideoCodec* codec_settings);
  ::Microsoft::WRL::ComPtr<IMFSample> FromVideoFrame(const VideoFrame& frame);

  rtc::CriticalSection crit_;

  bool inited_{};

  uint32_t currentWidth_;
  uint32_t currentHeight_;
  uint32_t currentBitrateBps_;
  uint32_t currentFps_;
  UINT32 target_bps_;
  UINT32 width_;
  UINT32 height_;
  UINT32 max_frame_rate_;
  int64_t lastTimeSettingsChanged_{};
  ::Microsoft::WRL::ComPtr<H264MediaSink> mediaSink_;
  ::Microsoft::WRL::ComPtr<IMFAttributes> sinkWriterCreationAttributes_;
  ::Microsoft::WRL::ComPtr<IMFSinkWriter> sinkWriter_;
  DWORD streamIndex_{};
  ::Microsoft::WRL::ComPtr<IMFAttributes> sinkWriterEncoderAttributes_;
  rtc::CriticalSection callbackCrit_;
  EncodedImageCallback* encodedCompleteCallback_{};
  int framePendingCount_;
  LONGLONG startTime_;
  LONGLONG lastTimestampHns_;
  bool firstFrame_;
  bool lastFrameDropped_;

  struct CachedFrameAttributes {
    uint32_t timestamp;
    uint64_t ntpTime;
    uint64_t captureRenderTime;
    uint32_t frameWidth;
    uint32_t frameHeight;
  };
  SampleAttributeQueue<CachedFrameAttributes> _sampleAttributeQueue;
  DWORD frameCount_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_H264_H264_ENCODER_IMPL_MF_H_
