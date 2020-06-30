/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef THIRD_PARTY_H264_WINUWP_H264ENCODER_IH264ENCODINGCALLBACK_H_
#define THIRD_PARTY_H264_WINUWP_H264ENCODER_IH264ENCODINGCALLBACK_H_

#include <wrl/implements.h>


namespace webrtc {

interface IH264EncodingCallback {
    virtual void OnH264Encoded(Microsoft::WRL::ComPtr<IMFSample> sample) = 0;
};

}  // namespace webrtc

#endif  // THIRD_PARTY_H264_WINUWP_H264ENCODER_IH264ENCODINGCALLBACK_H_

