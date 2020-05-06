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

call C:\Downloads\vs_enterprise.exe modify --installPath "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise" --add Microsoft.VisualStudio.Component.VC.ATLMFC;includeRecommended --quiet --wait
if errorlevel 1 goto :error

echo.
echo Installing Windows Debugging Tools for Windows 10 SDK, version 1903 (10.0.18362.1)...
curl -o winsdksetup.exe -L "https://go.microsoft.com/fwlink/p/?linkid=2083338&clcid=0x409"
if errorlevel 1 goto :error

winsdksetup.exe /features OptionId.WindowsDesktopDebuggers /quiet
if errorlevel 1 goto :error

REM Getting depot_tools
echo.
echo Cloning the depot_tools repo in the root of the c:drive...
c:
cd \
if errorlevel 1 goto :error

git clone https://chromium.googlesource.com/chromium/tools/depot_tools
if errorlevel 1 goto :error

echo.
echo Switching to the m80 branch of depot_tools...
cd depot_tools
if errorlevel 1 goto :error

git checkout chrome/3987
if errorlevel 1 goto :error

git fetch
if errorlevel 1 goto :error

REM At this moment, gclient will offer to use the most recent version of deplot_tools. 
REM We do not want that because the most recent version of depot_tools dropped compatibility 
REM with python 2.7 that is currently required for building for Windows by some components.
echo.
echo Downloading gclient...
echo N | gclient
if errorlevel 1 goto :error

echo.
echo Setting the path environment variable...
set PATH=c:\depot_tools;%PATH%

REM Installing Python 2.7
echo.
echo Making sure that any python program executed in this session comes from the depot_tools...
set errPython=
for /f "delims=" %%i in ('where python') do (
	if c:\depot_tools\python.bat==%%i set errPython==0
)
if not defined errPython goto :error

echo.
echo Installing pip...
call python -m pip install --upgrade pip

echo.
echo Figuring out where pip was installed...
cd c:\depot_tools
if errorlevel 1 goto :error

for /f "delims=" %%i in ('where /r . pip.exe') do cd "%%i\.."
pip install pywin32

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
echo N | gclient
if errorlevel 1 goto :error

echo.
echo Requestin the tools to fetch the WebRTC code base...
echo N | fetch --nohooks webrtc
if errorlevel 1 goto :error

echo.
echo Changing to the branch-heads/3987 branch...
cd src
if errorlevel 1 goto :error

call git checkout branch-heads/3987
if errorlevel 1 goto :error

echo.
echo Instructing the tools to bring the bits from all the sub repositories to your dev box...
echo N | gclient sync
if errorlevel 1 goto :error

goto :exit

:error
echo Last command failed with erro code: %errorlevel%

:exit
exit /b %errorlevel%
