@echo off

if not exist %WEBRTCM80_ROOT% ( exit /b )

set PATCH_DIR=%~dp0

pushd %WEBRTCM80_ROOT%\build
git.exe am "%PATCH_DIR%0000-WebRTC-doesn-t-need-C-CX.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party
git.exe am "%PATCH_DIR%0001-UWP-doesn-t-allow-reading-regkeys-from-that-hive.-Al.patch"
git.exe am "%PATCH_DIR%0002-UWP-apps-do-support-GetProcAddress-but-do-support-Ge.patch"
git.exe am "%PATCH_DIR%0009-Fixing-UWP-build-for-libvpx.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party\libyuv
git.exe am "%PATCH_DIR%0007.3-Fixing-NV12-to-I420-conversion-with-strides-and-gap.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party\boringssl\src
git.exe am "%PATCH_DIR%0003-Replacing-deprecated-and-non-UWP-supported-RtlGenRan.patch"
git.exe am "%PATCH_DIR%6401-Arm64-is-a-thing-and-has-intrinsic-to-mul-two-64bit-.patch"
popd

pushd %WEBRTCM80_ROOT%\third_party\libjpeg_turbo
git.exe am "%PATCH_DIR%6401-cl-aligns-differently-and-hack-for-extracting-first-.patch"
popd

pushd %WEBRTCM80_ROOT%
git.exe am "%PATCH_DIR%0004-This-is-a-change-that-fixes-3-issues-in-the-rtc_base.patch"
git.exe am "%PATCH_DIR%0005-Fixing-UWP-build-for-time_utils.cc.patch"
git.exe am "%PATCH_DIR%0006-Fixing-UWP-build-for-file_rotating_stream.cc.patch"
git.exe am "%PATCH_DIR%0007-Fixing-UWP-build-for-modules-video_capture.patch"
git.exe am "%PATCH_DIR%0007.1-BUG-Requested-camera-settings-were-not-being-honored.patch"
git.exe am "%PATCH_DIR%0007.2-Properly-handling-async-model-for-initializing-Media.patch"
git.exe am "%PATCH_DIR%0007.4-Adding-video-profiles-capabilities-to-the-video-capt.patch"
git.exe am "%PATCH_DIR%0007.5-Porting-H264-encoder-from-the-WebRTC-UWP-project.patch"
git.exe am "%PATCH_DIR%0007.6-Porting-H264-decoder-from-WebRTC-UWP.patch"
git.exe am "%PATCH_DIR%0008-Fixing-UWP-build-for-modules-audio_device.patch"
git.exe am "%PATCH_DIR%6401-Shift-operator-in-Arm-doesn-t-work-the-same-as-Intel.patch"
popd
