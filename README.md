# WinRTC

The WinRTC project aims to host everything needed to build apps with interoperable real time communications for modern Windows. It brings the power of WebRTC to modern Windows apps written in C#, C++ and VB. With WinRTC, native Windows apps can have real time communications with web browsers via WebRTC.

# Goodies

At this moment, you'll find the patch files required for building WebRTC for modern Windows. As is, the WebRTC code base has a Win32 port that doesn't build for UWP. You'll able to build a recent version of WebRTC with the patches available by this repo.

These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet. Contribute changes back take time. We want you to be able to build Windows apps with real time communications as soon as possible. So, feel free to follow the instructions in the `patches_for_WebRTC_org/m80` folder.

We know that building a patched WebRTC code base from scratch probably is not the best use of your time. There are other components we're building on top of WebRTC code base that will allow you to use interoperable real time communications in your app.

We're in our earlier stages, stay tuned because we're going to be developing this in the open.

# Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
