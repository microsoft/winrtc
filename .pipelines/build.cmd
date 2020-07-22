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

call :copy c:\webrtc\src\out\msvc\uwp\Release\x64\obj\webrtc.lib ..\output\msvc\uwp\Release\x64\obj\
call :copy c:\webrtc\src\api\*.h ..\include\api\
call :copy c:\webrtc\src\audio\*.h ..\include\audio\
call :copy c:\webrtc\src\base\*.h ..\include\base\
call :copy c:\webrtc\src\call\*.h ..\include\call\
call :copy c:\webrtc\src\common_audio\*.h ..\include\common_audio\
call :copy c:\webrtc\src\common_video\*.h ..\include\common_video\
call :copy c:\webrtc\src\data\*.h ..\include\data\
call :copy c:\webrtc\src\logging\*.h ..\include\logging\
call :copy c:\webrtc\src\media\*.h ..\include\media\
call :copy c:\webrtc\src\modules\*.h ..\include\modules\
call :copy c:\webrtc\src\p2p\*.h ..\include\p2p\
call :copy c:\webrtc\src\pc\*.h ..\include\pc\
call :copy c:\webrtc\src\rtc_base\*.h ..\include\rtc_base\
call :copy c:\webrtc\src\rtc_tools\*.h ..\include\rtc_tools\
call :copy c:\webrtc\src\sdk\*.h ..\include\sdk\
call :copy c:\webrtc\src\stats\*.h ..\include\stats\
call :copy c:\webrtc\src\system_wrappers\*.h ..\include\system_wrappers\
call :copy c:\webrtc\src\video\*.h ..\include\video\
call :copy c:\webrtc\src\common_types.h ..\include\

goto :exit

:copy
xcopy /s /i %~1 %~2
if errorlevel 1 goto :error
goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
