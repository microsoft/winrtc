@echo off

set PYTHONPATH=
set PYTHONHOME=
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
c:
cd c:\webrtc\src
set PATH=c:\depot_tools;%PATH%

REM Setting up
echo.
echo Excluding the unnecessary modules and prepares to build the drop for UWP...
call gn --ide=vs2019 gen out\msvc\x64\Release --filters=//:webrtc --args="is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false"

echo.
echo Viewing the full list of editable build arguments...
call gn args --list out\msvc\x64\Release

REM Building
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\x64\Release

REM Copying the binaries
echo.
echo Copying contents...
cd /D "%~dp0"
xcopy /e /i c:\webrtc\src\out ..\output