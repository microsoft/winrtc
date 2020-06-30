/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_WINDOWS_DEVICE_INFO_WINRT_H_
#define MODULES_VIDEO_CAPTURE_WINDOWS_DEVICE_INFO_WINRT_H_

#include "modules/video_capture/device_info_impl.h"

namespace webrtc {
namespace videocapturemodule {

class DeviceInfoWinRT : public DeviceInfoImpl {
 public:
  // Factory function.
  static DeviceInfoWinRT* Create();

  ~DeviceInfoWinRT() override;

  uint32_t NumberOfDevices() override;

  //
  // Returns the available capture devices.
  //
  int32_t GetDeviceName(uint32_t device_number,
                        char* device_name_utf8,
                        uint32_t device_name_length,
                        char* device_unique_id_utf8,
                        uint32_t device_unique_id_utf8_length,
                        char* product_unique_id_utf8,
                        uint32_t product_unique_id_utf8_length) override;

  //
  // Display OS /capture device specific settings dialog
  //
  int32_t DisplayCaptureSettingsDialogBox(const char* device_unique_id_utf8,
                                          const char* dialog_title_utf8,
                                          void* parent_window,
                                          uint32_t position_x,
                                          uint32_t position_y) override;

  int32_t CreateCapabilityMap(const char* device_unique_id_utf8) override;

 private:
  DeviceInfoWinRT();
  int32_t Init() override;

  void* device_info_internal_;
};
}  // namespace videocapturemodule
}  // namespace webrtc
#endif  // MODULES_VIDEO_CAPTURE_WINDOWS_DEVICE_INFO_WINRT_H_
