/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_CRITSEC_H_
#define MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_CRITSEC_H_

#include <WinBase.h>

class CritSec {
 public:
  CRITICAL_SECTION m_criticalSection;

 public:
  CritSec() { InitializeCriticalSectionEx(&m_criticalSection, 100, 0); }

  ~CritSec() { DeleteCriticalSection(&m_criticalSection); }

  _Acquires_lock_(m_criticalSection) void Lock() {
    EnterCriticalSection(&m_criticalSection);
  }

  _Releases_lock_(m_criticalSection) void Unlock() {
    LeaveCriticalSection(&m_criticalSection);
  }
};

//////////////////////////////////////////////////////////////////////////
//  AutoLock
//  Description: Provides automatic locking and unlocking of a
//               of a critical section.
//
//  Note: The AutoLock object must go out of scope before the CritSec.
//////////////////////////////////////////////////////////////////////////

class AutoLock {
 private:
  CritSec* m_pCriticalSection;

 public:
  _Acquires_lock_(m_pCriticalSection) explicit AutoLock(CritSec& crit) {
    m_pCriticalSection = &crit;
    m_pCriticalSection->Lock();
  }

  _Releases_lock_(m_pCriticalSection) ~AutoLock() {
    m_pCriticalSection->Unlock();
  }
};

#endif  // MODULES_VIDEO_CODING_CODECS_H264_WIN_UTILS_CRITSEC_H_
