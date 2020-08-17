@echo off

cd /D "%~dp0..\NuSpecs"
nuget pack Microsoft.WinRTC.libwebrtc.uwp.nuspec -Version %CDP_PACKAGE_VERSION_NUMERIC%-%CDP_BUILD_TAG%
nuget pack Microsoft.WinRTC.libwebrtc.win32.nuspec -Version %CDP_PACKAGE_VERSION_NUMERIC%-%CDP_BUILD_TAG%
