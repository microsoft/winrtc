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
Follow the steps to acqire WebRTC and add the Patches needed to build for UWP[Universal Windows Platform](https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide).


### Runing the Sample App

After obtaing the WinRTC repo and adding the patches to WebRTC,

1. Navigate to the WinRtc folder.
2. Click on samples.
3. click on MyFirstWinRtc.
4. Click on VideoConferencing.
5. Right click on the VideoConferencing.sln, then click on open.  

A visual studio window should appear on appear on the screen.
In the solution Explorer tab on Visual Studio, there should be 2 projects in the VideoConferencing solution 

<img src="/docs/sln.png"> 

**Note: If you see an Alert about restoring the nugets, Click on Restore to install all required nugets**

6. On the top of the Visual studio, Click on Local Machine. By clicking, a version of the MyFirstWinRtc UWP app will be deployed to your local machine.
Then an app window will pop up for MyFirstWinRtc.

<img src="/docs/AddUser.png"> 

7. Acquire the number from the buttom of App page
 
 <img src="/docs/Roomcode.png"> 

9. Goto [Appr.tc](https://appr.tc)

10. Type in the number gotten from the MyFirstWinRtc App.

 <img src="/docs/Joinroom.png"> 

11. Then Click Join to establish a video call between the two devices.

---

## FeedBack

Your feedback is important to us. Please [file an issue](https://github.com/microsoft/winrtc/issues/new/choose) to report a bug. 










