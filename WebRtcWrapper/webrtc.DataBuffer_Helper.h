// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "webrtc.DataBuffer_Helper.g.h"


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    struct DataBuffer_Helper : DataBuffer_HelperT<DataBuffer_Helper>
    {
      DataBuffer_Helper(::webrtc::DataBuffer data_buffer_helper);

      ::webrtc::DataBuffer get_webrtc_data_buffer_helper() const;

      private:
      ::webrtc::DataBuffer data_buffer_helper_;

    };
}
