@echo off

if not exist %WEBRTC_MASTER_ROOT% ( exit /b )

pushd %WEBRTC_MASTER_ROOT%\build
git.exe reset --hard 2f17606c25956e800b6c4670c294a03620e78551
popd

pushd %WEBRTC_MASTER_ROOT%\third_party
git.exe reset --hard 18f4ad54fc72cd2c408774dde375e25482b62b4b
popd

pushd %WEBRTC_MASTER_ROOT%\third_party\boringssl\src
git.exe reset --hard 1cc95ac07c17d61bea601832bbdc1f8d13d313db
popd

pushd %WEBRTC_MASTER_ROOT%\third_party\libvpx\source\libvpx
git.exe reset --hard 4254ecaa075e672b66b9d723ebdd3d7ed7125055
popd

pushd %WEBRTC_MASTER_ROOT%
git.exe reset --hard 08b11cafae02834dedb3230321c5f2c775febca2
popd
