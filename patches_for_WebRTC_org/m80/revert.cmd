@echo off

if not exist %WEBRTCM80_ROOT% ( exit /b )

pushd %WEBRTCM80_ROOT%\build
git.exe reset --hard fae06de3ddf35e28470d8042b69a14c5232605c4
popd

pushd %WEBRTCM80_ROOT%\third_party
git.exe reset --hard ca4f6358ddaff805c883393c17ed232d6b932e09
popd

pushd %WEBRTCM80_ROOT%\third_party\boringssl\src
git.exe reset --hard 243b5cc9e33979ae2afa79eaa4e4c8d59db161d4
popd

pushd %WEBRTCM80_ROOT%
git.exe reset --hard 1256d9bcac500d962e884231b0360d8c3eb3ef02
popd
