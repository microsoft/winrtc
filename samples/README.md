## How To Run The MyFirstWinRtc Sample App

### Requirement for running the sample App.

1. Visual Studio 2019 Version 16.6.2 or later.
2. At least 8GB of RAM (16GB of RAM is recommended).
3. At least 15GB of disk space.

### Acquiring Winrtc and adding WebRtc Patches

1. Acqiring the WinRtc. Goto to the [Winrtc GitHub Repo](https://github.com/microsoft/winrtc).

2. Clone the Repo for winrtc, by clicking download zip or using command line
```
C:\> git clone https://github.com/microsoft/winrtc.git
```
3. Goto the [Patches for WebRTC](https://github.com/microsoft/winrtc/tree/master/patches_for_WebRTC_org/m84).
Follow the steps to acqire WebRTC and add the Patches needed to build UWP ([Universal Windows Platform](https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide)).


### Runing the Sample App

After obtaing the WinRTC repo and adding the patches to WebRTC,

1. Navigate to the WinRtc folder.
2. Click on samples.
3. click on MyFirstWinRtc.
4. Click on VideoConferencing.
5. Right click on the VideoConferencing.sln, then click on open.  

A visual studio window should appear on the screen.
In the solution Explorer tab on Visual Studio, there should be 2 projects in the VideoConferencing solution 

<img src="/docs/sln.png"> 

**Note: If you see an alert about restoring the nugets, click on restore to install all required nugets. Else, see instruction 6**

6. To add the WinRTC NuGet, click on Tools on the Tool bar then navigate to NuGet Package Manager then click on Manage NuGet Package for Solution.

<img src="/docs/adding_Nuget.png">

**Note: Make sure to set the Package source to nuget.org**

7. Click on "Browse". On the Search bar, type "Microsoft.WinRTC.libwebrtc.uwp.84.0.14370001", the click "install" to install the NuGet.

<img src="/docs/install_Nuget.png">

8. On the top of the Visual studio, Click on Local Machine. By clicking, a version of the MyFirstWinRtc UWP app will be deployed to your local machine.
Then an app window will pop up for MyFirstWinRtc.

<img src="/docs/AddUser.png"> 

9. Acquire the number from the buttom of App page
 
 <img src="/docs/roomNum.png"> 

10. Goto [Appr.tc](https://appr.tc)

11. Type in the number gotten from the MyFirstWinRtc App.

 <img src="/docs/tempsnip.png"> 

12. Then Click Join to establish a connection between the two devices for a video call.

---

## FeedBack

Your feedback is important to us. Please [file an issue](https://github.com/microsoft/winrtc/issues/new/choose) to report a bug. 










