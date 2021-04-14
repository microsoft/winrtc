/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef THIRD_PARTY_H264_WINUWP_H264ENCODER_H264MEDIASINK_H_
#define THIRD_PARTY_H264_WINUWP_H264ENCODER_H264MEDIASINK_H_

#include <Mferror.h>
#include <mfidl.h>
#include <windows.foundation.h>
#include <windows.media.h>
#include <windows.media.mediaproperties.h>
#include <wrl.h>

#include "../Utils/crit_sec.h"
#include "ih264_encoding_callback.h"
#include "h264_stream_sink.h"

namespace webrtc {

class H264StreamSink;
class H264MediaSink : public Microsoft::WRL::RuntimeClass<
  Microsoft::WRL::RuntimeClassFlags<
  Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
  ABI::Windows::Media::IMediaExtension,
  Microsoft::WRL::FtmBase,
  IMFMediaSink,
  IMFClockStateSink> {
  InspectableClass(L"H264MediaSink", BaseTrust)

 public:
  H264MediaSink();
  virtual ~H264MediaSink();

  HRESULT RuntimeClassInitialize();

  // IMediaExtension
  IFACEMETHOD(SetProperties)
    (ABI::Windows::Foundation::Collections::IPropertySet
      *pConfiguration) {
    return S_OK;
  }

  // IMFMediaSink methods
  IFACEMETHOD(GetCharacteristics) (DWORD *pdwCharacteristics);

  IFACEMETHOD(AddStreamSink)(
    /* [in] */ DWORD dwStreamSinkIdentifier,
    /* [in] */ IMFMediaType *pMediaType,
    /* [out] */ IMFStreamSink **ppStreamSink);

  IFACEMETHOD(RemoveStreamSink) (DWORD dwStreamSinkIdentifier);
  IFACEMETHOD(GetStreamSinkCount) (_Out_ DWORD *pcStreamSinkCount);
  IFACEMETHOD(GetStreamSinkByIndex)
    (DWORD dwIndex, _Outptr_ IMFStreamSink **ppStreamSink);
  IFACEMETHOD(GetStreamSinkById)
    (DWORD dwIdentifier, IMFStreamSink **ppStreamSink);
  IFACEMETHOD(SetPresentationClock)
    (IMFPresentationClock *pPresentationClock);
  IFACEMETHOD(GetPresentationClock)
    (IMFPresentationClock **ppPresentationClock);
  IFACEMETHOD(Shutdown) ();

  // IMFClockStateSink methods
  IFACEMETHOD(OnClockStart)
    (MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
  IFACEMETHOD(OnClockStop) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockPause) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockRestart) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockSetRate) (MFTIME hnsSystemTime, float flRate);

  HRESULT RegisterEncodingCallback(IH264EncodingCallback *callback);

 private:
  void HandleError(HRESULT hr);

  HRESULT CheckShutdown() const {
    if (isShutdown_) {
      return MF_E_SHUTDOWN;
    } else {
      return S_OK;
    }
  }

 private:
  CritSec critSec_;

  bool isShutdown_;

  Microsoft::WRL::ComPtr<IMFPresentationClock> spClock_;
  Microsoft::WRL::ComPtr<H264StreamSink> outputStream_;
};

}  // namespace webrtc

#endif  // THIRD_PARTY_H264_WINUWP_H264ENCODER_H264MEDIASINK_H_
