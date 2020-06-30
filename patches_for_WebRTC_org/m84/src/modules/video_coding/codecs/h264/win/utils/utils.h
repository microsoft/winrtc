/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_UTILS_H_
#define MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_UTILS_H_

#ifdef _DEBUG
#define ON_SUCCEEDED(act)                      \
  if (SUCCEEDED(hr)) {                         \
    hr = (act);                                \
    if (FAILED(hr)) {                          \
      RTC_LOG(LS_WARNING) << "ERROR:" << #act; \
      __debugbreak();                          \
    }                                          \
  }
#else
#define ON_SUCCEEDED(act)                      \
  if (SUCCEEDED(hr)) {                         \
    hr = (act);                                \
    if (FAILED(hr)) {                          \
      RTC_LOG(LS_WARNING) << "ERROR:" << #act; \
    }                                          \
  }
#endif

#endif  // MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_UTILS_H_
