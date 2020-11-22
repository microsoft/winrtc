/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_capture/windows/help_functions_winrt.h"

#include <Windows.ApplicationModel.core.h>
#include <Windows.ApplicationModel.h>

#include <memory>

#include "rtc_base/logging.h"

using ::ABI::Windows::ApplicationModel::Core::ICoreApplication;
using ::ABI::Windows::ApplicationModel::Core::ICoreApplicationView;
using ::ABI::Windows::ApplicationModel::Core::ICoreImmersiveApplication;
using ::ABI::Windows::Foundation::ActivateInstance;
using ::ABI::Windows::Foundation::AsyncStatus;
using ::ABI::Windows::Foundation::GetActivationFactory;
using ::ABI::Windows::Foundation::IAsyncAction;
using ::ABI::Windows::Foundation::IAsyncActionCompletedHandler;
using ::ABI::Windows::Foundation::IAsyncInfo;
using ::ABI::Windows::Media::Capture::IMediaCapture;
using ::ABI::Windows::Media::Capture::IMediaCaptureFailedEventArgs;
using ::ABI::Windows::Media::Capture::IMediaCaptureFailedEventHandler;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings4;
using ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings5;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfile;
using ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfileMediaDescription;
using ::ABI::Windows::Media::Capture::MediaCaptureMemoryPreference;
using ::ABI::Windows::Media::Capture::StreamingCaptureMode;
using ::ABI::Windows::Media::MediaProperties::IMediaRatio;
using ::ABI::Windows::UI::Core::CoreDispatcherPriority;
using ::ABI::Windows::UI::Core::CoreProcessEventsOption;
using ::ABI::Windows::UI::Core::ICoreDispatcher;
using ::ABI::Windows::UI::Core::ICoreWindow;
using ::ABI::Windows::UI::Core::IDispatchedHandler;
using ::Microsoft::WRL::AgileRef;
using ::Microsoft::WRL::Callback;
using ::Microsoft::WRL::ComPtr;
using ::Microsoft::WRL::Delegate;
using ::Microsoft::WRL::FtmBase;
using ::Microsoft::WRL::Implements;
using ::Microsoft::WRL::RuntimeClassFlags;
using ::Microsoft::WRL::Wrappers::Event;
using ::Microsoft::WRL::Wrappers::HString;
using ::Microsoft::WRL::Wrappers::HStringReference;

namespace webrtc {
namespace videocapturemodule {

HRESULT GetMediaCaptureWithInitSettingsWin32(
    _In_ ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings*
        initialization_settings,
    _COM_Outptr_ ::ABI::Windows::Media::Capture::IMediaCapture**
        pp_media_capture) {
  HRESULT hr = S_OK;
  ComPtr<IMediaCapture> media_capture;
  ComPtr<IAsyncAction> media_capture_async;

  if (SUCCEEDED(hr)) {
    hr = ActivateInstance(
        HStringReference(RuntimeClass_Windows_Media_Capture_MediaCapture).Get(),
        media_capture.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture->InitializeWithSettingsAsync(
        initialization_settings, media_capture_async.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = WaitForAsyncAction(media_capture_async.Get());
  }

  if (SUCCEEDED(hr)) {
    *pp_media_capture = media_capture.Detach();
  } else {
    *pp_media_capture = nullptr;
  }

  return hr;
}  // namespace webrtc

HRESULT GetMediaCaptureWithInitSettingsUWP(
    _In_ IMediaCaptureInitializationSettings* initialization_settings,
    _COM_Outptr_ IMediaCapture** pp_media_capture) {
  HRESULT hr = S_OK;
  ComPtr<IMediaCapture> media_capture;
  ComPtr<ICoreDispatcher> main_view_dispatcher;
  ComPtr<IAsyncAction> async_action_ui_dispatcher;
  ComPtr<IAsyncAction> async_action_media_capture;
  AgileRef async_action_media_capture_agile;
  AgileRef media_capture_agile;
  Event event_wait_for_media_capture_async_action;
  Event event_wait_for_media_capture_async_action_completion;
  boolean has_thread_access;

  // Acquires the main view dispacther (UI thread).
  if (SUCCEEDED(hr)) {
    hr = GetMainViewDispatcher(&main_view_dispatcher);
  }

  // We'll be dispatching and waiting for code from the UI thread.
  // Let's make sure this is not the UI thread.
  if (SUCCEEDED(hr)) {
    hr = main_view_dispatcher->get_HasThreadAccess(&has_thread_access);
  }

  if (SUCCEEDED(hr)) {
    hr = has_thread_access ? RPC_E_WRONG_THREAD : S_OK;
  }

  // The media capture device has to be initialized in the UI thread.
  // The following event is used for letting this thread know that
  // async_action_media_capture has been created.
  if (SUCCEEDED(hr)) {
    event_wait_for_media_capture_async_action =
        Event(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET,
                              WRITE_OWNER | EVENT_ALL_ACCESS));
  }

  if (SUCCEEDED(hr)) {
    hr = event_wait_for_media_capture_async_action.IsValid()
             ? S_OK
             : HRESULT_FROM_WIN32(GetLastError());
  }

  if (SUCCEEDED(hr)) {
    hr = ActivateInstance(
        HStringReference(RuntimeClass_Windows_Media_Capture_MediaCapture).Get(),
        media_capture.ReleaseAndGetAddressOf());
  }

  if (SUCCEEDED(hr)) {
    hr = media_capture.AsAgile(&media_capture_agile);
  }

  // Dispatches the media capture initialization to the main view
  // dispatcher.
  if (SUCCEEDED(hr)) {
    hr = main_view_dispatcher->RunAsync(
        CoreDispatcherPriority::CoreDispatcherPriority_Normal,
        Callback<Implements<RuntimeClassFlags<Delegate>, IDispatchedHandler,
                            FtmBase>>(
            [&event_wait_for_media_capture_async_action,
             &async_action_media_capture_agile, &media_capture_agile,
             &initialization_settings]() -> HRESULT {
              HRESULT hr = S_OK;
              ComPtr<IAsyncAction> media_capture_async;
              ComPtr<IMediaCapture> media_capture;

              if (SUCCEEDED(hr)) {
                hr = media_capture_agile.As(&media_capture);
              }

              if (SUCCEEDED(hr)) {
                hr = media_capture->InitializeWithSettingsAsync(
                    initialization_settings,
                    media_capture_async.ReleaseAndGetAddressOf());
              }

              if (SUCCEEDED(hr)) {
                hr = media_capture_async.AsAgile(
                    &async_action_media_capture_agile);
              }

              ::SetEvent(event_wait_for_media_capture_async_action.Get());

              return hr;
            })
            .Get(),
        &async_action_ui_dispatcher);
  }

  // Waits for the UI thread to create async_action_media_capture.
  if (SUCCEEDED(hr)) {
    hr =
        ::WaitForSingleObjectEx(event_wait_for_media_capture_async_action.Get(),
                                INFINITE, FALSE) == WAIT_OBJECT_0
            ? S_OK
            : E_FAIL;
  }

  // Waits for user to approve (or not) access to the microphone and
  // camera.
  if (SUCCEEDED(hr)) {
    event_wait_for_media_capture_async_action_completion =
        Event(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET,
                              WRITE_OWNER | EVENT_ALL_ACCESS));
    hr = event_wait_for_media_capture_async_action_completion.IsValid()
             ? S_OK
             : HRESULT_FROM_WIN32(GetLastError());
  }

  // Waits until user grant (or deny) access to the camera.
  if (SUCCEEDED(hr)) {
    hr = async_action_media_capture_agile.As(&async_action_media_capture);
  }

  if (SUCCEEDED(hr)) {
    hr = async_action_media_capture->put_Completed(
        Callback<IAsyncActionCompletedHandler>(
            [&event_wait_for_media_capture_async_action_completion](
                IAsyncAction*, AsyncStatus async_status) -> HRESULT {
              ::SetEvent(
                  event_wait_for_media_capture_async_action_completion.Get());

              // Checks if user granted permission to access the camera.
              return async_status == AsyncStatus::Completed ? S_OK
                                                            : E_ACCESSDENIED;
            })
            .Get());
  }

  if (SUCCEEDED(hr)) {
    hr = ::WaitForSingleObjectEx(
             event_wait_for_media_capture_async_action_completion.Get(),
             INFINITE, FALSE) == WAIT_OBJECT_0
             ? S_OK
             : E_FAIL;
  }

  if (SUCCEEDED(hr)) {
    *pp_media_capture = media_capture.Detach();
  } else {
    *pp_media_capture = nullptr;
  }

  return hr;
}

HRESULT GetMediaCaptureWithInitSettings(
    _In_ ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings*
        initialization_settings,
    _COM_Outptr_ ::ABI::Windows::Media::Capture::IMediaCapture**
        media_capture) {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && \
    !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  return GetMediaCaptureWithInitSettingsUWP(initialization_settings,
                                            media_capture);
#else
  return GetMediaCaptureWithInitSettingsWin32(initialization_settings,
                                              media_capture);
#endif  // WINAPI_PARTITION_APP && !WINAPI_PARTITION_DESKTOP
}

uint32_t SafelyComputeMediaRatio(_In_ IMediaRatio* ratio_no_ref) {
  uint32_t numerator, denominator, media_ratio = 0;

  HRESULT hr = S_OK;

  if (SUCCEEDED(hr)) {
    hr = ratio_no_ref->get_Numerator(&numerator);
  }

  if (SUCCEEDED(hr)) {
    hr = ratio_no_ref->get_Denominator(&denominator);
  }

  if (SUCCEEDED(hr)) {
    media_ratio = (denominator != 0) ? numerator / denominator : 0;
  }

  return media_ratio;
}

VideoType ToVideoType(_In_ const HString& sub_type) {
  uint32_t cchCount;
  const wchar_t* video_type = sub_type.GetRawBuffer(&cchCount);
  VideoType converted_type = VideoType::kUnknown;

  if (cchCount < 4 || cchCount > 8) {
    return VideoType::kUnknown;
  }

  struct {
    const wchar_t* format;
    const VideoType type;
  } static format_to_type[] = {
      {L"I420", VideoType::kI420},         {L"I420", VideoType::kI420},
      {L"IYUV", VideoType::kIYUV},         {L"RGB24", VideoType::kRGB24},
      {L"ABGR", VideoType::kABGR},         {L"ARGB", VideoType::kARGB},
      {L"ARGB4444", VideoType::kARGB4444}, {L"RGB565", VideoType::kRGB565},
      {L"RGB565", VideoType::kRGB565},     {L"ARGB1555", VideoType::kARGB1555},
      {L"YUY2", VideoType::kYUY2},         {L"YV12", VideoType::kYV12},
      {L"UYVY", VideoType::kUYVY},         {L"MJPEG", VideoType::kMJPEG},
      {L"NV21", VideoType::kNV21},         {L"NV12", VideoType::kNV12},
      {L"BGRA", VideoType::kBGRA},
  };

  for (const auto& entry : format_to_type) {
    if (wcsncmp(entry.format, video_type, cchCount) == 0) {
      converted_type = entry.type;
      break;
    }
  }

  return converted_type;
}

HRESULT GetMainViewDispatcher(
    _COM_Outptr_ ICoreDispatcher** main_view_dispatcher) {
  ComPtr<IActivationFactory> activation_factory;
  ComPtr<ICoreApplication> coreApplication;
  ComPtr<ICoreImmersiveApplication> immersiveApplication;
  ComPtr<ICoreApplicationView> applicationView;
  ComPtr<ICoreWindow> coreWindow;

  HRESULT hr = S_OK;

  if (SUCCEEDED(hr)) {
    hr = GetActivationFactory(
        HStringReference(
            RuntimeClass_Windows_ApplicationModel_Core_CoreApplication)
            .Get(),
        &activation_factory);
  }

  if (SUCCEEDED(hr)) {
    hr = activation_factory.As(&coreApplication);
  }

  if (SUCCEEDED(hr)) {
    hr = coreApplication.As(&immersiveApplication);
  }

  if (SUCCEEDED(hr)) {
    hr = immersiveApplication->get_MainView(&applicationView);
  }

  if (SUCCEEDED(hr)) {
    hr = applicationView->get_CoreWindow(&coreWindow);
  }

  if (SUCCEEDED(hr)) {
    hr = coreWindow->get_Dispatcher(main_view_dispatcher);
  }

  return hr;
}

// Creates the settings that will be used to select the capture device and its
// configuration.
HRESULT CreateMediaCaptureInitializationSettings(
    _In_ const HStringReference& device_id,
    _In_ IMediaCaptureVideoProfile* video_profile,
    _In_ IMediaCaptureVideoProfileMediaDescription* profile_description,
    _COM_Outptr_ IMediaCaptureInitializationSettings**
        pp_initialization_settings) {
  HRESULT hr = S_OK;
  ComPtr<IMediaCaptureInitializationSettings> init_settings;
  ComPtr<IMediaCaptureInitializationSettings4> init_settings4;
  ComPtr<IMediaCaptureInitializationSettings5> init_settings5;

  if (SUCCEEDED(hr)) {
    hr = ActivateInstance(
        HStringReference(
            RuntimeClass_Windows_Media_Capture_MediaCaptureInitializationSettings)
            .Get(),
        &init_settings);
  }

  // There are two ways to specify which configuration from a camera to use.
  // For devices that support video capture profile, video_profile and
  // profile_description should be defined. For cameras that do not support
  // profiles, device_id should be defined. Keep in mind that device_id might be
  // populated for cameras with profile, but its content will not be useful for
  // put_VideoDeviceId (the content looks like a camera id, but it is post fixed
  // by a comma and a profile id).
  if (SUCCEEDED(hr) && !video_profile && !profile_description) {
    unsigned int length;
    device_id.GetRawBuffer(&length);
    if (length > 0) {
      hr = init_settings->put_VideoDeviceId(device_id.Get());
    }
  }

  if (SUCCEEDED(hr)) {
    hr = init_settings->put_StreamingCaptureMode(
        StreamingCaptureMode::StreamingCaptureMode_AudioAndVideo);
  }

  if (SUCCEEDED(hr)) {
    hr = init_settings.As(&init_settings4);
  }

  if (SUCCEEDED(hr) && video_profile) {
    hr = init_settings4->put_VideoProfile(video_profile);
  }

  if (SUCCEEDED(hr) && profile_description) {
    hr = init_settings4->put_RecordMediaDescription(profile_description);
  }

  if (SUCCEEDED(hr)) {
    hr = init_settings.As(&init_settings5);
  }

  if (SUCCEEDED(hr)) {
    hr = init_settings5->put_MemoryPreference(
        MediaCaptureMemoryPreference::MediaCaptureMemoryPreference_Cpu);
  }

  if (SUCCEEDED(hr)) {
    *pp_initialization_settings = init_settings.Detach();
  } else {
    *pp_initialization_settings = nullptr;
  }

  return hr;
}

HRESULT WaitForASyncWithEvent(
    _In_ IAsyncInfo* async_info,
    _In_ std::shared_ptr<HANDLE> event_completed_handle_ptr,
    _In_ DWORD timeout_ms) {
  HRESULT hr = S_OK;
  AsyncStatus async_status;

  if (SUCCEEDED(hr)) {
    hr = async_info ? S_OK : RPC_E_INVALID_PARAMETER;
  }

  // Gets the operation status to check if operation started before waiting.
  if (SUCCEEDED(hr)) {
    hr = async_info->get_Status(&async_status);
  }

  if (SUCCEEDED(hr)) {
    if (async_status == AsyncStatus::Started) {
      DWORD trigger_event = ::WaitForSingleObjectEx(*event_completed_handle_ptr,
                                                    timeout_ms, FALSE);
      if (trigger_event == WAIT_OBJECT_0) {
        hr = S_OK;
      } else if (trigger_event == WAIT_TIMEOUT) {
        HRESULT hr2 = async_info->get_Status(&async_status);
        if (SUCCEEDED(hr2) && (async_status == AsyncStatus::Completed)) {
          // The async operation might be completed before the put_Completed
          // callback triggering event_completed_handle_ptr have chance to be
          // defined. In that case, the event timesout, but the async
          // operation might be successfully completed.
          RTC_LOG(LS_WARNING)
              << "Wait operation timedout, but async operation completed.";
          hr = S_OK;
        } else {
          RTC_LOG(LS_ERROR)
              << "Wait operation timedout. It took longer than " << timeout_ms
              << " ms. hr2: " << hr2 << " Async Status: " << async_status;
          hr = RPC_E_TIMEOUT;
        }
      } else {
        RTC_LOG(LS_ERROR) << "Wait operation did not succeeded. Error: "
                          << trigger_event << " " << GetLastError();
        hr = E_FAIL;
      }
    } else if (async_status == AsyncStatus::Completed) {
      RTC_LOG(LS_WARNING)
          << "Wait operation didn't wait because async operation "
             "has been completed already.";
      hr = S_OK;
    } else {
      RTC_LOG(LS_ERROR) << "Something happened to the async operation. Error: "
                        << async_status;
      hr = E_FAIL;
    }
  }

  return hr;
}

HRESULT WaitForAsyncAction(
    _In_ ::ABI::Windows::Foundation::IAsyncAction* p_async_action,
    _In_ DWORD timeout_ms) {
  HRESULT hr = S_OK;
  HRESULT hr_async_error = S_OK;
  ComPtr<IAsyncAction> async_action(p_async_action);
  ComPtr<IAsyncInfo> async_info;
  std::shared_ptr<HANDLE> event_completed_handle_ptr = NULL;

  APTTYPE apt_type;
  APTTYPEQUALIFIER apt_qualifier;
  hr = CoGetApartmentType(&apt_type, &apt_qualifier);
  // Please make the caller of this API run on a MTA appartment type.
  // The caller shouldn't be running on the UI thread (STA).
  if (SUCCEEDED(hr) && (apt_type != APTTYPE_MTA)) {
    RTC_LOG(LS_ERROR) << "Waiting in a non-MTA thread. Deadlocks might occur.";
  }

  // IAsyncInfo::get_Status is needed to check if operation started and if any
  // errors happened.
  if (SUCCEEDED(hr)) {
    hr = async_action.As(&async_info);
  }

  // Creates the Event to be used to block and suspend until the async
  // operation finishes.
  if (SUCCEEDED(hr)) {
    // Using raw HANDLE because WRL::Wrappers::Event doesn't always work across
    // threads and there is no agile option.
    HANDLE event_completed_handle =
        ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET,
                        WRITE_OWNER | EVENT_ALL_ACCESS);
    if (event_completed_handle) {
      hr = S_OK;
      event_completed_handle_ptr = std::shared_ptr<HANDLE>(
          new HANDLE{event_completed_handle}, [](HANDLE* h) {
            ::CloseHandle(*h);
            delete h;
          });
    } else {
      hr = HRESULT_FROM_WIN32(GetLastError());
    }
  }

  if (SUCCEEDED(hr)) {
    hr = GetLastError() == ERROR_ALREADY_EXISTS
             ? HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)
             : S_OK;
  }

  // Defines the callback that will signal the event to unblock and resume.
  if (SUCCEEDED(hr)) {
    hr = async_action->put_Completed(
        Callback<IAsyncActionCompletedHandler>([event_completed_handle_ptr](
                                                   IAsyncAction*,
                                                   AsyncStatus async_status)
                                                   -> HRESULT {
          ::SetEvent(*event_completed_handle_ptr);

          return async_status == AsyncStatus::Completed ? S_OK : E_ABORT;
        }).Get());
  }

  // Block and suspend thread until the async operation finishes or timeout.
  if (SUCCEEDED(hr)) {
    hr = WaitForASyncWithEvent(async_info.Get(), event_completed_handle_ptr,
                               timeout_ms);
  }

  // Checks if async operation completed successfully.
  if (SUCCEEDED(hr)) {
    hr = async_info->get_ErrorCode(&hr_async_error);
  }

  if (SUCCEEDED(hr)) {
    hr = hr_async_error;
  }

  return hr;
}  // namespace videocapturemodule

}  // namespace videocapturemodule
}  // namespace webrtc
