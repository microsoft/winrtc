/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_capture/windows/video_capture_winrt.h"

#include <Windows.Devices.Enumeration.h>
#include <Windows.Foundation.Collections.h>
#include <unknwn.h>
#include <windows.foundation.h>
#include <windows.media.capture.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include <cassert>
#include <functional>

#include "modules/video_capture/video_capture_config.h"
#include "modules/video_capture/windows/help_functions_winrt.h"
#include "rtc_base/logging.h"

struct __declspec(uuid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")) __declspec(
    novtable) IMemoryBufferByteAccess : ::IUnknown {
  virtual HRESULT __stdcall GetBuffer(uint8_t** value, uint32_t* capacity) = 0;
};

using ::ABI::Windows::Foundation::ActivateInstance;
using ::ABI::Windows::Foundation::IAsyncAction;
using ::ABI::Windows::Foundation::IAsyncOperation;
using ::ABI::Windows::Foundation::IClosable;
using ::ABI::Windows::Foundation::IMemoryBuffer;
using ::ABI::Windows::Foundation::IMemoryBufferReference;
using ::ABI::Windows::Foundation::ITypedEventHandler;
using ::ABI::Windows::Foundation::Collections::IIterable;
using ::ABI::Windows::Foundation::Collections::IIterator;
using ::ABI::Windows::Foundation::Collections::IKeyValuePair;
using ::ABI::Windows::Foundation::Collections::IMapView;
using ::ABI::Windows::Foundation::Collections::IVectorView;
using ::ABI::Windows::Graphics::Imaging::BitmapBufferAccessMode;
using ::ABI::Windows::Graphics::Imaging::BitmapPlaneDescription;
using ::ABI::Windows::Graphics::Imaging::IBitmapBuffer;
using ::ABI::Windows::Graphics::Imaging::ISoftwareBitmap;
using ::ABI::Windows::Media::Capture::IMediaCapture;
using ::ABI::Windows::Media::Capture::IMediaCapture5;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings5;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfile;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfileMediaDescription;
using ::ABI::Windows::Media::Capture::MediaCaptureMemoryPreference;
using ::ABI::Windows::Media::Capture::MediaStreamType;
using ::ABI::Windows::Media::Capture::StreamingCaptureMode;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameArrivedEventArgs;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameFormat;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameReader;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameReference;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameSource;
using ::ABI::Windows::Media::Capture::Frames::IMediaFrameSourceInfo;
using ::ABI::Windows::Media::Capture::Frames::IVideoMediaFrame;
using ::ABI::Windows::Media::Capture::Frames::IVideoMediaFrameFormat;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameFormat;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameReader;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameReaderStartStatus;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameSource;
using ::ABI::Windows::Media::Capture::Frames::MediaFrameSourceKind;
using ::ABI::Windows::Media::MediaProperties::IMediaRatio;
using ::Microsoft::WRL::Callback;
using ::Microsoft::WRL::ComPtr;
using ::Microsoft::WRL::Wrappers::HString;
using ::Microsoft::WRL::Wrappers::HStringReference;

namespace webrtc {
namespace videocapturemodule {

///////////////////////////////////////////////////////////////////////////////
//
//  VideoCaptureWinRTInternal
//
///////////////////////////////////////////////////////////////////////////////

// Callback type for the following method:
// int32_t VideoCaptureImpl::IncomingFrame(uint8_t *plane_y,
//                                         int32_t stride_y,
//                                         uint8_t *plane_uv,
//                                         int32_t stride_uv,
//                                         size_t videoFrameLength,
//                                         const VideoCaptureCapability
//                                         &frameInfo, int64_t captureTime = 0
typedef std::function<int32_t(uint8_t*,
                              int32_t,
                              uint8_t*,
                              int32_t,
                              size_t,
                              const VideoCaptureCapability&,
                              int64_t)>
    PFNIncomingFrameType;

struct VideoCaptureWinRTInternal {
 public:
  explicit VideoCaptureWinRTInternal(
      const PFNIncomingFrameType& pfn_incoming_frame);

  ~VideoCaptureWinRTInternal();

  HRESULT StartCapture(const VideoCaptureCapability& capability);
  HRESULT StopCapture();
  bool CaptureStarted();

  HRESULT FrameArrived(IMediaFrameReader* sender,
                       IMediaFrameArrivedEventArgs* args);

 private:
  ComPtr<IMediaCapture> media_capture_;

  ComPtr<IMediaFrameReader> media_frame_reader_;
  EventRegistrationToken media_source_frame_arrived_token;

  bool is_capturing = false;
  PFNIncomingFrameType pfn_incoming_frame_;
};

VideoCaptureWinRTInternal::VideoCaptureWinRTInternal(
    const PFNIncomingFrameType& pfn_incoming_frame)
    : pfn_incoming_frame_(pfn_incoming_frame) {}

VideoCaptureWinRTInternal::~VideoCaptureWinRTInternal() {
  HRESULT hr = S_OK;
  hr = StopCapture();
  assert(SUCCEEDED(hr));
}

HRESULT VideoCaptureWinRTInternal::StartCapture(
    const VideoCaptureCapability& capability) {
  HRESULT hr = S_OK;
  ComPtr<IMediaCaptureInitializationSettings> initialization_settings;
  ComPtr<IMediaFrameSource> media_frame_source;
  ComPtr<IMediaCapture5> media_capture5;
  ComPtr<IMapView<HSTRING, MediaFrameSource*>> frame_sources;
  ComPtr<IIterable<IKeyValuePair<HSTRING, MediaFrameSource*>*>> iterable;
  ComPtr<IIterator<IKeyValuePair<HSTRING, MediaFrameSource*>*>>
      frame_source_iterator;
  ComPtr<IAsyncOperation<MediaFrameReader*>> async_operation;
  ComPtr<IAsyncOperation<MediaFrameReaderStartStatus>>
      async_media_frame_reader_start_status;
  ComPtr<IMediaCaptureVideoProfile> video_profile;
  ComPtr<IMediaCaptureVideoProfileMediaDescription> profile_description;
  HStringReference device_id(capability.profile_id.c_str());
  MediaFrameReaderStartStatus media_frame_reader_start_status;
  boolean has_current;

  if (SUCCEEDED(hr) && capability.media_capture_video_profile) {
    hr = capability.media_capture_video_profile->QueryInterface(
        video_profile.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr) && capability.record_media_description) {
    hr = capability.record_media_description->QueryInterface(
        profile_description.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = CreateMediaCaptureInitializationSettings(
        device_id, video_profile.Get(), profile_description.Get(),
        initialization_settings.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = GetMediaCaptureWithInitSettings(
        initialization_settings.Get(), media_capture_.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture_.As(&media_capture5);
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture5->get_FrameSources(&frame_sources);
  }

  if (SUCCEEDED(hr)) {
    hr = frame_sources.As(&iterable);
  }

  if (SUCCEEDED(hr)) {
    hr = iterable->First(&frame_source_iterator);
  }

  if (SUCCEEDED(hr)) {
    hr = frame_source_iterator->get_HasCurrent(&has_current);
  }

  while (SUCCEEDED(hr) && has_current) {
    ComPtr<IKeyValuePair<HSTRING, MediaFrameSource*>> key_value;
    ComPtr<IMediaFrameSource> value;
    ComPtr<IMediaFrameSourceInfo> info;
    ComPtr<IVectorView<MediaFrameFormat*>> supported_formats;
    ComPtr<IVideoMediaFrameFormat> video_media_frame_format;
    ComPtr<IMediaRatio> media_ratio;
    MediaStreamType media_stream_type;
    MediaFrameSourceKind media_frame_source_kind;
    unsigned int supported_formats_count;
    UINT32 frame_width, frame_height;

    if (SUCCEEDED(hr)) {
      hr = frame_source_iterator->get_Current(&key_value);
    }

    if (SUCCEEDED(hr)) {
      hr = key_value->get_Value(&value);
    }

    // Filters out frame sources other than color video cameras.
    if (SUCCEEDED(hr)) {
      hr = value->get_Info(&info);
    }

    if (SUCCEEDED(hr)) {
      hr = info->get_MediaStreamType(&media_stream_type);
    }

    if (SUCCEEDED(hr)) {
      hr = info->get_SourceKind(&media_frame_source_kind);
    }

    if (SUCCEEDED(hr) &&
        ((media_stream_type != MediaStreamType::MediaStreamType_VideoRecord) ||
         (media_frame_source_kind !=
          MediaFrameSourceKind::MediaFrameSourceKind_Color))) {
      hr = frame_source_iterator->MoveNext(&has_current);
      continue;
    }

    if (SUCCEEDED(hr)) {
      hr = value->get_SupportedFormats(&supported_formats);
    }

    if (SUCCEEDED(hr)) {
      hr = supported_formats->get_Size(&supported_formats_count);
    }

    for (unsigned int i = 0; SUCCEEDED(hr) && (i < supported_formats_count);
         ++i) {
      ComPtr<IMediaFrameFormat> media_frame_format;
      ComPtr<IAsyncAction> async_action;
      HString sub_type;

      if (SUCCEEDED(hr)) {
        hr = supported_formats->GetAt(i, &media_frame_format);
      }

      // Filters out frame sources not sending frames in I420, YUY2, YV12 or
      // the format requested by the requested capabilities.
      if (SUCCEEDED(hr)) {
        hr = media_frame_format->get_Subtype(sub_type.ReleaseAndGetAddressOf());
      }

      if (SUCCEEDED(hr)) {
        VideoType video_type = ToVideoType(sub_type);
        if (video_type != capability.videoType &&
            video_type != VideoType::kI420 && video_type != VideoType::kYUY2 &&
            video_type != VideoType::kYV12 && video_type != VideoType::kNV12) {
          continue;
        }
      }

      // Filters out frame sources with resolution different than the one
      // defined by the requested capabilities.
      if (SUCCEEDED(hr)) {
        hr = media_frame_format->get_VideoFormat(&video_media_frame_format);
      }

      if (SUCCEEDED(hr)) {
        hr = video_media_frame_format->get_Width(&frame_width);
      }

      if (SUCCEEDED(hr)) {
        hr = video_media_frame_format->get_Height(&frame_height);
      }

      if (SUCCEEDED(hr) &&
          ((frame_width != static_cast<UINT32>(capability.width)) ||
           (frame_height != static_cast<UINT32>(capability.height)))) {
        continue;
      }

      // Filters out frames sources with frame rate higher than the what is
      // requested by the capabilities.
      if (SUCCEEDED(hr)) {
        hr = media_frame_format->get_FrameRate(&media_ratio);
      }

      if (SUCCEEDED(hr) && SafelyComputeMediaRatio(media_ratio.Get()) >
                               static_cast<uint32_t>(capability.maxFPS)) {
        continue;
      }

      // Select this as media frame source.
      if (SUCCEEDED(hr)) {
        media_frame_source = value;
      }

      if (SUCCEEDED(hr)) {
        hr = media_frame_source->SetFormatAsync(media_frame_format.Get(),
                                                &async_action);
      }

      if (SUCCEEDED(hr)) {
        hr = WaitForAsyncAction(async_action.Get());
      }

      break;
    }

    // The same camera might provide many sources, for example, Surface
    // Studio 2 camera has a color source provider and a depth source
    // provider. We don't need to continue looking for sources once the first
    // color source provider matches with the configuration we're looking for.
    if (SUCCEEDED(hr) && media_frame_source) {
      break;
    }
  }

  // video capture device with capabilities not found
  if (SUCCEEDED(hr)) {
    hr = media_frame_source ? S_OK : E_FAIL;
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture5->CreateFrameReaderAsync(media_frame_source.Get(),
                                                &async_operation);
  }

  if (SUCCEEDED(hr)) {
    hr = WaitForAsyncOperation(async_operation.Get());
  }

  // Assigns a new media frame reader
  if (SUCCEEDED(hr)) {
    hr = async_operation->GetResults(&media_frame_reader_);
  }

  if (SUCCEEDED(hr)) {
    hr = media_frame_reader_->add_FrameArrived(
        Callback<
            ITypedEventHandler<MediaFrameReader*, MediaFrameArrivedEventArgs*>>(
            [this](IMediaFrameReader* pSender,
                   IMediaFrameArrivedEventArgs* pEventArgs) {
              return this->FrameArrived(pSender, pEventArgs);
            })
            .Get(),
        &media_source_frame_arrived_token);
  }

  if (SUCCEEDED(hr)) {
    hr =
        media_frame_reader_->StartAsync(&async_media_frame_reader_start_status);
  }

  if (SUCCEEDED(hr)) {
    hr = WaitForAsyncOperation(async_media_frame_reader_start_status.Get());
  }

  if (SUCCEEDED(hr)) {
    hr = async_media_frame_reader_start_status->GetResults(
        &media_frame_reader_start_status);
  }

  if (SUCCEEDED(hr)) {
    hr =
        media_frame_reader_start_status !=
                MediaFrameReaderStartStatus::MediaFrameReaderStartStatus_Success
            ? E_FAIL
            : S_OK;
  }

  if (SUCCEEDED(hr)) {
    is_capturing = true;
  }

  return hr;
}

HRESULT VideoCaptureWinRTInternal::StopCapture() {
  HRESULT hr = S_OK;

  if (media_frame_reader_) {
    ComPtr<IAsyncAction> async_action;

    if (SUCCEEDED(hr)) {
      hr = media_frame_reader_->remove_FrameArrived(
          media_source_frame_arrived_token);
    }

    if (SUCCEEDED(hr)) {
      hr = media_frame_reader_->StopAsync(&async_action);
    }

    if (SUCCEEDED(hr)) {
      hr = WaitForAsyncAction(async_action.Get(), 250);
    }
  }

  if (media_capture_) {
    ComPtr<IClosable> closable_media_capture;

    if (SUCCEEDED(hr)) {
      hr = media_capture_.As(&closable_media_capture);
    }

    if (SUCCEEDED(hr)) {
      hr = closable_media_capture->Close();
    }
  }

  if (SUCCEEDED(hr)) {
    is_capturing = false;
  }

  media_frame_reader_.Reset();
  media_capture_.Reset();

  return hr;
}

bool VideoCaptureWinRTInternal::CaptureStarted() {
  return is_capturing;
}

HRESULT VideoCaptureWinRTInternal::FrameArrived(
    IMediaFrameReader* sender_no_ref,
    IMediaFrameArrivedEventArgs* args_no_ref) {
  UNREFERENCED_PARAMETER(args_no_ref);

  HRESULT hr = S_OK;
  ComPtr<IMediaFrameReader> media_frame_reader{sender_no_ref};
  ComPtr<IMediaFrameReference> media_frame_reference;
  ComPtr<IVideoMediaFrame> video_media_frame;
  ComPtr<IVideoMediaFrameFormat> video_media_frame_format;
  ComPtr<IMediaFrameFormat> media_frame_format;
  ComPtr<IMediaRatio> media_ratio;
  ComPtr<ISoftwareBitmap> software_bitmap;
  ComPtr<IBitmapBuffer> bitmap_buffer;
  ComPtr<IMemoryBuffer> memory_buffer;
  ComPtr<IMemoryBufferReference> memory_buffer_reference;
  ComPtr<IMemoryBufferByteAccess> memory_buffer_byte_access;
  BitmapPlaneDescription bitmap_plane_description_y;
  BitmapPlaneDescription bitmap_plane_description_uv;
  HString video_subtype;
  uint8_t* bitmap_content;
  uint32_t bitmap_capacity;
  int32_t plane_count;

  if (SUCCEEDED(hr)) {
    hr = media_frame_reader->TryAcquireLatestFrame(&media_frame_reference);
  }

  if (SUCCEEDED(hr) && media_frame_reference) {
    hr = media_frame_reference->get_VideoMediaFrame(&video_media_frame);

    if (SUCCEEDED(hr)) {
      hr = video_media_frame->get_VideoFormat(&video_media_frame_format);
    }

    if (SUCCEEDED(hr)) {
      hr = video_media_frame_format->get_MediaFrameFormat(&media_frame_format);
    }

    if (SUCCEEDED(hr)) {
      hr = media_frame_format->get_FrameRate(&media_ratio);
    }

    VideoCaptureCapability frameInfo;

    if (SUCCEEDED(hr)) {
      hr = video_media_frame_format->get_Width(
          reinterpret_cast<UINT32*>(&frameInfo.width));
    }

    if (SUCCEEDED(hr)) {
      hr = video_media_frame_format->get_Height(
          reinterpret_cast<UINT32*>(&frameInfo.height));
    }

    if (SUCCEEDED(hr)) {
      hr = media_frame_format->get_Subtype(
          video_subtype.ReleaseAndGetAddressOf());
    }

    if (SUCCEEDED(hr)) {
      frameInfo.videoType = ToVideoType(video_subtype);
      frameInfo.maxFPS = SafelyComputeMediaRatio(media_ratio.Get());
      frameInfo.interlaced = false;
    }

    if (SUCCEEDED(hr)) {
      hr = video_media_frame->get_SoftwareBitmap(&software_bitmap);
    }

    if (SUCCEEDED(hr)) {
      hr = software_bitmap->LockBuffer(
          BitmapBufferAccessMode::BitmapBufferAccessMode_Read, &bitmap_buffer);
    }

    if (SUCCEEDED(hr)) {
      hr = bitmap_buffer->GetPlaneCount(&plane_count);
    }

    if (SUCCEEDED(hr) && plane_count >= 1) {
      hr = bitmap_buffer->GetPlaneDescription(0, &bitmap_plane_description_y);
    }

    if (SUCCEEDED(hr) && plane_count == 2) {
      hr = bitmap_buffer->GetPlaneDescription(1, &bitmap_plane_description_uv);
    }

    if (SUCCEEDED(hr)) {
      hr = bitmap_buffer.As(&memory_buffer);
    }

    if (SUCCEEDED(hr)) {
      hr = memory_buffer->CreateReference(&memory_buffer_reference);
    }

    if (SUCCEEDED(hr)) {
      hr = memory_buffer_reference.As(&memory_buffer_byte_access);
    }

    if (SUCCEEDED(hr)) {
      hr = memory_buffer_byte_access->GetBuffer(&bitmap_content,
                                                &bitmap_capacity);
    }

    if (SUCCEEDED(hr)) {
      pfn_incoming_frame_(
          bitmap_content,                                           // Plane Y
          bitmap_plane_description_y.Stride,                        // Stride Y
          bitmap_content + bitmap_plane_description_uv.StartIndex,  // Plane UV
          bitmap_plane_description_uv.Stride,                       // Stride UV
          bitmap_capacity,  // YUV buffer size
          frameInfo,        // Y Width and Height
          0);               // Capture Time
    }
  }

  if (memory_buffer_reference) {
    ComPtr<IClosable> closable;
    memory_buffer_reference.As(&closable);
    closable->Close();
  }

  if (bitmap_buffer) {
    ComPtr<IClosable> closable;
    bitmap_buffer.As(&closable);
    closable->Close();
  }

  if (software_bitmap) {
    ComPtr<IClosable> closable;
    software_bitmap.As(&closable);
    closable->Close();
  }

  if (media_frame_reference) {
    ComPtr<IClosable> closable;
    media_frame_reference.As(&closable);
    closable->Close();
  }

  return hr;
}

// Avoids forward declaring VideoCaptureWinRTInternal in the header.
constexpr VideoCaptureWinRTInternal* Impl(void* video_capture_internal) {
  return static_cast<VideoCaptureWinRTInternal*>(video_capture_internal);
}

///////////////////////////////////////////////////////////////////////////////
//
//   VideoCaptureWinRT
//
///////////////////////////////////////////////////////////////////////////////

VideoCaptureWinRT::VideoCaptureWinRT()
    : video_capture_internal_(new VideoCaptureWinRTInternal(
          std::bind(&VideoCaptureWinRT::IncomingFrame,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    std::placeholders::_5,
                    std::placeholders::_6,
                    std::placeholders::_7))) {}

VideoCaptureWinRT::~VideoCaptureWinRT() {
  delete Impl(video_capture_internal_);
}

// Helper method for filling _deviceUniqueId defined by the super class
int32_t VideoCaptureWinRT::SetDeviceUniqueId(
    const char* device_unique_id_UTF8) {
  auto device_id_length =
      strnlen(device_unique_id_UTF8, kVideoCaptureUniqueNameLength);

  if (device_id_length == kVideoCaptureUniqueNameLength) {
    RTC_LOG(LS_INFO) << "deviceUniqueId too long";
    return -1;
  }

  if (_deviceUniqueId) {
    RTC_LOG(LS_INFO) << "_deviceUniqueId leaked";
    delete[] _deviceUniqueId;
    _deviceUniqueId = nullptr;
  }

  // Store the device name
  // VideoCaptureImpl::~VideoCaptureImpl reclaims _deviceUniqueId
  _deviceUniqueId = new char[device_id_length + 1];
  memcpy(_deviceUniqueId, device_unique_id_UTF8, device_id_length + 1);

  return 0;
}

int32_t VideoCaptureWinRT::Init(const char* device_unique_id_UTF8) {
  // Gets hstring from deviceId utf8
  wchar_t device_id_w[kVideoCaptureUniqueNameLength];
  int device_id_w_length = MultiByteToWideChar(
      CP_UTF8, 0, device_unique_id_UTF8, -1, device_id_w, sizeof(device_id_w));
  if (device_id_w_length == 0) {
    return -1;
  }

  // Sets _deviceUniqueId defined by the super class
  if (SetDeviceUniqueId(device_unique_id_UTF8)) {
    return -1;
  }

  return 0;
}

int32_t VideoCaptureWinRT::StartCapture(
    const VideoCaptureCapability& capability) {
  rtc::CritScope cs(&_apiCs);

  if (CaptureStarted()) {
    if (capability == _requestedCapability) {
      return 0;
    }
    StopCapture();
  }

  int32_t ret = Impl(video_capture_internal_)->StartCapture(capability);

  if (SUCCEEDED(ret)) {
    _requestedCapability = capability;
  }

  return ret;
}

int32_t VideoCaptureWinRT::StopCapture() {
  rtc::CritScope cs(&_apiCs);
  return Impl(video_capture_internal_)->StopCapture();
}

bool VideoCaptureWinRT::CaptureStarted() {
  return Impl(video_capture_internal_)->CaptureStarted();
}

int32_t VideoCaptureWinRT::CaptureSettings(VideoCaptureCapability& settings) {
  settings = _requestedCapability;
  return 0;
}

}  // namespace videocapturemodule
}  // namespace webrtc
