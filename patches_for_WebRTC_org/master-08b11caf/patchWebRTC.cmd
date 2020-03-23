@echo off

if not exist %WEBRTC_MASTER_ROOT% ( exit /b )

set PATCH_DIR=%~dp0

pushd %WEBRTC_MASTER_ROOT%\build
git.exe am "%PATCH_DIR%build-0001-Add-UWP-clang-cl-toolchain.patch"
git.exe am "%PATCH_DIR%build-0002-Add-static-CRT-option.patch"
git.exe am "%PATCH_DIR%build-0003-Disable-Microsoft-template-warning.patch"
popd

pushd %WEBRTC_MASTER_ROOT%\third_party
git.exe am "%PATCH_DIR%..\m80\0001-UWP-doesn-t-allow-reading-regkeys-from-that-hive.-Al.patch"
git.exe am "%PATCH_DIR%..\m80\0002-UWP-apps-do-support-GetProcAddress-but-do-support-Ge.patch"
git.exe am "%PATCH_DIR%..\m80\0009-Fixing-UWP-build-for-libvpx.patch"
git.exe am "%PATCH_DIR%third_party-0001-Disable-symbolizer-on-UWP.patch"
popd

pushd %WEBRTC_MASTER_ROOT%\third_party\libvpx\source\libvpx
git.exe am "%PATCH_DIR%libvpx-0001-Cast-thread-start-to-correct-parameter-type.patch"
popd

pushd %WEBRTC_MASTER_ROOT%\third_party\boringssl\src
git.exe am "%PATCH_DIR%..\m80\0003-Replacing-deprecated-and-non-UWP-supported-RtlGenRan.patch"
popd

pushd %WEBRTC_MASTER_ROOT%
git.exe am "%PATCH_DIR%..\m80\0004-This-is-a-change-that-fixes-3-issues-in-the-rtc_base.patch"
git.exe am "%PATCH_DIR%..\m80\0005-Fixing-UWP-build-for-time_utils.cc.patch"
git.exe am "%PATCH_DIR%..\m80\0006-Fixing-UWP-build-for-file_rotating_stream.cc.patch"
git.exe am "%PATCH_DIR%..\m80\0007-Fixing-UWP-build-for-modules-video_capture.patch"

rem TODO comment this to build on win32
git.exe am "%PATCH_DIR%1008-Fixing-UWP-build-for-modules-audio_device.patch"
popd
