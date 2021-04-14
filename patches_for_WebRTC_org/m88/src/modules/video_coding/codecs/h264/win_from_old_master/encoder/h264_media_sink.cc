/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "modules/video_coding/codecs/h264/win/encoder/h264_media_sink.h"
#include <wrl.h>
#include <wrl/implements.h>

namespace webrtc {

H264MediaSink::H264MediaSink()
  : isShutdown_(false) {
}


H264MediaSink::~H264MediaSink() {
  OutputDebugString(L"H264MediaSink::~H264MediaSink()\r\n");
}

HRESULT H264MediaSink::RuntimeClassInitialize() {
  return S_OK;
}

IFACEMETHODIMP H264MediaSink::GetCharacteristics(DWORD *pdwCharacteristics) {
  if (pdwCharacteristics == NULL) {
    return E_INVALIDARG;
  }
  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    // Rateless sink.
    *pdwCharacteristics = MEDIASINK_RATELESS;
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::AddStreamSink(
  DWORD dwStreamSinkIdentifier,
  IMFMediaType *pMediaType,
  IMFStreamSink **ppStreamSink) {
  AutoLock lock(critSec_);
  HRESULT hr = CheckShutdown();

  if (outputStream_ != nullptr) {
    hr = MF_E_STREAMSINK_EXISTS;
  }

  if (SUCCEEDED(hr)) {
    hr = Microsoft::WRL::MakeAndInitialize<H264StreamSink>(
      &outputStream_, dwStreamSinkIdentifier, this);
  }

  if (SUCCEEDED(hr) && pMediaType != nullptr) {
    hr = outputStream_->SetCurrentMediaType(pMediaType);
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::RemoveStreamSink(DWORD dwStreamSinkIdentifier) {
  AutoLock lock(critSec_);
  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    if (outputStream_ == nullptr) {
      hr = E_INVALIDARG;
    }
  }

  if (SUCCEEDED(hr)) {
    DWORD currentSinkId;
    hr = outputStream_->GetIdentifier(&currentSinkId);
    if (FAILED(hr) || currentSinkId != dwStreamSinkIdentifier) {
      hr = E_INVALIDARG;
    }
  }

  if (SUCCEEDED(hr)) {
    hr = outputStream_->Shutdown();
  }

  outputStream_.Reset();

  return hr;
}

IFACEMETHODIMP H264MediaSink::GetStreamSinkCount(
  _Out_ DWORD *pcStreamSinkCount) {
  if (pcStreamSinkCount == NULL) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    *pcStreamSinkCount = outputStream_ == nullptr ? 0 : 1;
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::GetStreamSinkByIndex(
  DWORD dwIndex,
  _Outptr_ IMFStreamSink **ppStreamSink) {
  if (ppStreamSink == NULL) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  if (dwIndex >= 1) {
    return MF_E_INVALIDINDEX;
  }

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    *ppStreamSink = outputStream_.Get();
    (*ppStreamSink)->AddRef();
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::GetStreamSinkById(
  DWORD dwStreamSinkIdentifier,
  IMFStreamSink **ppStreamSink) {
  if (ppStreamSink == NULL) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);
  HRESULT hr = CheckShutdown();
  Microsoft::WRL::ComPtr<IMFStreamSink> spResult;

  if (SUCCEEDED(hr)) {
    if (outputStream_ == nullptr) {
      hr = MF_E_INVALIDSTREAMNUMBER;
    }
  }

  if (SUCCEEDED(hr)) {
    DWORD currentSinkId;
    hr = outputStream_->GetIdentifier(&currentSinkId);
    if (FAILED(hr) || currentSinkId != dwStreamSinkIdentifier) {
      hr = MF_E_INVALIDSTREAMNUMBER;
    }
  }

  if (SUCCEEDED(hr)) {
    *ppStreamSink = outputStream_.Get();
    (*ppStreamSink)->AddRef();
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::SetPresentationClock(
  IMFPresentationClock *pPresentationClock) {
  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    if (spClock_) {
      hr = spClock_->RemoveClockStateSink(this);
    }
  }

  if (SUCCEEDED(hr)) {
    if (pPresentationClock) {
      hr = pPresentationClock->AddClockStateSink(this);
    }
  }

  if (SUCCEEDED(hr)) {
    spClock_ = pPresentationClock;
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::GetPresentationClock(
  IMFPresentationClock **ppPresentationClock) {
  if (ppPresentationClock == NULL) {
    return E_INVALIDARG;
  }

  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    if (spClock_ == NULL) {
      hr = MF_E_NO_CLOCK;
    } else {
      *ppPresentationClock = spClock_.Get();
      (*ppPresentationClock)->AddRef();
    }
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::Shutdown() {
  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr) && outputStream_ != nullptr) {
    outputStream_->Shutdown();
    outputStream_.Reset();

    spClock_.Reset();

    isShutdown_ = true;
  }

  return S_OK;
}

IFACEMETHODIMP H264MediaSink::OnClockStart(
  MFTIME hnsSystemTime,
  LONGLONG llClockStartOffset) {
  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = outputStream_->Start(llClockStartOffset);
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::OnClockStop(
  MFTIME hnsSystemTime) {
  AutoLock lock(critSec_);

  HRESULT hr = CheckShutdown();

  if (SUCCEEDED(hr)) {
    hr = outputStream_->Stop();
  }

  return hr;
}

IFACEMETHODIMP H264MediaSink::OnClockPause(
  MFTIME hnsSystemTime) {
  return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP H264MediaSink::OnClockRestart(
  MFTIME hnsSystemTime) {
  return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP H264MediaSink::OnClockSetRate(
  /* [in] */ MFTIME hnsSystemTime,
  /* [in] */ float flRate) {
  return S_OK;
}

HRESULT H264MediaSink::RegisterEncodingCallback(
  IH264EncodingCallback *callback) {
  return outputStream_->RegisterEncodingCallback(callback);
}

}  // namespace webrtc
