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

REM Setting up for UWP x64
echo.
echo Excluding the unnecessary modules and prepares to build the drop for UWP x64...
call gn gen --ide=vs2019 out\msvc\uwp\Release\x64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"x64\" "
if errorlevel 1 goto :error

REM Building for UWP x64
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\uwp\Release\x64
if errorlevel 1 goto :error

REM Setting up for UWP arm64
echo.
echo Excluding the unnecessary modules and prepares to build the drop for UWP arm64...
call gn gen --ide=vs2019 out\msvc\uwp\Release\arm64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"arm64\" "
if errorlevel 1 goto :error

REM Building for UWP arm64
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\uwp\Release\arm64
if errorlevel 1 goto :error

REM Setting up for UWP x86
echo.
echo Excluding the unnecessary modules and prepares to build the drop for UWP x86...
call gn gen --ide=vs2019 out\msvc\uwp\Release\x86 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"x86\" " 
if errorlevel 1 goto :error

REM Building for UWP x86
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\uwp\Release\x86
if errorlevel 1 goto :error

REM Setting up for Win32 x64 
echo.
echo Excluding the unnecessary modules and prepares to build the drop for Win32 x64...
call gn gen --ide=vs2019 out\msvc\win32\Release\x64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"x64\" "
if errorlevel 1 goto :error

REM Building for Win32 x64
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\win32\Release\x64
if errorlevel 1 goto :error

REM Setting up for Win32 arm64
echo.
echo Excluding the unnecessary modules and prepares to build the drop for Win32 arm64...
call gn gen --ide=vs2019 out\msvc\win32\Release\arm64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"arm64\" "
if errorlevel 1 goto :error

REM Building for Win32 arm64
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\win32\Release\arm64
if errorlevel 1 goto :error

REM Setting up for Win32 x86
echo.
echo Excluding the unnecessary modules and prepares to build the drop for Win32 x86...
call gn gen --ide=vs2019 out\msvc\win32\Release\x86 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false target_cpu=\"x86\" " 
if errorlevel 1 goto :error

REM Building for Win32 x86
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\win32\Release\x86
if errorlevel 1 goto :error

REM Copying the binaries
echo.
echo Copying contents...
cd /D "%~dp0"
if errorlevel 1 goto :error

call :copyFiles c:\webrtc\src\out\msvc\uwp\Release\x64\obj\webrtc.lib ..\output\msvc\uwp\Release\x64\obj\
call :copyFiles c:\webrtc\src\out\msvc\uwp\Release\arm64\obj\webrtc.lib ..\output\msvc\uwp\Release\arm64\obj\
call :copyFiles c:\webrtc\src\out\msvc\uwp\Release\x86\obj\webrtc.lib ..\output\msvc\uwp\Release\x86\obj\
call :copyFiles c:\webrtc\src\out\msvc\win32\Release\x64\obj\webrtc.lib ..\output\msvc\win32\Release\x64\obj\
call :copyFiles c:\webrtc\src\out\msvc\win32\Release\arm64\obj\webrtc.lib ..\output\msvc\win32\Release\arm64\obj\
call :copyFiles c:\webrtc\src\out\msvc\win32\Release\x86\obj\webrtc.lib ..\output\msvc\win32\Release\x86\obj\
call :copyFiles c:\webrtc\src\api\*.h ..\include\api\
call :copyFiles c:\webrtc\src\audio\*.h ..\include\audio\
call :copyFiles c:\webrtc\src\base\*.h ..\include\base\
call :copyFiles c:\webrtc\src\call\*.h ..\include\call\
call :copyFiles c:\webrtc\src\common_audio\*.h ..\include\common_audio\
call :copyFiles c:\webrtc\src\common_video\*.h ..\include\common_video\
call :copyFiles c:\webrtc\src\logging\*.h ..\include\logging\
call :copyFiles c:\webrtc\src\media\*.h ..\include\media\
call :copyFiles c:\webrtc\src\modules\*.h ..\include\modules\
call :copyFiles c:\webrtc\src\p2p\*.h ..\include\p2p\
call :copyFiles c:\webrtc\src\pc\*.h ..\include\pc\
call :copyFiles c:\webrtc\src\rtc_base\*.h ..\include\rtc_base\
call :copyFiles c:\webrtc\src\rtc_tools\*.h ..\include\rtc_tools\
call :copyFiles c:\webrtc\src\stats\*.h ..\include\stats\
call :copyFiles c:\webrtc\src\system_wrappers\*.h ..\include\system_wrappers\
call :copyFiles c:\webrtc\src\third_party\abseil-cpp\absl\*.h ..\include\absl\
call :copyFiles c:\webrtc\src\third_party\libyuv\include\*.h ..\include\
call :copyFiles c:\webrtc\src\video\*.h ..\include\video\
call :copyFiles c:\webrtc\src\common_types.h ..\include\

goto :exit

:copyFiles
xcopy /s /i %~1 %~2
if errorlevel 1 goto :error
goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
