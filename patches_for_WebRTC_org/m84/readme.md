# Patching WebRTC for Windows

This document will walk you through the steps required for getting the WebRTC code base, patching it and building it for UWP. These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet.

## Prerequisites

#### Machine requirements 
- Windows 10 (build 19041) or later.
- At least 8GB of RAM (16GB of RAM is recommended). 
- At least 15GB of disk space. 
- SSD drive formatted with NTFS. 

#### External applications
- [Microsoft Visual Studio 2019 16.6.2](http://visualstudio.com). 
- Command Prompt ([Windows Terminal](https://www.microsoft.com/en-us/p/windows-terminal/9n0dx20hk701?activetab=pivot:overviewtab) is recommended, but cmd.exe works as well).

In the **Visual Studio Installer** app, please verify if Visual Studio 2019 has the **Desktop development with C++** and **Universal Windows Platform development** workloads installed. Switch to the **Individual components** tab. Make sure **C++ MFC for latest v142 build tools (x86 & x64)** and **C++ ATL for latest v142 build tools (x86 & x64)** are selected.

If you want to build for ARM/ARM64, also select the C++ MFC and ATL for latest v142 build tools and **C++ Universal Windows Platform support for v142 build tools** for the corresponding architecture.

When installed by Visual Studio, the Windows SDK doesn't have the **SDK Debugging Tools** installed. Please go to **Control Panel** → **Programs** → **Programs and Features** → Select the most recent **Windows Software Development Kit** → **Change** → **Change** → Select **Debugging Tools For Windows** → **Change**.

# Opening the developer command prompt

You'll need a command prompt configured for calling Visual Studio tools. The following are two options for opening a developer command prompt:

1. Using the shortcut in the start menu

Click on **Start Menu** → **Visual Studio 2019** → **x64 Native Tools Command Prompt for VS 2019**

or

2. Executing the batch file with the configuration

We can configure any command prompt (like **Windows Terminal**) with the environment variables for calling VS tools. You just need to call **"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"** from an opened command prompt.

# Acquiring WebRTC code base

## Getting depot_tools

WebRTC uses Chromium's build tools named **depot_tools**. You can download it with **curl** that is now shipped with Windows. The following command will download depot_tool.zip to the current folder.

```shell
curl https://storage.googleapis.com/chrome-infra/depot_tools.zip --output depot_tools.zip
```

Use the following command to unzip depot_tools in the root folder of the c: drive.

```shell
mkdir c:\depot_tools
tar -xf depot_tools.zip -C c:\depot_tools
```

Let's not clutter our dev box, so feel free to delete the depot_tools.zip file.

```shell
del depot_tools.zip
```

Set the path environment variable to execute commands in the depot_tools folder.

```shell
set PATH=c:\depot_tools;%PATH%
```

> Keep in mind that the previous command gave priority to programs sitting in that folder. One of the programs sitting in the depot_tools folder is a batch file named git. It means that for the duration of this terminal session, when you execute **git** you're calling Google's git, not the one was previously installed in your dev box.

## Setting up the environment

Let's inform depot_tools that we don't have access to Google's internal tools.

```shell
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
```

The GYP build tool must be informed about the version of the Visual Studio we're using.

```shell
set GYP_MSVS_VERSION=2019
```

Create the folder where the code base will be placed.

```shell
c:
mkdir c:\webrtc
cd c:\webrtc
```

## Downloading the bits

Tell to the gclient tool to initialize your local copy of the repos.

```shell
gclient
```

Request the tools to fetch the WebRTC code base. The following command will take time. Past experience shows that it might take around 1 hour and 10 minutes. 

```shell
fetch --nohooks webrtc
````

Change to the branch-heads/4147 branch. This is the commit that the UWP patches (see below) are based on. 

```shell
cd src
git checkout branch-heads/4147
```

Instruct the tools to bring the bits from all the sub repositories to your dev box. This may take a while.

```shell
gclient sync -D -r branch-heads/4147
```

## Applying the patches

The [patchWebRTCM84.cmd](https://github.com/microsoft/winrtc/blob/master/patches_for_WebRTC_org/m84/patchWebRTCM84.cmd) batch file needs to locate the WebRTC code base in order to be patched. The environment variable **WEBRTCM84_ROOT** should contain the path for the WebRTC code base you've just downloaded.

```shell
set WEBRTCM84_ROOT=c:\webrtc\src
```

Now, you just need to run the batch file that will patch all the necessary repos that form the WebRTC code base.

```shell
c:\WinRTC\patches_for_WebRTC_org\m84\patchWebRTCM84.cmd
```

# Setting up and building

## Setting up

WebRTC is an extensive project and not all of its modules are required for producing a UWP app with real time communications capabilities. Because of that, some parts of the WebRTC code base that do not build for UWP were not patched by the patchWebRTCM84.cmd script. As such, the following ninja command excludes the unnecessary modules and prepares to build for UWP.

```shell
gn gen --ide=vs2019 out\msvc\uwp\Release\x64 --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true enable_libaom=false rtc_enable_protobuf=false"
```

This will set up the `out\msvc\uwp\Release\x64` folder for building WebRTC for the UWP x64 Release configuration. Build settings can be customized. To view the full list of editable build arguments, you can run

```shell
gn args --list out\msvc\uwp\Release\x64
```

Please note that not all settings are supported in a UWP build.

## Building

Now that the project is set up, you can choose between building it inside Visual Studio or directly with the command line.

### With Visual Studio 2019

Open the generated Visual Studio solution with the following command:

```shell
devenv out\msvc\uwp\Release\x64\all.sln
```

Keep in mind that opening the all.sln solution from the Start Menu will render to different results. Calling Visual Studio from the current command prompt will seed Visual Studio with the environment variables defined earlier (DEPOT_TOOLS_WIN_TOOLCHAIN, GYP_MSVS_VERSION and PATH) and needed by VS2019 to build the solution from the IDE. 

To build on Visual Studio, make sure you can see the Solution Explorer window (**View** → **Solution Explorer**), then right-click on the webrtc project (it should be on the bottom of the window), and then click on **Select as Startup Project**. Finally, complete the build with **Build** → **Build Solution**.

### With command line

Run the following command to build the patched WebRTC from the command line.

```shell
ninja -C out\msvc\uwp\Release\x64
```

# Consuming the generated binaries

You'll need to set 5 pieces of configuration on your application for consuming this patched version of WebRTC. You'll find these information in the **webrtc.ninja** file inside the **out\msvc\uwp\Release\x64\obj** folder.

> The following instructions should work for Release builds on x64. For Debug builds and x86 builds you should harvest similar information from the webrtc.ninja file placed on its respective **obj** directory.

1. Add include folders

Click on the project name, then go to project properties (**Alt-F7**). Click on **Configuration Properties** → **C/C++** → **General** → **Additional Include Directories** and **add** the following paths:

```
c:\webrtc\src
c:\webrtc\src\out\msvc\uwp\$(Configuration)\$(PlatformTarget)\gen
c:\webrtc\src\third_party\abseil-cpp
c:\webrtc\src\third_party\libyuv\include
```

Click on **Apply**, but don't close the project properties window.

2. Preprocessor macros:

Click on **Preprocessor** → **Preprocessor Definitions** and **add** the following definitions:

```
USE_AURA=1;_HAS_EXCEPTIONS=0;__STD_C;_CRT_RAND_S;_CRT_SECURE_NO_DEPRECATE;_SCL_SECURE_NO_DEPRECATE;_ATL_NO_OPENGL;_WINDOWS;CERT_CHAIN_PARA_HAS_EXTRA_FIELDS;PSAPI_VERSION=2;WIN32;_SECURE_ATL;WINUWP;__WRL_NO_DEFAULT_LIB__;WINAPI_FAMILY=WINAPI_FAMILY_PC_APP;WIN10=_WIN32_WINNT_WIN10;WIN32_LEAN_AND_MEAN;NOMINMAX;_UNICODE;UNICODE;NTDDI_VERSION=NTDDI_WIN10_RS2;_WIN32_WINNT=0x0A00;WINVER=0x0A00;NDEBUG;NVALGRIND;DYNAMIC_ANNOTATIONS_ENABLED=0;WEBRTC_ENABLE_PROTOBUF=0;WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE;RTC_ENABLE_VP9;HAVE_SCTP;WEBRTC_LIBRARY_IMPL;WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0;WEBRTC_WIN;ABSL_ALLOCATOR_NOTHROW=1;HAVE_SCTP;WEBRTC_VIDEO_CAPTURE_WINRT
```

> Feel free to copy-and-paste with the semicolons in a single line. Visual Studio will expand them to multiple lines next time you open this UI.

Click on **Apply**, but don't close the project properties window.

3. Linker additional library path

Click on **Linker** → **General** → **Additional Library Directories** and **add** the following path:

```
c:\webrtc\src\out\msvc\uwp\$(Configuration)\$(PlatformTarget)\obj
```

Click on **Apply**, but don't close the project properties window.

4. WebRTC library name

Click on **Input** → **Additional Dependencies** and **add** the following file name:

```
webrtc.lib
```

5. Discarding libcmt from the linking

Still on the **Input** tab, click on **Ignore Specific Default Libraries** and add **libcmt**.

> It should be libcmt**d** on debug builds.

# Victory

Click on **OK** and you should be good to use WebRTC APIs on your UWP app! 
