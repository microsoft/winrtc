/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_ASYNC_H_
#define MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_ASYNC_H_

#include <mfidl.h>

template <class T>
class AsyncCallback : public IMFAsyncCallback {
 public:
  typedef HRESULT (T::*InvokeFn)(IMFAsyncResult* pAsyncResult);

  AsyncCallback(T* pParent, InvokeFn fn)
      : m_pParent(pParent), m_pInvokeFn(fn) {}

  // IUnknown
  STDMETHODIMP_(ULONG) AddRef() {
    // Delegate to parent class.
    return m_pParent->AddRef();
  }
  STDMETHODIMP_(ULONG) Release() {
    // Delegate to parent class.
    return m_pParent->Release();
  }
  STDMETHODIMP QueryInterface(REFIID iid, void** ppv) {
    if (!ppv) {
      return E_POINTER;
    }
    if (iid == __uuidof(IUnknown)) {
      *ppv = static_cast<IUnknown*>(static_cast<IMFAsyncCallback*>(this));
    } else if (iid == __uuidof(IMFAsyncCallback)) {
      *ppv = static_cast<IMFAsyncCallback*>(this);
    } else {
      *ppv = NULL;
      return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
  }

  // IMFAsyncCallback methods
  STDMETHODIMP GetParameters(DWORD*, DWORD*) {
    // Implementation of this method is optional.
    return E_NOTIMPL;
  }

  STDMETHODIMP Invoke(IMFAsyncResult* pAsyncResult) {
    if (m_pParent != nullptr && m_pInvokeFn != nullptr) {
      return (m_pParent->*m_pInvokeFn)(pAsyncResult);
    }
    return E_POINTER;
  }

  T* m_pParent;
  InvokeFn m_pInvokeFn;
};

#endif  // MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_ASYNC_H_
