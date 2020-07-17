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
We want to hear from you!

File a new issue! Tell us what problem you're trying to solve, how you've tried to solve it so far, and what would be the ideal solution for your app. Bonus points if there's a gist or existing repo we can look at with you.

  1. Ask a question
  
  Also, have a look at our frequently asked questions page which we will update periodically.
	
  2. Start a discussion
  
  Discussions can be about any topics or ideas related to WinRTC. For example, you might start a Discussion issue to see if others are interested in a cool new idea you've been thinking about that isn't quite ready to be filed as a more formal Feature Proposal.

  3. Make a feature proposal
  
  What components do you want to see in WinRTC? For example you can propose a new API on an existing type, or an idea for a new API. It's fine if you don't have all the details: you can start with a summary and rationale.

Over time, we will add WinRTC code and samples into this repo and provide code contribution/developer guidelines.

### License Info

Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution.

## Code of Conduct

This project has adopted the Microsoft Open Source Code of Conduct.
