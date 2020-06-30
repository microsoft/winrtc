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

#include "api/video_codecs/sdp_video_format.h"
#include "common_types.h"
#include "media/base/h264_profile_level_id.h"
#include "modules/video_coding/codecs/h264/win/decoder/h264_decoder_mf_impl.h"
#include "modules/video_coding/codecs/h264/win/encoder/h264_encoder_mf_impl.h"

namespace webrtc {

//
// H264MFEncoderFactory
//

SdpVideoFormat CreateH264Format(H264::Profile profile,
                                H264::Level level,
                                const std::string& packetization_mode) {
  const absl::optional<std::string> profile_string =
      H264::ProfileLevelIdToString(H264::ProfileLevelId(profile, level));
  RTC_CHECK(profile_string);
  return SdpVideoFormat(
      cricket::kH264CodecName,
      {{cricket::kH264FmtpProfileLevelId, *profile_string},
       {cricket::kH264FmtpLevelAsymmetryAllowed, "1"},
       {cricket::kH264FmtpPacketizationMode, packetization_mode}});
}

std::vector<SdpVideoFormat> SupportedFormats() {
  return {
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "1"),
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "0"),
      CreateH264Format(H264::kProfileConstrainedBaseline, H264::kLevel3_1, "1"),
      CreateH264Format(H264::kProfileConstrainedBaseline, H264::kLevel3_1,
                       "0")};
}

std::vector<SdpVideoFormat> H264MFEncoderFactory::GetSupportedFormats() const {
  return SupportedFormats();
}

VideoEncoderFactory::CodecInfo H264MFEncoderFactory::QueryVideoEncoder(
    const SdpVideoFormat& format) const {
  VideoEncoderFactory::CodecInfo codec_info;

  codec_info.has_internal_source = false;
  codec_info.is_hardware_accelerated = true;

  return codec_info;
}

std::unique_ptr<VideoEncoder> H264MFEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  cricket::VideoCodec codec{std::string(cricket::kH264CodecName)};
  return std::make_unique<H264EncoderMFImpl>(codec);
}

//
// H264MFDecoderFactory
//
std::vector<SdpVideoFormat> H264MFDecoderFactory::GetSupportedFormats() const {
  return SupportedFormats();
}

std::unique_ptr<VideoDecoder> H264MFDecoderFactory::CreateVideoDecoder(
    const SdpVideoFormat& /*format*/) {
  return std::make_unique<H264DecoderMFImpl>();
}

}  // namespace webrtc
