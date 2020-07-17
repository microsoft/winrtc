# WinRTC

### What is WinRTC? 
The WinRTC project aims to host everything needed to build apps with interoperable real time communications for windows. It brings the power of WebRTC to modern Windows apps written in C#, C++ and VB. With WinRTC, native Windows apps can have real time communications with web browsers via WebRTC.

### How can I use WinRTC? 

#### Patching WebRTC M84 

As is, the WebRTC code base has a Win32 port that doesn't build for UWP. Get started with building WebRTC for Windows with the patches available in this repo. We recommend applying the [`M84 patch`](patches_for_WebRTC_org/m84), which has the most recent security updates and features. 

These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet. Contribute changes back take time. We want you to be able to build Windows apps with real time communications as soon as possible. 

#### Wrappers for WinRT

The [`WebRtcWrapper`](https://github.com/microsoft/winrtc/tree/documentation-edits/WebRtcWrapper) folder contains an alpha version of WinRT wrappers for WebRTC. With these wrappers you'll be able to consume WebRTC functionality from any WinRT projection. Stay tuned for NuGet packages coming soon for these wrappers! 

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

## Road Map

_Coming soon!_

In the mean time, let us know what components you would like to see us work on in the near-future by making [a feature request](https://github.com/microsoft/winrtc/issues/new?assignees=&labels=&template=feature_request.md&title=)! For example, you can propose a new API on an existing type, or an idea for a new API. It's fine if you don't have all the details: you can start with a summary and rationale.

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
