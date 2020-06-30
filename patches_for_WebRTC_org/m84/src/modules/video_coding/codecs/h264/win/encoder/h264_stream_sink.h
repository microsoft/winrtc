/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef THIRD_PARTY_H264_WINUWP_H264ENCODER_H264STREAMSINK_H_
#define THIRD_PARTY_H264_WINUWP_H264ENCODER_H264STREAMSINK_H_

#include <mfidl.h>
#include <Mferror.h>
#include <list>

#include "../Utils/async.h"
#include "../Utils/crit_sec.h"
#include "ih264_encoding_callback.h"

using Microsoft::WRL::ComPtr;

namespace webrtc {

enum State {
  State_TypeNotSet = 0,    // No media type is set
  State_Ready,             // Media type is set, Start has never been called.
  State_Started,
  State_Stopped,
  State_Count              // Number of states
};

enum StreamOperation {
  OpSetMediaType = 0,
  OpStart,
  OpStop,
  OpProcessSample,
  OpPlaceMarker,
  Op_Count
};

class H264MediaSink;

class DECLSPEC_UUID("4b35435f-44ae-44a0-9ba0-b84f9f4a9c19")
  IAsyncStreamSinkOperation : public IUnknown {
 public:
  STDMETHOD(GetOp)(StreamOperation* op) PURE;
  STDMETHOD(GetPropVariant)(PROPVARIANT* propVariant) PURE;
};

class DECLSPEC_UUID("0c89c2e1-79bb-4ad7-a34f-cc006225f8e1")
  AsyncStreamSinkOperation
  : public Microsoft::WRL::RuntimeClass<
  Microsoft::WRL::RuntimeClassFlags<
  Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
  IAsyncStreamSinkOperation> {
  InspectableClass(L"AsyncStreamSinkOperation", BaseTrust)
 public:
  HRESULT RuntimeClassInitialize(StreamOperation op, const PROPVARIANT* propVariant);
  virtual ~AsyncStreamSinkOperation();

  IFACEMETHOD(GetOp) (StreamOperation* op);
  IFACEMETHOD(GetPropVariant)(PROPVARIANT* propVariant);

 private:
  PROPVARIANT m_propVariant;
  StreamOperation m_op;
};

class H264StreamSink : public Microsoft::WRL::RuntimeClass<
  Microsoft::WRL::RuntimeClassFlags<
    Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
  IMFStreamSink,
  IMFMediaEventGenerator,
  IMFMediaTypeHandler> {
  InspectableClass(L"H264StreamSink", BaseTrust)

 public:
  HRESULT RuntimeClassInitialize(DWORD dwIdentifier, H264MediaSink *pParent);

  // IMFMediaEventGenerator
  IFACEMETHOD(BeginGetEvent)(IMFAsyncCallback *pCallback, IUnknown *punkState);
  IFACEMETHOD(EndGetEvent) (IMFAsyncResult *pResult, IMFMediaEvent **ppEvent);
  IFACEMETHOD(GetEvent) (DWORD dwFlags, IMFMediaEvent **ppEvent);
  IFACEMETHOD(QueueEvent) (MediaEventType met, REFGUID guidExtendedType,
    HRESULT hrStatus, PROPVARIANT const *pvValue);

  // IMFStreamSink
  IFACEMETHOD(GetMediaSink) (IMFMediaSink **ppMediaSink);
  IFACEMETHOD(GetIdentifier) (DWORD *pdwIdentifier);
  IFACEMETHOD(GetMediaTypeHandler) (IMFMediaTypeHandler **ppHandler);
  IFACEMETHOD(ProcessSample) (IMFSample *pSample);

  IFACEMETHOD(PlaceMarker) (
    /* [in] */ MFSTREAMSINK_MARKER_TYPE eMarkerType,
    /* [in] */ PROPVARIANT const *pvarMarkerValue,
    /* [in] */ PROPVARIANT const *pvarContextValue);

  IFACEMETHOD(Flush)();

  // IMFMediaTypeHandler
  IFACEMETHOD(IsMediaTypeSupported) (IMFMediaType *pMediaType,
    IMFMediaType **ppMediaType);
  IFACEMETHOD(GetMediaTypeCount) (DWORD *pdwTypeCount);
  IFACEMETHOD(GetMediaTypeByIndex) (DWORD dwIndex, IMFMediaType **ppType);
  IFACEMETHOD(SetCurrentMediaType) (IMFMediaType *pMediaType);
  IFACEMETHOD(GetCurrentMediaType) (IMFMediaType **ppMediaType);
  IFACEMETHOD(GetMajorType) (GUID *pguidMajorType);

  // ValidStateMatrix: Defines a look-up table that says which operations
  // are valid from which states.
  static BOOL ValidStateMatrix[State_Count][Op_Count];

  HRESULT RegisterEncodingCallback(IH264EncodingCallback *callback);

  H264StreamSink();
  virtual ~H264StreamSink();

  HRESULT CheckShutdown() const {
    if (isShutdown_) {
      return MF_E_SHUTDOWN;
    } else {
      return S_OK;
    }
  }


  HRESULT     Start(MFTIME start);
  HRESULT     Stop();
  HRESULT     Shutdown();

 private:
  HRESULT     ValidateOperation(StreamOperation op);

  HRESULT     QueueAsyncOperation(StreamOperation op, const PROPVARIANT* propVariant = nullptr);

  HRESULT     OnDispatchWorkItem(IMFAsyncResult *pAsyncResult);

  bool        DropSamplesFromQueue();
  ComPtr<IMFSample> ProcessSamplesFromQueue();
  void        ProcessFormatChange();

  void        HandleError(HRESULT hr);

 private:
  CritSec                     critSec_;
  CritSec                     cbCritSec_;

  DWORD                       dwIdentifier_;
  State                       state_;
  bool                        isShutdown_;
  GUID                        guidCurrentSubtype_;

  DWORD                       workQueueId_;

  ComPtr<IMFMediaSink>        spSink_;
  ComPtr<IMFMediaEventQueue>  spEventQueue_;
  ComPtr<IMFMediaType>        spCurrentType_;

  std::list<ComPtr<IUnknown>> sampleQueue_;

  AsyncCallback<H264StreamSink>               workQueueCB_;

  IH264EncodingCallback*                      encodingCallback_;
};

}  // namespace webrtc

#endif  // THIRD_PARTY_H264_WINUWP_H264ENCODER_H264STREAMSINK_H_
