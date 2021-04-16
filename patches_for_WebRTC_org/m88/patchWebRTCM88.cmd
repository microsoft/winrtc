@echo off

if not exist %WEBRTCM88_ROOT% ( goto :missingenv )

set PATCH_DIR=%~dp0

pushd %WEBRTCM88_ROOT%
git.exe am "%PATCH_DIR%0001-Fixing-the-Video-Capture.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0001-Removing-unuse-Win32-APIs-files-from-rtc_base.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0002-Fixing-UWP-build-for-file-rotating-stream.cc.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0003-Allowing-no-contiguous-Y-and-UV-planes.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0004-Enabling-the-new-video-capture-module.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0005-Adds-the-Media-Foundation-H264-encoder-and-decoder.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0006-Disabling-switch-without-case-warning-for-aec3.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0007-Do-not-push_back-in-foreach.patch"
if errorlevel 1 goto :error
git.exe am "%PATCH_DIR%0008-Fixing-timing-issues-for-the-UWP-build.patch"
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
echo Please define WEBRTCM88_ROOT with the full path of WebRTC's src folder.
goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
