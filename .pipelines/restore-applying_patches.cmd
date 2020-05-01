@echo off

cd /D "%~dp0"

call git config --global user.name "WinRTC"
call git config --global user.email "<>"

echo.
echo Setting the WEBRTCM80_ROOT enviornment variable...
set WEBRTCM80_ROOT=c:\webrtc\src

echo.
echo Running the batch file that will patch all the necessary repos from the WebRTC code base...
..\patches_for_WebRTC_org\m80\patchWebRTCM80.cmd