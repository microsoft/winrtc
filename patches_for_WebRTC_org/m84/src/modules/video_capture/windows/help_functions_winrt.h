/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_WINDOWS_HELP_FUNCTIONS_WINRT_H_
#define MODULES_VIDEO_CAPTURE_WINDOWS_HELP_FUNCTIONS_WINRT_H_

#include <windows.foundation.h>
#include <windows.media.mediaproperties.h>
#include <windows.ui.core.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/wrappers/corewrappers.h>

#include <cassert>
#include <memory>

#include "modules/video_capture/video_capture_defines.h"
#include "rtc_base/logging.h"

namespace webrtc {
namespace videocapturemodule {

uint32_t SafelyComputeMediaRatio(
    _In_ ::ABI::Windows::Media::MediaProperties::IMediaRatio* ratio_no_ref);

VideoType ToVideoType(_In_ const ::Microsoft::WRL::Wrappers::HString& sub_type);

HRESULT GetMainViewDispatcher(
    _COM_Outptr_ ::ABI::Windows::UI::Core::ICoreDispatcher** dispatcher);

HRESULT CreateMediaCaptureInitializationSettings(
    _In_ const ::Microsoft::WRL::Wrappers::HStringReference& device_id,
    _In_ ::ABI::Windows::Media::Capture::IMediaCaptureVideoProfile*
        media_profile,
    _In_ ::ABI::Windows::Media::Capture::
        IMediaCaptureVideoProfileMediaDescription* profile_description,
    _COM_Outptr_ ::ABI::Windows::Media::Capture::
        IMediaCaptureInitializationSettings** pp_initialization_settings);

HRESULT GetMediaCaptureWithInitSettings(
    _In_ ::ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings*
        initialization_settings,
    _COM_Outptr_ ::ABI::Windows::Media::Capture::IMediaCapture** media_capture);

HRESULT WaitForASyncWithEvent(
    _In_ IAsyncInfo* async_info,
    _In_ std::shared_ptr<HANDLE> event_completed_handle_ptr,
    _In_ DWORD timeout_ms);

HRESULT WaitForAsyncAction(
    _In_ ::ABI::Windows::Foundation::IAsyncAction* async_action,
    _In_ DWORD timeout_ms = 5000);

template <typename T>
HRESULT WaitForAsyncOperation(
    _In_ ::ABI::Windows::Foundation::IAsyncOperation<T>* p_async_op) {
  using ::ABI::Windows::Foundation::AsyncStatus;
  using ::ABI::Windows::Foundation::IAsyncInfo;
  using ::ABI::Windows::Foundation::IAsyncOperation;
  using ::ABI::Windows::Foundation::IAsyncOperationCompletedHandler;
  using ::Microsoft::WRL::Callback;
  using ::Microsoft::WRL::Wrappers::Event;

  HRESULT hr = S_OK;
  HRESULT hr_async_error = S_OK;
  const DWORD timeout_ms = 180000;
  ComPtr<IAsyncOperation<T>> async_op(p_async_op);
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
    hr = async_op.template As<IAsyncInfo>(&async_info);
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

  if (SUCCEEDED(hr)) {
    // Defines the callback that will signal the event to unblock and resume.
    hr = async_op->put_Completed(
        Callback<IAsyncOperationCompletedHandler<T>>(
            [event_completed_handle_ptr](IAsyncOperation<T>*,
                                         AsyncStatus async_status) -> HRESULT {
              ::SetEvent(*event_completed_handle_ptr);

              return async_status == Completed ? S_OK : E_ABORT;
            })
            .Get());
  }

  // Block and suspend thread until the async operation finishes or timeout.
  if (SUCCEEDED(hr)) {
    hr = WaitForASyncWithEvent(async_info.Get(), event_completed_handle_ptr,
                               timeout_ms);
  }

  if (SUCCEEDED(hr)) {
    hr = async_info->get_ErrorCode(&hr_async_error);
  }

  if (SUCCEEDED(hr)) {
    hr = hr_async_error;
  }

  return hr;
}

}  // namespace videocapturemodule
}  // namespace webrtc

#endif  // MODULES_VIDEO_CAPTURE_WINDOWS_HELP_FUNCTIONS_WINRT_H_
