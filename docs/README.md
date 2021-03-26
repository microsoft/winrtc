# Welcome to the WinRTC repo!

This repository contains the following sections:

[Getting Started](#overview)   |   [Resources](#resources)   |   [FAQ](#faq)   |   [What's New](#whats-new)   |   [Road Map](#road-map)   |   [Contribution Guidelines](#contributing)   |   [Feedback Form](#feedback)

---

## Overview 

### What is WinRTC? 
The WinRTC project aims to host everything needed to build apps with interoperable real time communications for windows. It brings the power of WebRTC to Windows apps written in C#, C++ and VB. With WinRTC, native Windows apps can have real time communications with web browsers via WebRTC.

#### Build 2020 WinRTC Session

For an overview and live walkthrough of building with WinRTC please see our [session from Build 2020](https://aka.ms/m365sk126)!

Session Breakdown:
- [00:00 to 11:30](https://aka.ms/m365sk126) - Excellent WinRTC overview, history, use cases and customer success stories from Dina Ayman, PM for WinRTC
- [11:30 to 25:10](https://youtu.be/GKrTmgZT-EA?t=691) - Awesome end to end demo of using WinRTC in code from Augusto Righetto, Sr Engineer for WinRTC

#### WinRTC Architecture 

<img alt="WinRTC architecture" src="https://raw.githubusercontent.com/microsoft/winrtc/master/docs/WinRTC_Architecture_Diagram.png">

WinRTC's compatibility extends to .Net and RN4W, as well as UWP and Win32 applications. For more insight into the architecture, check out our [frequently asked questions](https://github.com/microsoft/winrtc/blob/master/docs/FAQ.md) page. 

---

## Installing and running WinRTC

#### Patching WebRTC M84

As is, the WebRTC code base has a Win32 port that doesn't build for UWP. Get started with building WebRTC for Windows with the patches available in this repo. We recommend applying the [M84 patch](../patches_for_WebRTC_org/m84), which has the most recent security updates and features. 

These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet. 

#### Wrappers for WinRTC

The [`WebRtcWrapper`](https://github.com/microsoft/winrtc/tree/master/WebRtcWrapper) folder contains an alpha version of WinRT wrappers for WebRTC. With these wrappers you'll be able to consume WebRTC functionality from any WinRT projection. 

**Note: The WinRTC NuGet is now live and ready to download. You can download the NuGet from Nuget.org or through Visual Studio.
<img src="/docs/install_Nuget.png">** 

---

## Resources

For more information about WinRTC, you may find some of these resources useful and interesting:

* Overview and live walkthrough of WinRTC: [Build 2020 WinRTC Session](https://aka.ms/m365sk126)
* Answering common questions from our community: [Expert Q&A: WinRTC Open Source WebRTC Library](https://www.youtube.com/watch?v=AsjgLAtzUT4)
* The foundation of WinRTC: [Google's WebRTC project](https://webrtc.org/)
* Intuitive explanation of real time communication: [Quick introduction to WebRTC](https://www.youtube.com/watch?v=RI5fGsEvDnI)
* Extensive blog about WebRTC fundamentals: [HTML5 Rocks WebRTC Tutorial](https://www.html5rocks.com/en/tutorials/webrtc/basics/)
* Collection of WebRTC tutorials: [Kranky Geek WebRTC video series](https://www.bing.com/videos/search?q=kranky+geek+winrtc&qpvt=kranky+geek+winrtc&FORM=VDRE)

---

## FAQ

Check out our [frequently asked questions](https://github.com/microsoft/winrtc/blob/master/docs/FAQ.md) page which we will update periodically.

Have an urgent question and can't find the answer? [Don't hesitate to ask!](https://github.com/microsoft/winrtc/issues/new/choose)

---

## What's New
### July 2020 Update

A big goal for us this month was to improve the hardware accelerated capabilities for audio and video capture, rendering, encoders and decoders in WinRTC.  

We'd like to thank everyone who filed a bug, gave feedback, or made a pull-request. The WinRTC team is extremely grateful to have the support of an amazing active community.

In our most recent update, we have:
- Added port WebRTC-UWP H264 Encoder & Decoder over WinRTC
- Added port WebRTC-UWP supporting Camera Profiles over WinRTC
- Enabled libWebRTC built-in camera capture module for Arm64 devices
- Created public documentation on GitHub wiki about how to change libWebRTC build system 

For our next release, we are proactively working on:
- Creating NuGet packages for WinRTC wrappers

---

## Road Map

_Coming soon!_

We're in the midst of building our road map to showcase which features we'll be working on in the near-future. [You can help by filing issues for features you'd like to see!](https://github.com/microsoft/winrtc/issues/new/choose) For example, you can propose an idea for a new API. It's fine if you don't have all the details: you can start with a summary and rationale.

## Contributing

_We want to hear from you!_

File a [new issue!](https://github.com/microsoft/winrtc/issues/new/choose) Tell us what problem you're
trying to solve, how you've tried to solve it so far, and what would be the ideal solution for your app.  Bonus
points if there's a gist or existing repo we can look at with you.

Read more about the [contribution guide here](https://github.com/microsoft/winrtc/blob/master/docs/CONTRIBUTING.md).

### License Info

Most contributions require you to agree to a [Contributor License Agreement (CLA)][oss-CLA] declaring that you have the right to, and actually do, grant us the rights to use your contribution.

---

## Feedback

Did you find this page helpful? [Let us know by providing your feedback](https://forms.office.com/Pages/ResponsePage.aspx?id=v4j5cvGGr0GRqy180BHbR-tCZConOl1DsmA7Z5sVzcpUQTNTSkRaWEMxVFJVWDdQWUpSOUxCRVJNVC4u).  

---

## Support

Let us know how we can help. [For Support](../docs/SUPPORT.md)

---

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct][oss-conduct-code].

[oss-CLA]: https://cla.opensource.microsoft.com
[oss-conduct-code]: CODE_OF_CONDUCT.md
