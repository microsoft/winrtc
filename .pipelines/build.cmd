@echo off

set PYTHONPATH=
set PYTHONHOME=
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2019
set PATH=c:\depot_tools;%PATH%
c:
cd c:\webrtc\src
if errorlevel 1 goto :error

REM Setting the vs developer environment
echo.
echo Opening the developer command prompt...
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64
if errorlevel 1 goto :error

REM Setting up
echo.
echo Excluding the unnecessary modules and prepares to build the drop for UWP...
call gn --ide=vs2019 gen out\msvc\x64\Release --filters=//:webrtc --args="is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false"
if errorlevel 1 goto :error

REM Building
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\x64\Release
if errorlevel 1 goto :error

REM Copying the binaries
echo.
echo Copying contents...
cd /D "%~dp0"
if errorlevel 1 goto :error

xcopy /e /i c:\webrtc\src\out ..\output
if errorlevel 1 goto :error

goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
