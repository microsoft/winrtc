@echo off

cd /D "%~dp0..\NuSpecs"
nuget pack -Version %CDP_PACKAGE_VERSION_NUMERIC%-%CDP_BUILD_TAG%
