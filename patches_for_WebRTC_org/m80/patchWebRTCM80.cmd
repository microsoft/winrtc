@echo off

if not exist %WEBRTCM80_ROOT% ( exit /b )

set PATCH_DIR=%~dp0

pushd %WEBRTCM80_ROOT%\build
git.exe am "%PATCH_DIR%0000-WebRTC-doesn-t-need-C-CX.patch"
git.exe am "%PATCH_DIR%T001-Temp-fix-after-VS-2019-update.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party
git.exe am "%PATCH_DIR%0001-UWP-doesn-t-allow-reading-regkeys-from-that-hive.-Al.patch"
git.exe am "%PATCH_DIR%0002-UWP-apps-do-support-GetProcAddress-but-do-support-Ge.patch"
git.exe am "%PATCH_DIR%0009-Fixing-UWP-build-for-libvpx.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party\boringssl\src
git.exe am "%PATCH_DIR%0003-Replacing-deprecated-and-non-UWP-supported-RtlGenRan.patch"
popd

pushd %WEBRTCM80_ROOT%
git.exe am "%PATCH_DIR%0004-This-is-a-change-that-fixes-3-issues-in-the-rtc_base.patch"
git.exe am "%PATCH_DIR%0005-Fixing-UWP-build-for-time_utils.cc.patch"
git.exe am "%PATCH_DIR%0006-Fixing-UWP-build-for-file_rotating_stream.cc.patch"
git.exe am "%PATCH_DIR%0007-Fixing-UWP-build-for-modules-video_capture.patch"
git.exe am "%PATCH_DIR%0008-Fixing-UWP-build-for-modules-audio_device.patch"
popd
