/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_capture/windows/device_info_winrt.h"

#include <Windows.Devices.Enumeration.h>
#include <Windows.Foundation.Collections.h>
#include <windows.foundation.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include <memory>
#include <vector>

#include "modules/video_capture/video_capture_config.h"
#include "modules/video_capture/windows/help_functions_winrt.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_utils.h"

using ::ABI::Windows::ApplicationModel::Core::ICoreApplication;
using ::ABI::Windows::ApplicationModel::Core::ICoreApplicationView;
using ::ABI::Windows::ApplicationModel::Core::ICoreImmersiveApplication;
using ::ABI::Windows::Devices::Enumeration::DeviceClass;
using ::ABI::Windows::Devices::Enumeration::DeviceClass_VideoCapture;
using ::ABI::Windows::Devices::Enumeration::DeviceInformation;
using ::ABI::Windows::Devices::Enumeration::DeviceInformationCollection;
using ::ABI::Windows::Devices::Enumeration::IDeviceInformation;
using ::ABI::Windows::Devices::Enumeration::IDeviceInformationStatics;
using ::ABI::Windows::Foundation::ActivateInstance;
using ::ABI::Windows::Foundation::GetActivationFactory;
using ::ABI::Windows::Foundation::IAsyncAction;
using ::ABI::Windows::Foundation::IAsyncActionCompletedHandler;
using ::ABI::Windows::Foundation::IAsyncOperation;
using ::ABI::Windows::Foundation::IClosable;
using ::ABI::Windows::Foundation::Collections::IVectorView;
using ::ABI::Windows::Media::Capture::IMediaCapture;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings5;
using ::ABI::Windows::Media::Capture::IMediaCaptureStatics;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfile;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfileMediaDescription;
using ::ABI::Windows::Media::Capture::
    IMediaCaptureVideoProfileMediaDescription2;
using ::ABI::Windows::Media::Capture::MediaCaptureMemoryPreference;
using ::ABI::Windows::Media::Capture::MediaCaptureVideoProfile;
using ::ABI::Windows::Media::Capture::MediaCaptureVideoProfileMediaDescription;
using ::ABI::Windows::Media::Capture::MediaStreamType;
using ::ABI::Windows::Media::Capture::StreamingCaptureMode;
using ::ABI::Windows::Media::Devices::IMediaDeviceController;
using ::ABI::Windows::Media::Devices::IVideoDeviceController;
using ::ABI::Windows::Media::MediaProperties::IMediaEncodingProperties;
using ::ABI::Windows::Media::MediaProperties::IMediaRatio;
using ::ABI::Windows::Media::MediaProperties::IVideoEncodingProperties;
using ::ABI::Windows::UI::Core::CoreDispatcherPriority;
using ::ABI::Windows::UI::Core::CoreProcessEventsOption;
using ::ABI::Windows::UI::Core::ICoreDispatcher;
using ::ABI::Windows::UI::Core::ICoreWindow;
using ::ABI::Windows::UI::Core::IDispatchedHandler;
using ::Microsoft::WRL::Callback;
using ::Microsoft::WRL::ComPtr;
using ::Microsoft::WRL::Delegate;
using ::Microsoft::WRL::FtmBase;
using ::Microsoft::WRL::Implements;
using ::Microsoft::WRL::RuntimeClassFlags;
using ::Microsoft::WRL::Wrappers::Event;
using ::Microsoft::WRL::Wrappers::HString;
using ::Microsoft::WRL::Wrappers::HStringReference;
using ::Microsoft::WRL::Wrappers::RoInitializeWrapper;
using ::std::vector;

namespace webrtc {
namespace videocapturemodule {

///////////////////////////////////////////////////////////////////////////////
//
// DeviceInfoWinRTInternal
//
///////////////////////////////////////////////////////////////////////////////
struct DeviceInfoWinRTInternal {
  DeviceInfoWinRTInternal();

  ~DeviceInfoWinRTInternal() = default;

  HRESULT GetNumberOfDevices(uint32_t* device_count);

  HRESULT GetDeviceName(uint32_t device_number,
                        char* device_name_UTF8,
                        uint32_t device_name_length,
                        char* device_unique_id_UTF8,
                        uint32_t device_unique_id_UTF8_length,
                        char* product_unique_id_UTF8,
                        uint32_t product_unique_id_UTF8_length);

  HRESULT CreateCapabilityMap(
      const wchar_t* device_unique_id,
      vector<VideoCaptureCapability>* video_capture_capabilities);

 private:
  HRESULT GetDeviceInformationCollection(
      IVectorView<DeviceInformation*>** device_collection);

  HRESULT AssureMediaCaptureStatics();

  HRESULT IsVideoProfileSupported(const HStringReference& device_id,
                                  boolean* video_profile_supported);

  HRESULT GetMediaCaptureVideoProfilesForDevice(
      const HStringReference& device_id,
      ComPtr<IVectorView<MediaCaptureVideoProfile*>>& video_profiles);

  HRESULT FillVideoCaptureCapabilityFromProfiles(
      const ComPtr<IVectorView<MediaCaptureVideoProfile*>>& video_profiles,
      vector<VideoCaptureCapability>& device_caps);

  HRESULT FillVideoCaptureCapabilityFromProfile(
      const ComPtr<IMediaCaptureVideoProfile>& video_profile,
      vector<VideoCaptureCapability>& device_caps);

  HRESULT FillVideoCaptureCapabilityFromDeviceProfiles(
      const HStringReference& device_id,
      vector<VideoCaptureCapability>& device_caps);

  HRESULT FillVideoCaptureCapabilityFromMediaController(
      const ComPtr<IMediaDeviceController>& media_device_controller,
      vector<VideoCaptureCapability>& device_caps);

  HRESULT FillVideoCaptureCapabilityFromDeviceWithoutProfiles(
      const HStringReference& device_id,
      vector<VideoCaptureCapability>& device_caps);

  RoInitializeWrapper ro_initialize_wrapper_;
  ComPtr<IMediaCaptureStatics> media_capture_statics_;
};

DeviceInfoWinRTInternal::DeviceInfoWinRTInternal()
    : ro_initialize_wrapper_(RO_INIT_MULTITHREADED) {}

HRESULT DeviceInfoWinRTInternal::AssureMediaCaptureStatics() {
  HRESULT hr = S_OK;

  if (!media_capture_statics_) {
    hr = GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Media_Capture_MediaCapture).Get(),
        media_capture_statics_.ReleaseAndGetAddressOf());
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::IsVideoProfileSupported(
    _In_ const HStringReference& device_id,
    _Out_ boolean* video_profile_supported) {
  HRESULT hr = S_OK;

  if (SUCCEEDED(hr)) {
    hr = AssureMediaCaptureStatics();
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture_statics_->IsVideoProfileSupported(
        device_id.Get(), video_profile_supported);
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::GetMediaCaptureVideoProfilesForDevice(
    const HStringReference& device_id,
    ComPtr<IVectorView<MediaCaptureVideoProfile*>>& video_profiles) {
  HRESULT hr = S_OK;

  if (SUCCEEDED(hr)) {
    hr = AssureMediaCaptureStatics();
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture_statics_->FindAllVideoProfiles(device_id.Get(),
                                                      &video_profiles);
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::FillVideoCaptureCapabilityFromProfiles(
    const ComPtr<IVectorView<MediaCaptureVideoProfile*>>& video_profiles,
    vector<VideoCaptureCapability>& device_caps) {
  HRESULT hr = S_OK;
  unsigned int video_profiles_size = 0;

  if (SUCCEEDED(hr)) {
    hr = video_profiles->get_Size(&video_profiles_size);
  }

  for (unsigned int i = 0; SUCCEEDED(hr) && i < video_profiles_size; ++i) {
    ComPtr<IMediaCaptureVideoProfile> media_capture_video_profile;
    HString profile_id;

    if (SUCCEEDED(hr)) {
      hr = video_profiles->GetAt(i, &media_capture_video_profile);
    }

    if (SUCCEEDED(hr)) {
      hr = FillVideoCaptureCapabilityFromProfile(media_capture_video_profile,
                                                 device_caps);
    }
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::FillVideoCaptureCapabilityFromProfile(
    const ComPtr<IMediaCaptureVideoProfile>& video_profile,
    vector<VideoCaptureCapability>& device_caps) {
  HRESULT hr = S_OK;
  ComPtr<IVectorView<MediaCaptureVideoProfileMediaDescription*>>
      profile_description_list;
  HString profile_id;
  unsigned int profile_description_size = 0;

  if (SUCCEEDED(hr)) {
    hr = video_profile->get_SupportedRecordMediaDescription(
        &profile_description_list);
  }

  if (SUCCEEDED(hr)) {
    hr = video_profile->get_Id(profile_id.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = profile_description_list->get_Size(&profile_description_size);
  }

  for (unsigned int i = 0; SUCCEEDED(hr) && i < profile_description_size; ++i) {
    ComPtr<IMediaCaptureVideoProfileMediaDescription> profile_description;
    ComPtr<IMediaCaptureVideoProfileMediaDescription2> profile_description2;
    ComPtr<IUnknown> video_profile_as_unknown;
    ComPtr<IUnknown> profile_description_as_unknown;
    HString subtype;
    VideoCaptureCapability video_capture_capability;
    double frame_rate;

    if (SUCCEEDED(hr)) {
      hr = profile_description_list->GetAt(i, &profile_description);
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description->get_Width(
          reinterpret_cast<UINT32*>(&video_capture_capability.width));
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description->get_Height(
          reinterpret_cast<UINT32*>(&video_capture_capability.height));
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description->get_FrameRate(&frame_rate);
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description.As(&profile_description2);
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description2->get_Subtype(subtype.ReleaseAndGetAddressOf());
    }

    if (SUCCEEDED(hr)) {
      hr = video_profile.As(&video_profile_as_unknown);
    }

    if (SUCCEEDED(hr)) {
      video_capture_capability.media_capture_video_profile =
          video_profile_as_unknown;
    }

    if (SUCCEEDED(hr)) {
      hr = profile_description.As(&profile_description_as_unknown);
    }

    if (SUCCEEDED(hr)) {
      video_capture_capability.record_media_description =
          profile_description_as_unknown;
    }

    if (SUCCEEDED(hr)) {
      video_capture_capability.videoType = ToVideoType(subtype);
      video_capture_capability.maxFPS = frame_rate;
      video_capture_capability.interlaced = false;
      video_capture_capability.profile_id = profile_id.GetRawBuffer(nullptr);
      device_caps.push_back(video_capture_capability);
    }
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::FillVideoCaptureCapabilityFromDeviceProfiles(
    const HStringReference& device_id,
    vector<VideoCaptureCapability>& device_caps) {
  HRESULT hr = S_OK;
  ComPtr<IVectorView<MediaCaptureVideoProfile*>> video_profiles;

  if (SUCCEEDED(hr)) {
    hr = GetMediaCaptureVideoProfilesForDevice(device_id, video_profiles);
  }

  if (SUCCEEDED(hr)) {
    hr = FillVideoCaptureCapabilityFromProfiles(video_profiles, device_caps);
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::FillVideoCaptureCapabilityFromMediaController(
    const ComPtr<IMediaDeviceController>& media_device_controller,
    vector<VideoCaptureCapability>& device_caps) {
  HRESULT hr = S_OK;
  ComPtr<IVectorView<IMediaEncodingProperties*>> stream_capabilities;
  unsigned int stream_capabilities_size = 0;

  if (SUCCEEDED(hr)) {
    hr = media_device_controller->GetAvailableMediaStreamProperties(
        MediaStreamType::MediaStreamType_VideoRecord, &stream_capabilities);
  }

  if (SUCCEEDED(hr)) {
    hr = stream_capabilities->get_Size(&stream_capabilities_size);
  }

  for (unsigned int i = 0; SUCCEEDED(hr) && i < stream_capabilities_size; ++i) {
    ComPtr<IMediaEncodingProperties> media_encoding_props;
    ComPtr<IVideoEncodingProperties> video_encoding_props;
    ComPtr<IMediaRatio> media_ratio;
    VideoCaptureCapability video_capture_capability;
    HString subtype;

    if (SUCCEEDED(hr)) {
      hr = stream_capabilities->GetAt(i, &media_encoding_props);
    }

    if (SUCCEEDED(hr)) {
      hr = media_encoding_props.As(&video_encoding_props);
    }

    if (SUCCEEDED(hr)) {
      hr = video_encoding_props->get_Width(
          reinterpret_cast<UINT32*>(&video_capture_capability.width));
    }

    if (SUCCEEDED(hr)) {
      hr = video_encoding_props->get_Height(
          reinterpret_cast<UINT32*>(&video_capture_capability.height));
    }

    if (SUCCEEDED(hr)) {
      hr = video_encoding_props->get_FrameRate(&media_ratio);
    }

    if (SUCCEEDED(hr)) {
      video_capture_capability.maxFPS =
          SafelyComputeMediaRatio(media_ratio.Get());

      hr = media_encoding_props->get_Subtype(subtype.ReleaseAndGetAddressOf());
    }

    if (SUCCEEDED(hr)) {
      video_capture_capability.videoType = ToVideoType(subtype);
      video_capture_capability.interlaced = false;
      device_caps.push_back(video_capture_capability);
    }
  }

  return hr;
}

HRESULT
DeviceInfoWinRTInternal::FillVideoCaptureCapabilityFromDeviceWithoutProfiles(
    const HStringReference& device_id,
    vector<VideoCaptureCapability>& device_caps) {
  HRESULT hr = S_OK;
  ComPtr<IMediaCaptureInitializationSettings> initialization_settings;
  ComPtr<IMediaCapture> media_capture;
  ComPtr<IClosable> media_capture_closable;
  ComPtr<IVideoDeviceController> video_device_controller;
  ComPtr<IMediaDeviceController> media_device_controller;

  if (SUCCEEDED(hr)) {
    hr = CreateMediaCaptureInitializationSettings(
        device_id, nullptr, nullptr,
        initialization_settings.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = GetMediaCaptureWithInitSettings(
        initialization_settings.Get(), media_capture.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture->get_VideoDeviceController(&video_device_controller);
  }

  if (SUCCEEDED(hr)) {
    hr = video_device_controller.As(&media_device_controller);
  }

  if (SUCCEEDED(hr)) {
    hr = FillVideoCaptureCapabilityFromMediaController(media_device_controller,
                                                       device_caps);
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture.As(&media_capture_closable);
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture_closable->Close();
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::GetDeviceInformationCollection(
    IVectorView<DeviceInformation*>** device_collection) {
  HRESULT hr = S_OK;
  ComPtr<IActivationFactory> activation_factory;
  ComPtr<IDeviceInformationStatics> device_info_statics;
  ComPtr<IAsyncOperation<DeviceInformationCollection*>>
      async_op_device_info_collection;

  // Get the object containing the DeviceInformation static methods.
  if (SUCCEEDED(hr)) {
    hr = GetActivationFactory(
        HStringReference(
            RuntimeClass_Windows_Devices_Enumeration_DeviceInformation)
            .Get(),
        &activation_factory);
  }

  if (SUCCEEDED(hr)) {
    hr = activation_factory.As(&device_info_statics);
  }

  // Call FindAllAsync and then start the async operation.
  if (SUCCEEDED(hr)) {
    hr = device_info_statics->FindAllAsyncDeviceClass(
        DeviceClass_VideoCapture, &async_op_device_info_collection);
  }

  // Block and suspend thread until the async operation finishes or timeouts.
  if (SUCCEEDED(hr)) {
    hr = WaitForAsyncOperation(async_op_device_info_collection.Get());
  }

  // Returns device collection if async operation completed successfully.
  if (SUCCEEDED(hr)) {
    hr = async_op_device_info_collection->GetResults(device_collection);
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::GetNumberOfDevices(uint32_t* device_count) {
  HRESULT hr = S_OK;
  ComPtr<IVectorView<DeviceInformation*>> device_info_collection;

  if (SUCCEEDED(hr)) {
    hr = GetDeviceInformationCollection(
        device_info_collection.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = device_info_collection->get_Size(device_count);
  }

  return hr;
}

HRESULT DeviceInfoWinRTInternal::GetDeviceName(
    uint32_t device_number,
    char* device_name_UTF8,
    uint32_t device_name_length,
    char* device_unique_id_UTF8,
    uint32_t device_unique_id_UTF8_length,
    char* product_unique_id_UTF8,
    uint32_t product_unique_id_UTF8_length) {
  HRESULT hr = S_OK;
  uint32_t device_count;
  ComPtr<IVectorView<DeviceInformation*>> device_info_collection;
  ComPtr<IDeviceInformation> device_info;

  // Gets the device information collection synchronously
  if (SUCCEEDED(hr)) {
    hr = GetDeviceInformationCollection(
        device_info_collection.ReleaseAndGetAddressOf());
  }

  // Checks if desired device index is within the collection
  if (SUCCEEDED(hr)) {
    hr = device_info_collection->get_Size(&device_count);
  }

  if (SUCCEEDED(hr) && (device_number >= device_count)) {
    RTC_LOG(LS_INFO) << "Device number is out of bounds";
    hr = E_BOUNDS;
  }

  if (SUCCEEDED(hr)) {
    hr = device_info_collection->GetAt(device_number, &device_info);
  }

  if (SUCCEEDED(hr) && (device_name_length > 0)) {
    HString video_capture_name;
    hr = device_info->get_Name(video_capture_name.ReleaseAndGetAddressOf());

    // rtc::ToUtf8 does not check for errors
    if (SUCCEEDED(hr) &&
        ::WideCharToMultiByte(CP_UTF8, 0,
                              video_capture_name.GetRawBuffer(nullptr), -1,
                              reinterpret_cast<char*>(device_name_UTF8),
                              device_name_length, NULL, NULL) == 0) {
      RTC_LOG(LS_INFO) << "Failed to convert device name to UTF8, error = "
                       << GetLastError();
      hr = E_FAIL;
    }
  }

  if (SUCCEEDED(hr) && (device_unique_id_UTF8_length > 0)) {
    HString video_capture_id;
    hr = device_info->get_Id(video_capture_id.ReleaseAndGetAddressOf());

    // rtc::ToUtf8 does not check for errors
    if (SUCCEEDED(hr) && ::WideCharToMultiByte(
                             CP_UTF8, 0, video_capture_id.GetRawBuffer(nullptr),
                             -1, reinterpret_cast<char*>(device_unique_id_UTF8),
                             device_unique_id_UTF8_length, NULL, NULL) == 0) {
      RTC_LOG(LS_INFO) << "Failed to convert device id to UTF8, error = "
                       << GetLastError();
      hr = E_FAIL;
    }
  }

  if (SUCCEEDED(hr) && product_unique_id_UTF8_length > 0) {
    HString video_capture_id_hs;
    unsigned int hs_lenght;
    const wchar_t* video_capture_id_wc;
    std::unique_ptr<wchar_t[]> buffer;
    wchar_t *token, *next_token;

    hr = device_info->get_Id(video_capture_id_hs.ReleaseAndGetAddressOf());

    if (SUCCEEDED(hr)) {
      video_capture_id_wc = video_capture_id_hs.GetRawBuffer(&hs_lenght);
      hr = video_capture_id_wc ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr)) {
      // hs_lenght doesn't count \0 needed by wcscpy_s.
      ++hs_lenght;

      // The contents of the HString has to be copied to buffer because
      // wcstok_s is destructive operation.
      buffer = std::make_unique<wchar_t[]>(hs_lenght);
      hr = buffer ? S_OK : E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr)) {
      hr = (0 == wcscpy_s(buffer.get(), hs_lenght, video_capture_id_wc)
                ? S_OK
                : E_FAIL);
    }

    if (SUCCEEDED(hr)) {
      token = wcstok_s(buffer.get(), L"&", &next_token);
      hr = token ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr)) {
      token = wcstok_s(nullptr, L"&", &next_token);
      hr = token ? S_OK : E_FAIL;
    }

    // rtc::ToUtf8 does not check for errors
    if (SUCCEEDED(hr) &&
        ::WideCharToMultiByte(CP_UTF8, 0, token, -1,
                              reinterpret_cast<char*>(product_unique_id_UTF8),
                              product_unique_id_UTF8_length, NULL, NULL) == 0) {
      RTC_LOG(LS_INFO)
          << "Failed to convert product unique id to UTF8, error = "
          << GetLastError();
      hr = E_FAIL;
    }
  }

  return hr;
}  // namespace videocapturemodule

HRESULT DeviceInfoWinRTInternal::CreateCapabilityMap(
    const wchar_t* device_unique_id,
    vector<VideoCaptureCapability>* video_capture_capabilities) {
  HRESULT hr = S_OK;
  HStringReference device_id(device_unique_id);
  vector<VideoCaptureCapability> device_caps;

  if (SUCCEEDED(hr)) {
    hr = video_capture_capabilities ? S_OK : E_INVALIDARG;
  }

  if (SUCCEEDED(hr)) {
    hr = FillVideoCaptureCapabilityFromDeviceProfiles(device_id, device_caps);
  }

  if (SUCCEEDED(hr)) {
    hr = FillVideoCaptureCapabilityFromDeviceWithoutProfiles(device_id,
                                                             device_caps);
  }

  if (SUCCEEDED(hr)) {
    // All media calls succeeded, let's copy the results.
    video_capture_capabilities->swap(device_caps);
  }

  return hr;
}

// Allows not forward declaring DeviceInfoWinRTInternal in the header.
constexpr DeviceInfoWinRTInternal* Impl(void* device_info_internal) {
  return static_cast<DeviceInfoWinRTInternal*>(device_info_internal);
}

///////////////////////////////////////////////////////////////////////////////
//
// DeviceInfoWinRT
//
///////////////////////////////////////////////////////////////////////////////

// static
DeviceInfoWinRT* DeviceInfoWinRT::Create() {
  return new DeviceInfoWinRT();
}

DeviceInfoWinRT::DeviceInfoWinRT()
    : device_info_internal_(new DeviceInfoWinRTInternal()) {}

DeviceInfoWinRT::~DeviceInfoWinRT() {
  delete Impl(device_info_internal_);
}

int32_t DeviceInfoWinRT::Init() {
  return 0;
}

uint32_t DeviceInfoWinRT::NumberOfDevices() {
  ReadLockScoped cs(_apiLock);
  uint32_t device_count = -1;

  HRESULT hr = Impl(device_info_internal_)->GetNumberOfDevices(&device_count);

  return SUCCEEDED(hr) ? device_count : -1;
}

int32_t DeviceInfoWinRT::GetDeviceName(uint32_t device_number,
                                       char* device_name_UTF8,
                                       uint32_t device_name_length,
                                       char* device_unique_id_UTF8,
                                       uint32_t device_unique_id_UTF8_length,
                                       char* product_unique_id_UTF8,
                                       uint32_t product_unique_id_UTF8_length) {
  ReadLockScoped cs(_apiLock);

  HRESULT hr = Impl(device_info_internal_)
                   ->GetDeviceName(
                       device_number, device_name_UTF8, device_name_length,
                       device_unique_id_UTF8, device_unique_id_UTF8_length,
                       product_unique_id_UTF8, product_unique_id_UTF8_length);

  if (SUCCEEDED(hr) && device_name_length) {
    RTC_LOG_F(LS_INFO) << " " << device_name_UTF8;
  }

  return SUCCEEDED(hr) ? 0 : -1;
}

int32_t DeviceInfoWinRT::DisplayCaptureSettingsDialogBox(
    const char* device_unique_id_utf8,
    const char* dialog_title_utf8,
    void* parent_window,
    uint32_t position_x,
    uint32_t position_y) {
  return -1;
}

int32_t DeviceInfoWinRT::CreateCapabilityMap(
    const char* device_unique_id_utf8) {
  // Checks if device_unique_id_utf8 length is not too long
  const int32_t device_unique_id_UTF8_length =
      (int32_t)strnlen(reinterpret_cast<const char*>(device_unique_id_utf8),
                       kVideoCaptureUniqueNameLength);

  if (device_unique_id_UTF8_length == kVideoCaptureUniqueNameLength) {
    RTC_LOG(LS_INFO) << "Device ID too long";
    return -1;
  }

  RTC_LOG(LS_INFO) << "CreateCapabilityMap called for device "
                   << device_unique_id_utf8;

  wchar_t deviceIdW[kVideoCaptureUniqueNameLength];
  int device_id_w_length = ::MultiByteToWideChar(
      CP_UTF8, 0, device_unique_id_utf8, -1, deviceIdW, sizeof(deviceIdW));
  if (device_id_w_length == 0) {
    RTC_LOG(LS_INFO) << "Failed to convert Device ID from UTF8, error = "
                     << GetLastError();
    return -1;
  }

  if (FAILED(Impl(device_info_internal_)
                 ->CreateCapabilityMap(deviceIdW, &_captureCapabilities))) {
    return -1;
  }

  // Store the new used device name
  _lastUsedDeviceNameLength = device_unique_id_UTF8_length;
  _lastUsedDeviceName = reinterpret_cast<char*>(
      realloc(_lastUsedDeviceName, _lastUsedDeviceNameLength + 1));
  memcpy(_lastUsedDeviceName, device_unique_id_utf8,
         _lastUsedDeviceNameLength + 1);

  RTC_LOG(LS_INFO) << "CreateCapabilityMap " << _captureCapabilities.size();

  return static_cast<int32_t>(_captureCapabilities.size());
}

}  // namespace videocapturemodule
}  // namespace webrtc
