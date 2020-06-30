/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "modules/video_coding/codecs/h264/win/encoder/h264_stream_sink.h"

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include "../utils/utils.h"
#include "rtc_base/logging.h"

namespace webrtc {

class H264MediaSink;

H264StreamSink::H264StreamSink()
  : dwIdentifier_((DWORD)-1)
  , state_(State_TypeNotSet)
  , isShutdown_(false)
  , workQueueId_(0)
  , workQueueCB_(this, &H264StreamSink::OnDispatchWorkItem) {
}

H264StreamSink::~H264StreamSink() {
  OutputDebugString(L"H264StreamSink::~H264StreamSink()\r\n");
}

HRESULT H264StreamSink::RuntimeClassInitialize(
  DWORD dwIdentifier, H264MediaSink *pParent) {
  HRESULT hr = S_OK;

  hr = MFCreateEventQueue(&spEventQueue_);

  // Allocate a new work queue for async operations.
  if (SUCCEEDED(hr)) {
    hr = MFAllocateSerialWorkQueue(MFASYNC_CALLBACK_QUEUE_STANDARD,
      &workQueueId_);
  }

  if (SUCCEEDED(hr)) {
    spSink_ = reinterpret_cast<IMFMediaSink*>(pParent);
    dwIdentifier_ = dwIdentifier;
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::BeginGetEvent(
  IMFAsyncCallback *pCallback, IUnknown *punkState) {
  HRESULT hr = S_OK;

  AutoLock lock(critSec_);

  hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = spEventQueue_->BeginGetEvent(pCallback, punkState);
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::EndGetEvent(
  IMFAsyncResult *pResult, IMFMediaEvent **ppEvent) {
  HRESULT hr = S_OK;

  AutoLock lock(critSec_);

  hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = spEventQueue_->EndGetEvent(pResult, ppEvent);
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetEvent(
  DWORD dwFlags, IMFMediaEvent **ppEvent) {
  // NOTE:
  // GetEvent can block indefinitely, so we don't hold the lock.
  // This requires some juggling with the event queue pointer.

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<IMFMediaEventQueue> spQueue;
  {
    AutoLock lock(critSec_);

    hr = CheckShutdown();

    if (SUCCEEDED(hr)) {
      spQueue = spEventQueue_;
    }
  }

  if (SUCCEEDED(hr)) {
    hr = spQueue->GetEvent(dwFlags, ppEvent);
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::QueueEvent(
  MediaEventType met, REFGUID guidExtendedType,
  HRESULT hrStatus, PROPVARIANT const *pvValue) {
  HRESULT hr = S_OK;

  AutoLock lock(critSec_);

  hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = spEventQueue_->QueueEventParamVar(met, guidExtendedType,
      hrStatus, pvValue);
  }

  return hr;
}

/// IMFStreamSink methods
IFACEMETHODIMP H264StreamSink::GetMediaSink(IMFMediaSink **ppMediaSink) {
  if (ppMediaSink == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    *ppMediaSink = spSink_.Get();
    (*ppMediaSink)->AddRef();
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetIdentifier(DWORD *pdwIdentifier) {
  if (pdwIdentifier == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    *pdwIdentifier = dwIdentifier_;
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetMediaTypeHandler(
  IMFMediaTypeHandler **ppHandler) {
  if (ppHandler == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  // This stream object acts as its own type handler, so we QI ourselves.
  if (SUCCEEDED(hr)) {
    hr = QueryInterface(IID_IMFMediaTypeHandler, (void**)ppHandler);
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::ProcessSample(IMFSample *pSample) {
  if (pSample == nullptr) {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  AutoLock lock(critSec_);

  hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = ValidateOperation(OpProcessSample);
  }

  if (SUCCEEDED(hr)) {
    sampleQueue_.push_back(pSample);

    hr = QueueAsyncOperation(OpProcessSample);
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::PlaceMarker(
  MFSTREAMSINK_MARKER_TYPE eMarkerType,
  const PROPVARIANT *pvarMarkerValue,
  const PROPVARIANT *pvarContextValue) {
  AutoLock lock(critSec_);
  HRESULT hr = S_OK;
  hr = CheckShutdown();
  if (SUCCEEDED(hr)) {
    hr = QueueAsyncOperation(OpPlaceMarker, pvarContextValue);
  }
  return hr;
}

IFACEMETHODIMP H264StreamSink::Flush() {
  AutoLock lock(critSec_);
  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    DropSamplesFromQueue();
  }

  return hr;
}

/// IMFMediaTypeHandler methods

IFACEMETHODIMP H264StreamSink::IsMediaTypeSupported(
  /* [in] */ IMFMediaType *pMediaType,
  /* [out] */ IMFMediaType **ppMediaType) {
  if (pMediaType == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  GUID majorType = GUID_NULL;

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
  }

  if (SUCCEEDED(hr)) {
    if (majorType != MFMediaType_Video && majorType != MFMediaType_Audio) {
      hr = MF_E_INVALIDTYPE;
    }
  }

  // Don't support changing subtype.
  if (SUCCEEDED(hr) && spCurrentType_ != nullptr) {
    GUID guiNewSubtype;
    if (FAILED(pMediaType->GetGUID(MF_MT_SUBTYPE, &guiNewSubtype)) ||
      guiNewSubtype != guidCurrentSubtype_) {
      hr = MF_E_INVALIDTYPE;
    }
  }

  if (ppMediaType) {
    *ppMediaType = nullptr;
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetMediaTypeCount(DWORD *pdwTypeCount) {
  if (pdwTypeCount == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    *pdwTypeCount = 1;
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetMediaTypeByIndex(
  /* [in] */ DWORD dwIndex,
  /* [out] */ IMFMediaType **ppType) {
  if (ppType == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (dwIndex > 0) {
    hr = MF_E_NO_MORE_TYPES;
  } else {
    *ppType = spCurrentType_.Get();
    if (*ppType != nullptr) {
      (*ppType)->AddRef();
    }
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::SetCurrentMediaType(
  IMFMediaType *pMediaType) {
  HRESULT hr = S_OK;
  if (pMediaType == nullptr) {
    hr = E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  ON_SUCCEEDED(CheckShutdown());

  ON_SUCCEEDED(ValidateOperation(OpSetMediaType));

  // We set media type already
  if (state_ >= State_Ready) {
    ON_SUCCEEDED(IsMediaTypeSupported(pMediaType, nullptr));
  }

  ON_SUCCEEDED(MFCreateMediaType(spCurrentType_.ReleaseAndGetAddressOf()));
  ON_SUCCEEDED(pMediaType->CopyAllItems(spCurrentType_.Get()));
  ON_SUCCEEDED(spCurrentType_->GetGUID(MF_MT_SUBTYPE, &guidCurrentSubtype_));
  if (SUCCEEDED(hr)) {
    if (state_ < State_Ready) {
      state_ = State_Ready;
    } else if (state_ > State_Ready) {
      if (SUCCEEDED(hr)) {
        ProcessFormatChange();
      }
    }
  }
  return hr;
}

IFACEMETHODIMP H264StreamSink::GetCurrentMediaType(
  IMFMediaType **ppMediaType) {
  if (ppMediaType == nullptr) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    if (spCurrentType_ == nullptr) {
      hr = MF_E_NOT_INITIALIZED;
    }
  }

  if (SUCCEEDED(hr)) {
    *ppMediaType = spCurrentType_.Get();
    (*ppMediaType)->AddRef();
  }

  return hr;
}

IFACEMETHODIMP H264StreamSink::GetMajorType(GUID *pguidMajorType) {
  if (pguidMajorType == nullptr) {
    return E_INVALIDARG;
  }

  if (!spCurrentType_) {
    return MF_E_NOT_INITIALIZED;
  }

  *pguidMajorType = MFMediaType_Video;

  return S_OK;
}

HRESULT H264StreamSink::Start(MFTIME start) {
  AutoLock lock(critSec_);

  HRESULT hr = S_OK;

  hr = ValidateOperation(OpStart);

  if (SUCCEEDED(hr)) {
    state_ = State_Started;
    hr = QueueAsyncOperation(OpStart);
  }

  return hr;
}

HRESULT H264StreamSink::Stop() {
  AutoLock lock(critSec_);

  HRESULT hr = S_OK;

  hr = ValidateOperation(OpStop);

  if (SUCCEEDED(hr)) {
    state_ = State_Stopped;
    hr = QueueAsyncOperation(OpStop);
  }

  return hr;
}

BOOL H264StreamSink::ValidStateMatrix
  [State::State_Count][StreamOperation::Op_Count] = {
    // States:    Operations:
    //            SetType Start Stop Sample
    /* NotSet */  {TRUE, FALSE, FALSE, FALSE},
    /* Ready */   {TRUE,  TRUE,  TRUE, FALSE},
    /* Start */   {TRUE,  TRUE,  TRUE,  TRUE},
    /* Stop */    {TRUE,  TRUE,  TRUE, FALSE}
};

HRESULT H264StreamSink::ValidateOperation(StreamOperation op) {
  if (ValidStateMatrix[state_][op]) {
    return S_OK;
  } else if (state_ == State_TypeNotSet) {
    return MF_E_NOT_INITIALIZED;
  } else {
    return MF_E_INVALIDREQUEST;
  }
}

HRESULT H264StreamSink::Shutdown() {
  AutoLock lock(critSec_);

  if (!isShutdown_) {
    if (spEventQueue_) {
      spEventQueue_->Shutdown();
    }

    MFUnlockWorkQueue(workQueueId_);

    sampleQueue_.clear();

    spSink_.Reset();
    spEventQueue_.Reset();
    spCurrentType_.Reset();

    encodingCallback_ = nullptr;

    isShutdown_ = true;
  }

  return S_OK;
}

HRESULT H264StreamSink::QueueAsyncOperation(StreamOperation op, const PROPVARIANT* propVariant) {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IUnknown> spOp;
  hr = Microsoft::WRL::MakeAndInitialize<AsyncStreamSinkOperation>(&spOp, op, propVariant);

  if (SUCCEEDED(hr)) {
    hr = MFPutWorkItem2(workQueueId_, 0, &workQueueCB_, spOp.Get());
  }

  return hr;
}

HRESULT H264StreamSink::OnDispatchWorkItem(IMFAsyncResult *pAsyncResult) {
  HRESULT hr = S_OK;
  auto sample = ComPtr<IMFSample>();

  // Scope the AutoLock
  {
    AutoLock lock(critSec_);
    Microsoft::WRL::ComPtr<IUnknown> spState;

    hr = pAsyncResult->GetState(&spState);

    if (SUCCEEDED(hr)) {
      ComPtr<IAsyncStreamSinkOperation> pOp;
      spState.As(&pOp);
      StreamOperation op;
      pOp->GetOp(&op);

      switch (op) {
      case OpStart:
        hr = QueueEvent(MEStreamSinkStarted, GUID_NULL, S_OK, nullptr);

        if (SUCCEEDED(hr)) {
          sample = ProcessSamplesFromQueue();
          if (sample == nullptr) {
            hr = QueueEvent(MEStreamSinkRequestSample,
              GUID_NULL, S_OK, nullptr);
          }
        }
        break;

      case OpStop:
        DropSamplesFromQueue();

        hr = QueueEvent(MEStreamSinkStopped, GUID_NULL, S_OK, nullptr);
        break;

      case OpProcessSample:
        hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, S_OK, nullptr);
        if (SUCCEEDED(hr)) {
          sample = ProcessSamplesFromQueue();
        }
        break;

      case OpPlaceMarker: {
          PROPVARIANT propVariant;
          PropVariantInit(&propVariant);
          if (SUCCEEDED(pOp->GetPropVariant(&propVariant))) {
            hr = QueueEvent(MEStreamSinkMarker, GUID_NULL, S_OK, &propVariant);
          }
          break;
        }

      case OpSetMediaType:
        hr = QueueEvent(MEStreamSinkFormatChanged, GUID_NULL, S_OK, nullptr);
        break;

      case Op_Count: break;
      }
    }
  }

  if (SUCCEEDED(hr) && sample != nullptr) {
    AutoLock lock(cbCritSec_);
    if (encodingCallback_ != nullptr) {
      encodingCallback_->OnH264Encoded(sample);
    }
  }

  return hr;
}

bool H264StreamSink::DropSamplesFromQueue() {
  sampleQueue_.clear();

  return true;
}

ComPtr<IMFSample> H264StreamSink::ProcessSamplesFromQueue() {
  Microsoft::WRL::ComPtr<IUnknown> spunkSample;
  auto sample = ComPtr<IMFSample>();

  if (!sampleQueue_.empty()) {
    spunkSample = sampleQueue_.front();
    sampleQueue_.pop_front();
    spunkSample.As<IMFSample>(&sample);
  }

  return sample;
}

void H264StreamSink::ProcessFormatChange() {
  HRESULT hr = S_OK;

  hr = QueueAsyncOperation(OpSetMediaType);
}

void H264StreamSink::HandleError(HRESULT hr) {
  if (!isShutdown_) {
    QueueEvent(MEError, GUID_NULL, hr, nullptr);
  }
}


HRESULT AsyncStreamSinkOperation::RuntimeClassInitialize(
  StreamOperation op, const PROPVARIANT* propVariant) {
  HRESULT hr = S_OK;
  m_op = op;
  if (propVariant != nullptr) {
    PropVariantCopy(&m_propVariant, propVariant);
  }
  else {
    PropVariantInit(&m_propVariant);
  }
  return hr;
}

AsyncStreamSinkOperation::~AsyncStreamSinkOperation() {
}

HRESULT AsyncStreamSinkOperation::GetOp(StreamOperation* op) {
  if (op == nullptr)
    return E_INVALIDARG;
  *op = m_op;
  return S_OK;
}

HRESULT AsyncStreamSinkOperation::GetPropVariant(PROPVARIANT* propVariant) {
  if (propVariant == nullptr)
    return E_INVALIDARG;
  PropVariantCopy(propVariant, &m_propVariant);
  return S_OK;
}

HRESULT H264StreamSink::RegisterEncodingCallback(
  IH264EncodingCallback *callback) {
  AutoLock lock(cbCritSec_);
  encodingCallback_ = callback;
  return S_OK;
}

}  // namespace webrtc
