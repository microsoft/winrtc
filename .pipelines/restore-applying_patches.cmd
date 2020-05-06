@echo off

cd /D "%~dp0"

call git config --global user.name "WinRTC"
if errorlevel 1 goto :error

call git config --global user.email "<>"
if errorlevel 1 goto :error

echo.
echo Setting the WEBRTCM80_ROOT enviornment variable...
set WEBRTCM80_ROOT=c:\webrtc\src

echo.
echo Running the batch file that will patch all the necessary repos from the WebRTC code base...
..\patches_for_WebRTC_org\m80\patchWebRTCM80.cmd
if errorlevel 1 goto :error

goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
