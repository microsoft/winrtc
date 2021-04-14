/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_coding/codecs/h264/win/h264_mf_factory.h"

#include <vector>

#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/sdp_video_format.h"
#include "common_types.h"
#include "media/base/h264_profile_level_id.h"
#include "modules/video_coding/codecs/h264/win/decoder/h264_decoder_mf_impl.h"
#include "modules/video_coding/codecs/h264/win/encoder/h264_encoder_mf_impl.h"
#include "third_party/abseil-cpp/absl/strings/match.h"

using std::make_unique;
using std::unique_ptr;
using std::vector;

namespace webrtc {

//
// H264MFEncoderFactory
//
H264MFEncoderFactory::H264MFEncoderFactory()
    : builtin_video_encoder_factory_(
          webrtc::CreateBuiltinVideoEncoderFactory()) {}

void AddSupportedH264Codecs(vector<SdpVideoFormat>& formats) {
  formats.emplace_back(
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "1"));
  formats.emplace_back(
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "0"));
  formats.emplace_back(CreateH264Format(H264::kProfileConstrainedBaseline,
                                        H264::kLevel3_1, "1"));
  formats.emplace_back(CreateH264Format(H264::kProfileConstrainedBaseline,
                                        H264::kLevel3_1, "0"));
}

vector<SdpVideoFormat> H264MFEncoderFactory::GetSupportedFormats() const {
  auto formats = builtin_video_encoder_factory_->GetSupportedFormats();
  AddSupportedH264Codecs(formats);

  return formats;
}

VideoEncoderFactory::CodecInfo H264MFEncoderFactory::QueryVideoEncoder(
    const SdpVideoFormat& format) const {
  VideoEncoderFactory::CodecInfo codec_info;

  codec_info.has_internal_source = false;
  codec_info.is_hardware_accelerated = true;

  return codec_info;
}

unique_ptr<VideoEncoder> H264MFEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  if (absl::EqualsIgnoreCase(format.name.c_str(), cricket::kH264CodecName)) {
    return make_unique<H264EncoderMFImpl>();
  }

  return builtin_video_encoder_factory_->CreateVideoEncoder(format);
}

//
// H264MFDecoderFactory
//
H264MFDecoderFactory::H264MFDecoderFactory()
    : builtin_video_decoder_factory_(
          webrtc::CreateBuiltinVideoDecoderFactory()) {}

vector<SdpVideoFormat> H264MFDecoderFactory::GetSupportedFormats() const {
  auto formats = builtin_video_decoder_factory_->GetSupportedFormats();
  AddSupportedH264Codecs(formats);

  return formats;
}

unique_ptr<VideoDecoder> H264MFDecoderFactory::CreateVideoDecoder(
    const SdpVideoFormat& format) {
  if (absl::EqualsIgnoreCase(format.name.c_str(), cricket::kH264CodecName)) {
    return make_unique<H264DecoderMFImpl>();
  }

  return builtin_video_decoder_factory_->CreateVideoDecoder(format);
}

}  // namespace webrtc
