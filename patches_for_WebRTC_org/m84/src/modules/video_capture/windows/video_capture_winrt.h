/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_WINDOWS_VIDEO_CAPTURE_WINRT_H_
#define MODULES_VIDEO_CAPTURE_WINDOWS_VIDEO_CAPTURE_WINRT_H_

#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture_impl.h"

namespace webrtc {
namespace videocapturemodule {

class VideoCaptureWinRT : public VideoCaptureImpl {
 public:
  VideoCaptureWinRT();

  int32_t Init(const char* device_unique_id_UTF8);

  //
  //  Start/Stop
  //
  int32_t StartCapture(const VideoCaptureCapability& capability) override;
  int32_t StopCapture() override;

  //
  //  Properties of the set device
  //
  bool CaptureStarted() override;
  int32_t CaptureSettings(VideoCaptureCapability& settings) override;

 protected:
  ~VideoCaptureWinRT() override;

 private:
  int32_t SetDeviceUniqueId(const char* device_unique_id_UTF8);
  void* video_capture_internal_;
};

}  // namespace videocapturemodule
}  // namespace webrtc
#endif  // MODULES_VIDEO_CAPTURE_WINDOWS_VIDEO_CAPTURE_WINRT_H_
