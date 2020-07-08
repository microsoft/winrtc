From 6d9f314982b0b0f95692952d6744c6eb9f325bde Mon Sep 17 00:00:00 2001
From: Augusto Righetto <aurighet@microsoft.com>
Date: Mon, 23 Dec 2019 11:27:37 -0800
Subject: [PATCH] Fixing timing issues for the UWP build

1. safe_conversions.h is being included directly on Mac builds, but not on Windows builds.
   Probably safe_conversions.h is indirectly included on Windows builds by some header that is not being built on UWP builds.

2. Converting to nano seconds in way that fits the variable precision.
---
 rtc_base/time_utils.cc          | 5 +++++
 system_wrappers/source/clock.cc | 2 +-
 2 files changed, 6 insertions(+), 1 deletion(-)

diff --git a/rtc_base/time_utils.cc b/rtc_base/time_utils.cc
index 7a72173db7..4033c12740 100644
--- a/rtc_base/time_utils.cc
+++ b/rtc_base/time_utils.cc
@@ -26,6 +26,11 @@
 #include <windows.h>
 #include <mmsystem.h>
 #include <sys/timeb.h>
+
+#if defined(WINUWP)
+#include "rtc_base/numerics/safe_conversions.h"
+#endif
+
 // clang-format on
 #endif
 
diff --git a/system_wrappers/source/clock.cc b/system_wrappers/source/clock.cc
index 3eea155711..5976fb2562 100644
--- a/system_wrappers/source/clock.cc
+++ b/system_wrappers/source/clock.cc
@@ -97,7 +97,7 @@ class WinUwpRealTimeClock final : public RealTimeClock {
     struct timeval tv;
 
     tv.tv_sec = rtc::dchecked_cast<long>(nanos / 1000000000);
-    tv.tv_usec = rtc::dchecked_cast<long>(nanos / 1000);
+    tv.tv_usec = rtc::dchecked_cast<long>((nanos % 1000000000) / 1000);
 
     return tv;
   }
-- 
2.22.0.vfs.1.1.57.gbaf16c8

