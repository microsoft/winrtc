# WinRTC

## What is WinRTC? 

The WinRTC project aims to host everything needed to build apps with interoperable real time communications for modern Windows. It brings the power of WebRTC to modern Windows apps written in C#, C++ and VB. With WinRTC, native Windows apps can have real time communications with web browsers via WebRTC.

## How can I use WinRTC? 

### Patching WebRTC M80

At this moment, you'll find the patch files required for building WebRTC for modern Windows. As is, the WebRTC code base has a Win32 port that doesn't build for UWP. You'll able to build a recent version of WebRTC with the patches available by this repo.

These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet. Contribute changes back take time. We want you to be able to build Windows apps with real time communications as soon as possible. So, feel free to follow the instructions in the `patches_for_WebRTC_org/m80` folder.

We know that building a patched WebRTC code base from scratch probably is not the best use of your time. There are other components we're building on top of WebRTC code base that will allow you to use interoperable real time communications in your app.

We're in our earlier stages, stay tuned because we're going to be developing this in the open.

### Wrappers for WinRT

The `WebRtcWrapper` folder contains an alpha version of WinRT wrappers for WebRTC. With these wrappers you'll be able to consume WebRTC functionality from any WinRT projection. We're working on NuGet packages for these wrappers. It takes more than a .nuspec file to have binary code shipped in a NuGet package.

This is a work in progress, and we would love to hear from you.

## What's New
### July 2020 Update
- Working on the improvements required for leveraging the hardware accelerated capabilities for audio and video capture, rendering, encoders and decoders in WinRTC.  
- Port WebRTC-UWP H264 Encoder & Decoder over WinRTC. 
- Port WebRTC-UWP supporting Camera Profiles over WinRTC. 
- Enabled libWebRTC built-in camera capture module for Arm64 devices. 

Created a public documentation on GitHub wiki: 
- Newcomers to WinRTC have documentation about how to change libWebRTC build system. 

For our next release, we are proactively working on:
- Ensuring that NuGet packages are available.

## Contributing

_We want to hear from you!_

File a [new issue!](https://github.com/microsoft/winrtc/issues/new/choose) Tell us what problem you're
trying to solve, how you've tried to solve it so far, and what would be the ideal solution for your app.  Bonus
points if there's a gist or existing repo we can look at with you.

Read more about the [contribution guide here](CONTRIBUTING.md).

### License Info

 Most contributions require you to agree to a [Contributor License Agreement (CLA)][oss-CLA] declaring that you have the right to, and actually do, grant us the rights to use your contribution.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct][oss-conduct-code].

[oss-CLA]: https://cla.opensource.microsoft.com
[oss-conduct-code]: CODE_OF_CONDUCT.md
