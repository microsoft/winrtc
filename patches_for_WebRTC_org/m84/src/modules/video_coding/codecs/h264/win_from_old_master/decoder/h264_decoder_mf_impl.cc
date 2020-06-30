/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_coding/codecs/h264/win/decoder/h264_decoder_mf_impl.h"

#include <Windows.h>
#include <codecapi.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <ppltasks.h>
#include <robuffer.h>
#include <wrl.h>
#include <wrl\implements.h>
#include <iomanip>
#include "common_video/include/video_frame_buffer.h"
#include "libyuv/convert.h"
#include "modules/video_coding/codecs/h264/win/utils/utils.h"
#include "modules/video_coding/include/video_codec_interface.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"

using ::Microsoft::WRL::ComPtr;

namespace webrtc {

//////////////////////////////////////////
// H264 WinUWP Decoder Implementation
//////////////////////////////////////////

H264DecoderMFImpl::H264DecoderMFImpl()
    : buffer_pool_(false, 300), /* max_number_of_buffers*/
      width_(absl::nullopt),
      height_(absl::nullopt),
      decode_complete_callback_(nullptr) {}

H264DecoderMFImpl::~H264DecoderMFImpl() {
  OutputDebugString(L"H264DecoderMFImpl::~H264DecoderMFImpl()\n");
  Release();
}

HRESULT ConfigureOutputMediaType(ComPtr<IMFTransform> decoder,
                                 GUID media_type,
                                 bool* type_found) {
  HRESULT hr = S_OK;
  *type_found = false;

  int type = 0;
  while (true) {
    ComPtr<IMFMediaType> output_media;
    ON_SUCCEEDED(decoder->GetOutputAvailableType(0, type, &output_media));
    if (hr == MF_E_NO_MORE_TYPES)
      return S_OK;

    GUID cur_type;
    ON_SUCCEEDED(output_media->GetGUID(MF_MT_SUBTYPE, &cur_type));
    if (FAILED(hr))
      return hr;

    if (cur_type == media_type) {
      hr = decoder->SetOutputType(0, output_media.Get(), 0);
      ON_SUCCEEDED(*type_found = true);
      return hr;
    }

    type++;
  }
}

HRESULT CreateInputMediaType(IMFMediaType** pp_input_media,
                             absl::optional<UINT32> img_width,
                             absl::optional<UINT32> img_height,
                             absl::optional<UINT32> frame_rate) {
  HRESULT hr = MFCreateMediaType(pp_input_media);

  IMFMediaType* input_media = *pp_input_media;
  ON_SUCCEEDED(input_media->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  ON_SUCCEEDED(input_media->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
  ON_SUCCEEDED(
      MFSetAttributeRatio(input_media, MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
  ON_SUCCEEDED(input_media->SetUINT32(
      MF_MT_INTERLACE_MODE, MFVideoInterlace_MixedInterlaceOrProgressive));

  if (frame_rate.has_value()) {
    ON_SUCCEEDED(MFSetAttributeRatio(input_media, MF_MT_FRAME_RATE,
                                     frame_rate.value(), 1));
  }

  if (img_width.has_value() && img_height.has_value()) {
    ON_SUCCEEDED(MFSetAttributeSize(input_media, MF_MT_FRAME_SIZE,
                                    img_width.value(), img_height.value()));
  }

  return hr;
}

int H264DecoderMFImpl::InitDecode(const VideoCodec* codec_settings,
                                  int number_of_cores) {
  RTC_LOG(LS_INFO) << "H264DecoderMFImpl::InitDecode()\n";

  width_ = codec_settings->width > 0
               ? absl::optional<UINT32>(codec_settings->width)
               : absl::nullopt;
  height_ = codec_settings->height > 0
                ? absl::optional<UINT32>(codec_settings->height)
                : absl::nullopt;

  HRESULT hr = S_OK;
  //ON_SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED));
  ON_SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET));

  ON_SUCCEEDED(CoCreateInstance(CLSID_MSH264DecoderMFT, nullptr,
                                CLSCTX_INPROC_SERVER, IID_IUnknown,
                                (void**)&decoder_));

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Init failure: could not create Media Foundation H264 "
                         "decoder instance.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Try set decoder attributes
  ComPtr<IMFAttributes> decoder_attrs;
  ON_SUCCEEDED(decoder_->GetAttributes(decoder_attrs.GetAddressOf()));

  if (SUCCEEDED(hr)) {
    ON_SUCCEEDED(decoder_attrs->SetUINT32(CODECAPI_AVLowLatencyMode, TRUE));
    if (FAILED(hr)) {
      RTC_LOG(LS_WARNING)
          << "Init warning: failed to set low latency in H264 decoder.";
      hr = S_OK;
    }

    ON_SUCCEEDED(
        decoder_attrs->SetUINT32(CODECAPI_AVDecVideoAcceleration_H264, TRUE));
    if (FAILED(hr)) {
      RTC_LOG(LS_WARNING)
          << "Init warning: failed to set HW accel in H264 decoder.";
    }
  }

  // Clear any error from try set attributes
  hr = S_OK;

  ComPtr<IMFMediaType> input_media;
  ON_SUCCEEDED(CreateInputMediaType(
      input_media.GetAddressOf(), width_, height_,
      codec_settings->maxFramerate > 0
          ? absl::optional<UINT32>(codec_settings->maxFramerate)
          : absl::nullopt));

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Init failure: could not create input media type.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Register the input type with the decoder
  ON_SUCCEEDED(decoder_->SetInputType(0, input_media.Get(), 0));

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR)
        << "Init failure: failed to set input media type on decoder.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Assert MF supports NV12 output
  bool suitable_type_found;
  ON_SUCCEEDED(ConfigureOutputMediaType(decoder_, MFVideoFormat_NV12,
                                        &suitable_type_found));

  if (FAILED(hr) || !suitable_type_found) {
    RTC_LOG(LS_ERROR) << "Init failure: failed to find a valid output media "
                         "type for decoding.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  DWORD status;
  ON_SUCCEEDED(decoder_->GetInputStatus(0, &status));

  // Validate that decoder is up and running
  if (SUCCEEDED(hr)) {
    if (MFT_INPUT_STATUS_ACCEPT_DATA != status)
      // H.264 decoder MFT is not accepting data
      return WEBRTC_VIDEO_CODEC_ERROR;
  }

  ON_SUCCEEDED(decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL));
  ON_SUCCEEDED(
      decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL));
  ON_SUCCEEDED(
      decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL));

  inited_ = true;
  return SUCCEEDED(hr) ? WEBRTC_VIDEO_CODEC_OK : WEBRTC_VIDEO_CODEC_ERROR;
}

/**
 * Workaround [MF H264 bug: Output status is never set, even when ready]
 *  => For now, always mark "ready" (results in extra buffer alloc/dealloc).
 */
HRESULT GetOutputStatus(ComPtr<IMFTransform> decoder, DWORD* output_status) {
  HRESULT hr = decoder->GetOutputStatus(output_status);

  // Don't MFT trust output status for now.
  *output_status = MFT_OUTPUT_STATUS_SAMPLE_READY;
  return hr;
}

/**
 * Note: expected to return MF_E_TRANSFORM_NEED_MORE_INPUT and
 *       MF_E_TRANSFORM_STREAM_CHANGE which must be handled by caller.
 */
HRESULT H264DecoderMFImpl::FlushFrames(uint32_t rtp_timestamp,
                                       uint64_t ntp_time_ms) {
  HRESULT hr;
  DWORD output_status;

  while (SUCCEEDED(hr = GetOutputStatus(decoder_, &output_status)) &&
         output_status == MFT_OUTPUT_STATUS_SAMPLE_READY) {
    // Get needed size of our output buffer
    MFT_OUTPUT_STREAM_INFO strm_info;
    ON_SUCCEEDED(decoder_->GetOutputStreamInfo(0, &strm_info));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR) << "Decode failure: failed to get output stream info.";
      return hr;
    }

    // Create output sample
    ComPtr<IMFMediaBuffer> out_buffer;
    ON_SUCCEEDED(MFCreateMemoryBuffer(strm_info.cbSize, &out_buffer));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR)
          << "Decode failure: output image memory buffer creation failed.";
      return hr;
    }

    ComPtr<IMFSample> out_sample;
    ON_SUCCEEDED(MFCreateSample(&out_sample));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR) << "Decode failure: output in_sample creation failed.";
      return hr;
    }

    ON_SUCCEEDED(out_sample->AddBuffer(out_buffer.Get()));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR)
          << "Decode failure: failed to add buffer to output in_sample.";
      return hr;
    }

    // Create output buffer description
    MFT_OUTPUT_DATA_BUFFER output_data_buffer;
    output_data_buffer.dwStatus = 0;
    output_data_buffer.dwStreamID = 0;
    output_data_buffer.pEvents = nullptr;
    output_data_buffer.pSample = out_sample.Get();

    // Invoke the Media Foundation decoder
    // Note: we don't use ON_SUCCEEDED here since ProcessOutput returns
    //       MF_E_TRANSFORM_NEED_MORE_INPUT often (too many log messages).
    DWORD status;
    hr = decoder_->ProcessOutput(0, 1, &output_data_buffer, &status);

    if (FAILED(hr))
      return hr; /* can return MF_E_TRANSFORM_NEED_MORE_INPUT or
                    MF_E_TRANSFORM_STREAM_CHANGE (entirely acceptable) */

    // Copy raw output sample data to video frame buffer.
    ComPtr<IMFMediaBuffer> src_buffer;
    ON_SUCCEEDED(out_sample->ConvertToContiguousBuffer(&src_buffer));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR) << "Decode failure: failed to get contiguous buffer.";
      return hr;
    }

    uint32_t width, height;
    if (width_.has_value() && height_.has_value()) {
      width = width_.value();
      height = height_.value();
    } else {
      // Query the size from MF output media type
      ComPtr<IMFMediaType> output_type;
      ON_SUCCEEDED(
          decoder_->GetOutputCurrentType(0, output_type.GetAddressOf()));

      ON_SUCCEEDED(MFGetAttributeSize(output_type.Get(), MF_MT_FRAME_SIZE,
                                      &width, &height));
      if (FAILED(hr)) {
        RTC_LOG(LS_ERROR) << "Decode failure: could not read image dimensions "
                             "from Media Foundation, so the video frame buffer "
                             "size can not be determined.";
        return hr;
      }

      // Update members to avoid querying unnecessarily
      width_.emplace(width);
      height_.emplace(height);
    }

    rtc::scoped_refptr<I420Buffer> buffer =
        buffer_pool_.CreateBuffer(width, height);

    if (!buffer.get()) {
      // Pool has too many pending frames.
      RTC_LOG(LS_WARNING) << "Decode warning: too many frames. Dropping frame.";
      return WEBRTC_VIDEO_CODEC_NO_OUTPUT;
    }

    DWORD cur_length;
    ON_SUCCEEDED(src_buffer->GetCurrentLength(&cur_length));
    if (FAILED(hr)) {
      RTC_LOG(LS_ERROR) << "Decode failure: could not get buffer length.";
      return hr;
    }

    if (cur_length > 0) {
      BYTE* src_data;
      DWORD max_len, cur_len;
      ON_SUCCEEDED(src_buffer->Lock(&src_data, &max_len, &cur_len));
      if (FAILED(hr)) {
        RTC_LOG(LS_ERROR) << "Decode failure: could lock buffer for copying.";
        return hr;
      }

      // Convert NV12 to I420. Y and UV sections have same stride in NV12
      // (width). The size of the Y section is the size of the frame, since Y
      // luminance values are 8-bits each.
      libyuv::NV12ToI420(src_data, width, src_data + (width * height), width,
                         buffer->MutableDataY(), buffer->StrideY(),
                         buffer->MutableDataU(), buffer->StrideU(),
                         buffer->MutableDataV(), buffer->StrideV(), width,
                         height);

      ON_SUCCEEDED(src_buffer->Unlock());
      if (FAILED(hr))
        return hr;
    }

    // LONGLONG sample_time; /* unused */
    // ON_SUCCEEDED(spOutSample->GetSampleTime(&sample_time));

    // TODO: Ideally, we should convert sample_time (above) back to 90khz + base
    // and use it in place of rtp_timestamp, since MF may interpolate it.
    // Instead, we ignore the MFT sample time out, using rtp from in frame that
    // triggered this decoded frame.
    VideoFrame decoded_frame(buffer, rtp_timestamp, 0, kVideoRotation_0);

    // Use ntp time from the earliest frame
    decoded_frame.set_ntp_time_ms(ntp_time_ms);

    // Emit image to downstream
    if (decode_complete_callback_ != nullptr) {
      decode_complete_callback_->Decoded(decoded_frame, absl::nullopt,
                                         absl::nullopt);
    }
  }

  return hr;
}

/**
 * Note: acceptable to return MF_E_NOTACCEPTING (though it shouldn't since
 * last loop should've flushed)
 */
HRESULT H264DecoderMFImpl::EnqueueFrame(const EncodedImage& input_image,
                                        bool missing_frames) {
  HRESULT hr = S_OK;

  // Create a MF buffer from our data
  ComPtr<IMFMediaBuffer> in_buffer;
  ON_SUCCEEDED(MFCreateMemoryBuffer(input_image.size(), &in_buffer));
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR)
        << "Decode failure: input image memory buffer creation failed.";
    return hr;
  }

  DWORD max_len, cur_len;
  BYTE* data;
  ON_SUCCEEDED(in_buffer->Lock(&data, &max_len, &cur_len));
  if (FAILED(hr))
    return hr;

  memcpy(data, input_image.data(), input_image.size());

  ON_SUCCEEDED(in_buffer->Unlock());
  if (FAILED(hr))
    return hr;

  ON_SUCCEEDED(in_buffer->SetCurrentLength(input_image.size()));
  if (FAILED(hr))
    return hr;

  // Create a sample from media buffer
  ComPtr<IMFSample> in_sample;
  ON_SUCCEEDED(MFCreateSample(&in_sample));
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Decode failure: input in_sample creation failed.";
    return hr;
  }

  ON_SUCCEEDED(in_sample->AddBuffer(in_buffer.Get()));
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR)
        << "Decode failure: failed to add buffer to input in_sample.";
    return hr;
  }

  int64_t sample_time_ms;
  if (first_frame_rtp_ == 0) {
    first_frame_rtp_ = input_image.Timestamp();
    sample_time_ms = 0;
  } else {
    // Convert from 90 khz, rounding to nearest ms.
    sample_time_ms =
        (static_cast<uint64_t>(input_image.Timestamp()) - first_frame_rtp_) /
            90.0 +
        0.5f;
  }

  ON_SUCCEEDED(in_sample->SetSampleTime(
      sample_time_ms *
      10000 /* convert milliseconds to 100-nanosecond unit */));
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR)
        << "Decode failure: failed to set in_sample time on input in_sample.";
    return hr;
  }

  // Set sample attributes
  ComPtr<IMFAttributes> sample_attrs;
  ON_SUCCEEDED(in_sample.As(&sample_attrs));

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR)
        << "Decode warning: failed to set image attributes for frame.";
    hr = S_OK;
  } else {
    if (input_image._frameType == VideoFrameType::kVideoFrameKey &&
        input_image._completeFrame) {
      ON_SUCCEEDED(sample_attrs->SetUINT32(MFSampleExtension_CleanPoint, TRUE));
      hr = S_OK;
    }

    if (missing_frames) {
      ON_SUCCEEDED(
          sample_attrs->SetUINT32(MFSampleExtension_Discontinuity, TRUE));
      hr = S_OK;
    }
  }

  // Enqueue sample with Media Foundation
  ON_SUCCEEDED(decoder_->ProcessInput(0, in_sample.Get(), 0));
  return hr;
}

int H264DecoderMFImpl::Decode(const EncodedImage& input_image,
                              bool missing_frames,
                              int64_t render_time_ms) {
  HRESULT hr = S_OK;

  if (!inited_) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  if (decode_complete_callback_ == NULL) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  if (input_image.data() == NULL && input_image.size() > 0) {
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  // Discard until keyframe.
  if (require_keyframe_) {
    if (input_image._frameType != VideoFrameType::kVideoFrameKey ||
        !input_image._completeFrame) {
      return WEBRTC_VIDEO_CODEC_ERROR;
    } else {
      require_keyframe_ = false;
    }
  }

  // Enqueue the new frame with Media Foundation
  ON_SUCCEEDED(EnqueueFrame(input_image, missing_frames));
  if (hr == MF_E_NOTACCEPTING) {
    // For robustness (shouldn't happen). Flush any old MF data blocking the
    // new frames.
    hr = decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);

    if (input_image._frameType == VideoFrameType::kVideoFrameKey) {
      ON_SUCCEEDED(EnqueueFrame(input_image, missing_frames));
    } else {
      require_keyframe_ = true;
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
  }

  if (FAILED(hr))
    return WEBRTC_VIDEO_CODEC_ERROR;

  // Flush any decoded samples resulting from new frame, invoking callback
  hr = FlushFrames(input_image.Timestamp(), input_image.ntp_time_ms_);

  if (hr == MF_E_TRANSFORM_STREAM_CHANGE) {
    // Output media type is no longer suitable. Reconfigure and retry.
    bool suitable_type_found;
    hr = ConfigureOutputMediaType(decoder_, MFVideoFormat_NV12,
                                  &suitable_type_found);

    if (FAILED(hr) || !suitable_type_found)
      return WEBRTC_VIDEO_CODEC_ERROR;

    // Reset width and height in case output media size has changed (though it
    // seems that would be unexpected, given that the input media would need to
    // be manually changed too).
    width_.reset();
    height_.reset();

    hr = FlushFrames(input_image.Timestamp(), input_image.ntp_time_ms_);
  }

  if (SUCCEEDED(hr) || hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    return WEBRTC_VIDEO_CODEC_OK;
  }

  return WEBRTC_VIDEO_CODEC_ERROR;
}

int H264DecoderMFImpl::RegisterDecodeCompleteCallback(
    DecodedImageCallback* callback) {
  rtc::CritScope lock(&crit_);
  decode_complete_callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int H264DecoderMFImpl::Release() {
  OutputDebugString(L"H264DecoderMFImpl::Release()\n");
  HRESULT hr = S_OK;
  inited_ = false;

  // Release I420 frame buffer pool
  buffer_pool_.Release();

  if (decoder_ != NULL) {
    // Follow shutdown procedure gracefully. On fail, continue anyway.
    ON_SUCCEEDED(decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0));
    ON_SUCCEEDED(decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL));
    ON_SUCCEEDED(decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL));
    decoder_ = nullptr;
  }

  ON_SUCCEEDED(MFShutdown());
  //CoUninitialize();

  return WEBRTC_VIDEO_CODEC_OK;
}

const char* H264DecoderMFImpl::ImplementationName() const {
  return "WinRTC_MF_H264";
}

}  // namespace webrtc
