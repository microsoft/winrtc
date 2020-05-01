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
cd c:\Downloads
curl -o vs_enterprise.exe -L "https://aka.ms/vs/16/release/vs_enterprise.exe"
call C:\Downloads\vs_enterprise.exe modify --installPath "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise" --add Microsoft.VisualStudio.Component.VC.ATLMFC;includeRecommended --quiet --wait

echo.
echo Installing Windows Debugging Tools...
curl -o winsdksetup.exe -L "https://go.microsoft.com/fwlink/p/?linkid=2083338&clcid=0x409"
winsdksetup.exe /features OptionId.WindowsDesktopDebuggers /quiet

REM Getting depot_tools
echo.
echo Cloneing the depot_tools repo in the root of the c:drive...
c:
cd \
git clone https://chromium.googlesource.com/chromium/tools/depot_tools

echo.
echo Switching to the m80 branch of depot_tools...
cd depot_tools
git checkout chrome/3987
git fetch

echo.
echo Downloading gclient...
echo N | gclient

echo.
echo Setting the path environment variable...
set PATH=c:\depot_tools;%PATH%

REM Installing Python 2.7
echo.
echo Making sure that any python program executed in this session comes from the depot_tools...
where python

echo.
echo Installing pip...
call python -m pip install --upgrade pip

echo.
echo Figuring out where pip was installed...
cd c:\depot_tools
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
cd c:\webrtc

REM Downloading the bits
echo.
echo Telling the gclient tool to initialize your local copy of the repos...
echo N | gclient

echo.
echo Requestin the tools to fetch the WebRTC code base...
echo N | fetch --nohooks webrtc

echo.
echo Changing to the branch-heads/3987 branch...
cd src
call git checkout branch-heads/3987

echo.
echo Instructing the tools to bring the bits from all the sub repositories to your dev box...
echo N | gclient sync