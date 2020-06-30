/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_WIN_H264_MF_FACTORY_H_
#define MODULES_VIDEO_CODING_CODECS_H264_WIN_H264_MF_FACTORY_H_

#include <string>

#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"

namespace webrtc {

class H264MFEncoderFactory : public VideoEncoderFactory {
 public:
  H264MFEncoderFactory() = default;
  ~H264MFEncoderFactory() override = default;

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;

  VideoEncoderFactory::CodecInfo QueryVideoEncoder(
      const SdpVideoFormat& format) const override;

  std::unique_ptr<VideoEncoder> CreateVideoEncoder(
      const SdpVideoFormat& format) override;
};

class H264MFDecoderFactory : public VideoDecoderFactory {
 public:
  H264MFDecoderFactory() = default;
  ~H264MFDecoderFactory() override = default;

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;

  std::unique_ptr<VideoDecoder> CreateVideoDecoder(
      const SdpVideoFormat& format) override;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_H264_WIN_H264_MF_FACTORY_H_
