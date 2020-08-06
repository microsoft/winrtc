# Patching WebRTC for Windows

This document will walk you through the steps required for getting the WebRTC code base, patching it and building it for UWP. These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet.

## Prerequisites

#### Machine requirements 
- Windows 10 (build 18362) or later.
- At least 8GB of RAM (16GB of RAM is recommended). 
- At least 15GB of disk space. 
- SSD drive formatted with NTFS. 

#### External applications
- [Microsoft Visual Studio 2019 16.6.2](http://visualstudio.com). 
- Command Prompt ([Windows Terminal](https://www.microsoft.com/en-us/p/windows-terminal/9n0dx20hk701?activetab=pivot:overviewtab) is recommended, but cmd.exe works as well).

In the **Visual Studio Installer** app, please verify if Visual Studio 2019 has the **Desktop development with C++** and **Universal Windows Platform development** workloads installed. Switch to the **Individual components** tab. Make sure **C++ v14.24 MFC for v142 build tools (x86 & x64)** and **C++ v14.24 ATL for v142 build tools (x86 & x64)** are selected.

If you want to build for ARM/ARM64, also select the C++ MFC and ATL v142 build tools and **C++ Universal Windows Platform support for v142 build tools** for the corresponding architecture.

When installed by Visual Studio, the Windows SDK doesn't have the **SDK Debugging Tools** installed. Please go to **Control Panel** → **Programs** → **Programs and Features** → Select the most recent **Windows Software Development Kit** → **Change** → **Change** → Select **Debugging Tools For Windows** → **Change**.

# Opening the developer command prompt

You'll need a command prompt configured for calling Visual Studio tools. The following are two options for opening a developer command prompt:

1. Using the shortcut in the start menu

Click on the **Start Menu** → **Visual Studio 2019** → **x64 Native Tools Command Prompt for VS 2019**

or

2. Executing the batch file with the configuration

We can configure any command prompt (like **Windows Terminal**) with the environment variables for calling VS tools. You just need to call **"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"** from an opened command prompt.

# Acquiring WebRTC code base

## Getting depot_tools

WebRTC uses Chromium's build tools named **depot_tools**. Let's start by cloning the depot_tools repo in the root of the c: drive.

```shell
c:
cd \
git clone https://chromium.googlesource.com/chromium/tools/depot_tools
```

Let's switch to the m80 branch of depot_tools and bring the most recent bits for this branch to your machine.

```shell
cd depot_tools
git checkout chrome/3987
git fetch
```

depot_tools has some tools that are not part of the repo and need to be downloaded. Let's download these extra tools. This might take a few minutes.

```shell
gclient
```

At this moment, gclient will offer you to use the most recent version of depot_tools. We do **not** want that.

```shell
Your depot_tools checkout is configured to fetch from an obsolete URL
Would you like to update it? [y/N]: N
```

Set the path environment variable to execute commands in the depot_tools folder.

```shell
set PATH=c:\depot_tools;%PATH%
```

> Keep in mind that the previous command gave priority to programs sitting in that folder. One of the programs sitting in the depot_tools folder is a batch file named git. It means that for the duration of this terminal session, when you execute **git** you're calling Google's git, not the one was previously installed in your dev box.

## Installing Python 2.7

Python 2.7 reached its end-of-life, but it is still required to build WebRTC m80 for Windows. Let's install Python 2.7 and pywin32 in depot_tools.

Make sure that any python program executed in this session comes from the depot_tools.

```shell
where python
```

The response should be **c:\depot_tools\python.bat**.

Now that we're using the right python version from depot_tool, let's install pip and pywin32.

Let's first install pip.

```shell
python -m pip install --upgrade pip
```

Let's figure out where pip was installed.

```shell
cd c:\depot_tools
where /r . pip.exe
```

at the moment this is being written, the answer is **c:\depot_tools\bootstrap-3_8_0_chromium_8_bin\python\bin\Scripts\pip.exe**, but it might be different on your machine.

Change the current folder to the one with pip and then install pywin32.

```shell
cd c:\depot_tools\bootstrap-3_8_0_chromium_8_bin\python\bin\Scripts
pip install pywin32
```

> Don't worry about the deprecation notice from Python 2.7. Migrating to Python3 is a working in progress that didn't make it to the m80 branch.

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

Request the tools to fetch the WebRTC code base. The following command will take time. Past experience shows that it might take around 1 hour an 10 minutes. 

```shell
fetch --nohooks webrtc
````

Change to the branch-heads/3987 branch. This is the commit that the UWP patches (see below) are based on. You can use a different commit but the patches might not apply correctly.

```shell
cd src
git checkout branch-heads/3987
```

Instruct the tools to bring the bits from all the sub repositories to your dev box. This will take awhile.

```shell
gclient sync
```

## Applying the patches

As is, WebRTC doesn't build for modern Windows (UWP). Probably you already know that and it is why you're reading this. So, now is time to patch WebRTC for building for UWP.

There is a batch file named **patchWebRTC.cmd** in the folder **patches_for_WebRTC_org\m80** of the **WinRTC** repo for patching WebRTC. The patchWebRTC.cmd batch file needs to know where is the WebRTC code base to be patched. The environment variable **WEBRTCM80_ROOT** should contain the path for the WebRTC code base you've just downloaded.

```shell
set WEBRTCM80_ROOT=c:\webrtc\src
```

Now, you just need to run the batch file that will patch all the necessary repos the form the WebRTC code base.

```shell
c:\WinRTC\patches_for_WebRTC_org\m80\patchWebRTCM80.cmd
```

# Setting up the and building

## Setting up

WebRTC is a extensive project and not all of its modules are required for producing an UWP app with real time communications capabilities. Because of that, some parts of the WebRTC code base that do not build for UWP were not patched by the patchWebRTC.cmd script. You don't need to worry, the following ninja command excludes the unnecessary modules and prepares to build the drop for UWP.

```shell
gn --ide=vs2019 gen out\msvc\x64\Release --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false rtc_win_use_mf_h264=true"
```

This will set up the `out\msvc\x64\Release` folder for building WebRTC for the UWP x64 Release configuration. Build settings can be customized. To view the full list of editable build arguments, you can run

```shell
gn args --list out\msvc\x64\Release
```

Please note that not all settings are supported in a UWP build.

## Building

One you have the project set up, you can choose between building it inside Visual Studio or directly with the command line.

### With Visual Studio 2019

Assuming that your terminal has Visual Studio in the path, you can open the generated solution file by running the following command.

```shell
devenv out\msvc\x64\Release\all.sln
```

To build on Visual Studio, make sure you can see the Solution Explorer window (**View** → **Solution Explorer**), then right-click on the webrtc project (it should be on the bottom of the window), and then click on **Select as Startup Project**.
Now is just build the solution with **Build** → **Build Solution**.

### With command line

Run the following command to build the patched WebRTC from the command line.

```shell
ninja -C out\msvc\x64\Release
```

# Consuming the generated binaries

You'll need to set 5 pieces of configuration on your application for consuming this patched version of WebRTC. You'll find these information in the **webrtc.ninja** file inside the **out\msvc\x64\Release\obj** folder.

> The following instructions should work for Release builds on x64. For Debug builds and x86 builds you should harvest similar information from the webrtc.ninja file placed on its respective **obj** directory.

1. Add include folders

Click on the project name, then go to project properties (**Alt-F7**). Click on **Configuration Properties** → **C/C++** → **General** → **Additional Include Directories** and **add** the following paths:

```
c:\webrtc\src
c:\webrtc\src\out\msvc\$(PlatformTarget)\$(Configuration)\gen
c:\webrtc\src\third_party\abseil-cpp
c:\webrtc\src\third_party\libyuv\include
c:\webrtc\src\third_party\protobuf\src
c:\webrtc\src\out\msvc\$(PlatformTarget)\$(Configuration)\gen\protoc_out
ration)\gen\protoc_out
```

Click on **Apply**, but don't close the project properties window.

2. Preprocessor macros:

Click on **Preprocessor** → **Preprocessor Definitions** and **add** the following definitions:

```
USE_AURA=1;_HAS_EXCEPTIONS=0;__STD_C;_CRT_RAND_S;_CRT_SECURE_NO_DEPRECATE;_SCL_SECURE_NO_DEPRECATE;_ATL_NO_OPENGL;_WINDOWS;CERT_CHAIN_PARA_HAS_EXTRA_FIELDS;PSAPI_VERSION=2;WIN32;_SECURE_ATL;_USING_V110_SDK71_;WINUWP;__WRL_NO_DEFAULT_LIB__;WINAPI_FAMILY=WINAPI_FAMILY_PC_APP;WIN10=_WIN32_WINNT_WIN10;WIN32_LEAN_AND_MEAN;NOMINMAX;_UNICODE;UNICODE;NTDDI_VERSION=NTDDI_WIN10_RS2;_WIN32_WINNT=0x0A00;WINVER=0x0A00;NVALGRIND;DYNAMIC_ANNOTATIONS_ENABLED=0;WEBRTC_ENABLE_PROTOBUF=1;WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE;RTC_ENABLE_VP9;HAVE_SCTP;WEBRTC_LIBRARY_IMPL;WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0;WEBRTC_WIN;ABSL_ALLOCATOR_NOTHROW=1;GOOGLE_PROTOBUF_NO_RTTI;GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
```

> Feel free to copy-and-paste with the semicolons in a single line. Visual Studio will expand them to multiple lines next time you open this UI.

Click on **Apply**, but don't close the project properties window.

3. Linker additional library path

Click on **Linker** → **General** → **Additional Library Directories** and **add** the following path:

```
c:\webrtc\src\out\msvc\$(PlatformTarget)\$(Configuration)\obj
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

Click on **OK** and you should be good to use WebRTC APIs on your UWP app! :)
