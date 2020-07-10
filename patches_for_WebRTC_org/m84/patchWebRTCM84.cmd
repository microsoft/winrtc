@echo off

if not exist %WEBRTCM84_ROOT% ( goto :missingenv )

set PATCH_DIR=%~dp0

pushd %WEBRTCM84_ROOT%\build
git.exe am "%PATCH_DIR%0001-Adding-flags-for-using-WinRT-C-projections-and-build.patch"
if errorlevel 1 goto :error
popd

pushd %WEBRTCM84_ROOT%\third_party
git.exe am "%PATCH_DIR%1001-Fixing-UWP-build-for-libvpx.patch"
if errorlevel 1 goto :error
popd

pushd %WEBRTCM84_ROOT%\third_party\libyuv
git.exe am "%PATCH_DIR%2001-Fixing-NV12-to-I420-conversion-with-strides-and-gap.patch"
if errorlevel 1 goto :error
popd

pushd %WEBRTCM84_ROOT%\third_party\boringssl\src
git.exe am "%PATCH_DIR%4001-Arm64-is-a-thing-and-has-intrinsic-to-mul-two-64bit-.patch"
if errorlevel 1 goto :error
popd

pushd %WEBRTCM84_ROOT%\third_party\libjpeg_turbo
git.exe am "%PATCH_DIR%5001-cl-aligns-differently-and-hack-for-extracting-first-.patch"
if errorlevel 1 goto :error
popd

pushd %WEBRTCM84_ROOT%
git.exe am "%PATCH_DIR%3001-Removing-unused-files-containing-Win32-APIs-from-rtc.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3002-Fixing-UWP-build-for-file_rotating_stream.cc.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3003-Allowing-no-contiguous-Y-and-UV-planes.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3004-Changes-for-enabling-the-new-video-capture-module.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3005-Adds-the-Media-Foundation-H264-encoder-and-decoder.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3006-Disabling-switch-without-case-warning-for-aec3.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3007-Do-not-push_back-in-foreach.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%3008-Fixing-UWP-build-for-time_utils.cc"
if errorlevel 1 goto :error
xcopy /Y /E /Q "%PATCH_DIR%\src" .
if errorlevel 1 goto :error

git.exe add modules/audio_device/win/audio_device_core_win.*
if errorlevel 1 goto :error
git.exe commit -m "Audio device for UWP"
if errorlevel 1 goto :error

git.exe add modules/video_capture/windows/*_winrt.*
if errorlevel 1 goto :error
git.exe commit -m "Video capture for UWP"
if errorlevel 1 goto :error

git.exe add modules/video_coding/codecs/h264/win/*.* modules/video_coding/codecs/h264/win_from_old_master/*.*
if errorlevel 1 goto :error
git.exe commit -m "MF based H264 codec"
if errorlevel 1 goto :error

popd

goto :exit

:missingenv
echo Please define WEBRTCM84_ROOT with the full path of WebRTC's src folder.
goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
