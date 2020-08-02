@echo off

REM Prerequisities
echo.
echo Deleteing python from path...
set PYTHONPATH=
set PYTHONHOME=

echo.
echo Installing Visual Stuio 2019 workloads...
c:
mkdir c:\Downloads
if errorlevel 1 goto :error

cd c:\Downloads
if errorlevel 1 goto :error

curl -o vs_enterprise.exe -L "https://aka.ms/vs/16/release/vs_enterprise.exe"
if errorlevel 1 goto :error

call C:\Downloads\vs_enterprise.exe modify --installPath "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise" --add Microsoft.VisualStudio.Component.VC.ATLMFC;includeRecommended --add Microsoft.VisualStudio.Component.VC.MFC.ARM64;includeRecommended --add Microsoft.VisualStudio.Component.VC.Tools.ARM64;includeRecommended --quiet --wait
if errorlevel 1 goto :error

echo.
echo Installing Windows Debugging Tools for Windows 10 SDK, version 1903 (10.0.18362.1)...
curl -o winsdksetup.exe -L "https://go.microsoft.com/fwlink/p/?linkid=2083338&clcid=0x409"
if errorlevel 1 goto :error

winsdksetup.exe /features OptionId.WindowsDesktopDebuggers /quiet
if errorlevel 1 goto :error

REM Getting depot_tools
echo.
echo Downloading the depot_tools...
curl https://storage.googleapis.com/chrome-infra/depot_tools.zip --output depot_tools.zip
if errorlevel 1 goto :error

echo.
echo Opening the zip file...
c:
mkdir c:\depot_tools
if errorlevel 1 goto :error

tar -xf depot_tools.zip -C /../depot_tools/
if errorlevel 1 goto :error

echo.
echo Deleting the depot_tools.zip file
del depot_tools.zip

echo.
echo Setting the path environment variable...
set PATH=c:\depot_tools;%PATH%

REM Setting up the environment
echo.
echo Informing depot_tools to use locally installed version of Visual Studio...
set DEPOT_TOOLS_WIN_TOOLCHAIN=0

echo.
echo Informing GYP build tool about the version of the Visual Studio we're using...
set GYP_MSVS_VERSION=2019

echo.
echo Creating the folder where the code base will be placed...
c:
mkdir c:\webrtc
if errorlevel 1 goto :error

cd c:\webrtc
if errorlevel 1 goto :error

REM Downloading the bits
echo.
echo Telling the gclient tool to initialize your local copy of the repos...
call gclient
if errorlevel 1 goto :error

echo.
echo Requesting the tools to fetch the WebRTC code base...
call fetch --nohooks webrtc
if errorlevel 1 goto :error

echo.
echo Changing to the branch-heads/4147 branch...
cd src
if errorlevel 1 goto :error

call git checkout branch-heads/4147
if errorlevel 1 goto :error

echo.
echo Instructing the tools to bring the bits from all the sub repositories to your dev box...
gclient sync -D -r branch-heads/4147
if errorlevel 1 goto :error

goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
