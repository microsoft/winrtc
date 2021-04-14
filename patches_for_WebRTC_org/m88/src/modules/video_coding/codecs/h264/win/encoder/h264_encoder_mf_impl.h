/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_WIN_ENCODER_H264_ENCODER_MF_IMPL_H_
#define MODULES_VIDEO_CODING_CODECS_H264_WIN_ENCODER_H264_ENCODER_MF_IMPL_H_

#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <vector>
#include "../utils/sample_attribute_queue.h"
#include "H264_media_sink.h"
#include "api/video_codecs/video_encoder.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "ih264_encoding_callback.h"
#include "modules/video_coding/utility/quality_scaler.h"
#include "rtc_base/critical_section.h"

namespace webrtc {

class H264MediaSink;

class H264EncoderMFImpl : public VideoEncoder, public IH264EncodingCallback {
 public:
  explicit H264EncoderMFImpl();

  ~H264EncoderMFImpl();

  // === VideoEncoder overrides ===
  int32_t H264EncoderMFImpl::InitEncode(
      const VideoCodec* codec_settings,
      const VideoEncoder::Settings& settings) override;
  int Release() override;

  int RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;

  // The result of encoding - an EncodedImage and RTPFragmentationHeader - are
  // passed to the encode complete callback.
  int32_t Encode(const VideoFrame& frame,
                 const std::vector<VideoFrameType>* frame_types) override;

  VideoEncoder::EncoderInfo GetEncoderInfo() const override;

  // === IH264EncodingCallback overrides ===
  void OnH264Encoded(Microsoft::WRL::ComPtr<IMFSample> sample) override;

 private:
  Microsoft::WRL::ComPtr<IMFSample> FromVideoFrame(const VideoFrame& frame);
  int InitWriter();
  int ReleaseWriter();
  LONGLONG GetFrameTimestampHns(const VideoFrame& frame) const;
  int ReconfigureSinkWriter(UINT32 new_width,
                            UINT32 new_height,
                            UINT32 new_target_bps,
                            UINT32 new_frame_rate);

 private:
  rtc::CriticalSection crit_;
  rtc::CriticalSection callbackCrit_;
  bool inited_{};
  Microsoft::WRL::ComPtr<IMFSinkWriter> sinkWriter_;
  Microsoft::WRL::ComPtr<H264MediaSink> mediaSink_;
  EncodedImageCallback* encodedCompleteCallback_{};
  DWORD streamIndex_{};
  LONGLONG startTime_{};
  LONGLONG lastTimestampHns_{};
  bool firstFrame_{true};
  int framePendingCount_{};
  DWORD frameCount_{};
  bool lastFrameDropped_{};
  // These fields are never used
  /*
  UINT32 currentWidth_ {};
  UINT32 currentHeight_ {};
  UINT32 currentBitrateBps_ {};
  UINT32 currentFps_ {};
  */
  UINT32 max_bitrate_;

  UINT32 width_;
  UINT32 height_;
  UINT32 frame_rate_;
  UINT32 target_bps_;
  UINT32 max_qp_;
  VideoCodecMode mode_;
  // H.264 specifc parameters
  bool frame_dropping_on_;
  int key_frame_interval_;

  int64_t last_rate_change_time_rtc_ms{};
  bool rate_change_requested_{};

  // Values to use as soon as the min interval between rate changes has passed
  UINT32 next_frame_rate_;
  UINT32 next_target_bps_;

  struct CachedFrameAttributes {
    uint32_t timestamp;
    uint64_t ntpTime;
    uint64_t captureRenderTime;
    uint32_t frameWidth;
    uint32_t frameHeight;
  };
  SampleAttributeQueue<CachedFrameAttributes> _sampleAttributeQueue;

};  // end of WinUWPH264EncoderImpl class

}  // namespace webrtc
#endif  // MODULES_VIDEO_CODING_CODECS_H264_WIN_ENCODER_H264_ENCODER_MF_IMPL_H_
