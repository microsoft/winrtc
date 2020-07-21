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
call gn gen --ide=vs2019 out\msvc\uwp\Release\x64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false"
if errorlevel 1 goto :error

REM Building
echo.
echo Building the patched WebRTC...
ninja -C out\msvc\uwp\Release\x64
if errorlevel 1 goto :error

REM Copying the binaries
echo.
echo Copying contents...
cd /D "%~dp0"
if errorlevel 1 goto :error

xcopy /s /i c:\webrtc\src\out\msvc\uwp\Release\x64\obj\webrtc.lib ..\output\msvc\uwp\Release\x64\obj\
xcopy /s /i c:\webrtc\src\api\*.h ..\include\api\
xcopy /s /i c:\webrtc\src\audio\*.h ..\include\audio\
xcopy /s /i c:\webrtc\src\base\*.h ..\include\base\
xcopy /s /i c:\webrtc\src\call\*.h ..\include\call\
xcopy /s /i c:\webrtc\src\common_audio\*.h ..\include\common_audio\
xcopy /s /i c:\webrtc\src\common_video\*.h ..\include\common_video\
xcopy /s /i c:\webrtc\src\data\*.h ..\include\data\
xcopy /s /i c:\webrtc\src\logging\*.h ..\include\logging\
xcopy /s /i c:\webrtc\src\media\*.h ..\include\media\
xcopy /s /i c:\webrtc\src\modules\*.h ..\include\modules\
xcopy /s /i c:\webrtc\src\p2p\*.h ..\include\p2p\
xcopy /s /i c:\webrtc\src\pc\*.h ..\include\pc\
xcopy /s /i c:\webrtc\src\rtc_base\*.h ..\include\rtc_base\
xcopy /s /i c:\webrtc\src\rtc_tools\*.h ..\include\rtc_tools\
xcopy /s /i c:\webrtc\src\sdk\*.h ..\include\sdk\
xcopy /s /i c:\webrtc\src\stats\*.h ..\include\stats\
xcopy /s /i c:\webrtc\src\system_wrappers\*.h ..\include\system_wrappers\
xcopy /s /i c:\webrtc\src\video\*.h ..\include\video\
xcopy /s /i c:\webrtc\src\common_types.h ..\include\
if errorlevel 1 goto :error

goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
