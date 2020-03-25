# Patching WebRTC build for modern Windows

This document will walk you through the steps required for getting WebRTC code base, patching it and build it for UWP.

These patches are being contributed back. Some of these patches were already merged into their original repos, but didn't rolled over WebRTC yet. Contribute changes back take time. We want you to be able to build Windows apps with real time communications as soon as possible.

Building WebRTC from scratch is extremely powerful, but it is a lot of work. If you are looking for shipping a limited set of codecs or greater control of the binary size, building from scratch is the way to go. However, we believe that most developers would be ok with a more off of shelf solution.

We're also investigating the feasibility of having NuGet packages of a patched WebRTC. Building against WebRTC requires a considerable amount of header files from multiple different repos. Besides that, there is the problem of the sheer size of the binaries produced. Stay tuned because this repo will be updated if/when a NuGet package is available.

# Prerequisites

To build WebRTC rapidly requires a biffy machine. Make sure your build rig is a **64-bit Intel** based machine running **Windows 10 build 18362 or more recent**. It builds on machines with 8GB of RAM, but at least **16GB of RAM** is recommended. It also requires a seriously amount of disk space. Make sure you have at least **100GB available**. Finally, make sure you **SSD** drive is formatted with **NTFS**.

You'll also need **Microsoft Visual Studio 2019**. Download your favorite flavor of Visual Studio 2019 from [http://visualstudio.com](http://visualstudio.com). In the **Visual Studio Installer** app, please verify if Visual Studio 2019 has the **Desktop development with C++** and **Universal Windows Platform development** workloads installed. Switch to the **Individual components** tab. Make sure **C++ MFC for latest v142 build tools (x86 & x64)** and **C++ ATL for latest v142 build tools (x86 & x64)** are selected.

When installed by Visual Studio, the Windows SDK doesn't have the **SDK Debugging Tools** installed. Please go to **Control Panel** → **Programs** → **Programs and Features** → Select the most recent **Windows Software Development Kit** → **Change** → **Change** → Select **Debugging Tools For Windows** → **Change**.

You'll be running commands in the console. You can use the good old cmd.exe for that, but we do recommend using the [Windows Terminal](https://www.microsoft.com/en-us/p/windows-terminal-preview/9n0dx20hk701?activetab=pivot:reviewstab).

# Acquiring WebRTC code base

Open the **Windows Terminal** and type the following commands:

## Getting depot_tools

WebRTC uses Chromium's build tools named **depot_tools**. You can download it with **curl** that is now shipped with Windows. The following command will download depot_tool.zip to the current folder.

```shell
curl https://storage.googleapis.com/chrome-infra/depot_tools.zip --output depot_tools.zip
```

Use the following command to open the zip file in File Explorer.

```shell
RunDll32.exe zipfldr.dll,RouteTheCall depot_tools.zip
```

Click on the **Extract all** button and extract the contents of the zip file into the root of the C: drive (**C:\depot_tools**).

Let's not clutter our dev box, so feel free to delete the depot_tools.zip file.

```shell
del depot_tools.zip
```

## Setting up the environment

Set the path environment variable to execute commands in the depot_tools folder.

```shell
set PATH=c:\depot_tools;%PATH%
```

> Keep in ming that the previous command gave priority to programs sitting in that folder. One of the programs sitting in the depot_tools folder is a batch file named git. It means that for the duration of this terminal session, when you execute **git** you're calling Google's git, not the one was previously installed in your dev box.

Make sure that any python program executed in this session comes from the depot_tools.

```shell
where python
```

The response should be **c:\depot_tools\python.bat**.

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

Request the tools to fetch the WebRTC code base.

```shell
fetch --nohooks --no-history webrtc
````

Instruct the tools to bring the bits from all the sub repositories to your dev box. This will take awhile.

```shell
gclient sync
```

## Applying the patches

As is, WebRTC doesn't build for modern Windows (UWP). Probably you already know that and it is why you're reading this. So, now is time to patch WebRTC for building for UWP.

There is a batch file named **patchWebRTC.cmd** in the folder **patches_for_WebRTC_org/master--08b11caf** of the **WinRTC** repo for patching WebRTC. The patchWebRTC.cmd batch file needs to know where is the WebRTC code base to be patched. The environment variable **WEBRTC_MASTER_ROOT** should contain the path for the WebRTC code base you've just downloaded.

```shell
set WEBRTC_MASTER_ROOT=c:\webrtc\src
```

Now, you just need to run the batch file that will patch all the necessary repos the form the WebRTC code base.

```shell
patchWebRTC.cmd
```

# Setting up the and building

## Setting up

WebRTC is a extensive project and not all of its modules are required for producing an UWP app with real time communications capabilities. Because of that, some parts of the WebRTC code base that do not build for UWP were not patched by the patchWebRTC.cmd script. You don't need to worry, the following ninja command excludes the unnecessary modules and prepares to build the drop for UWP.

```shell
gn --ide=vs2019 gen out\msvc\x64\Release --filters=//:webrtc "--args=is_debug=false use_lld=false is_clang=false rtc_include_tests=false rtc_build_tools=false rtc_win_video_capture_winrt=true target_os=\"winuwp\" rtc_build_examples=false"
```

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

You'll need to set 4 pieces of configuration on your application for consuming this patched version of WebRTC. You'll find these information in the **webrtc.ninja** file inside the **out\msvc\x64\Release\obj** folder.

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
USE_AURA=1
_HAS_EXCEPTIONS=0
__STD_C
_CRT_RAND_S
_CRT_SECURE_NO_DEPRECATE
_SCL_SECURE_NO_DEPRECATE
_ATL_NO_OPENGL
_WINDOWS
CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
PSAPI_VERSION=2
WIN32
_SECURE_ATL
_USING_V110_SDK71_
WINUWP
__WRL_NO_DEFAULT_LIB__
WINAPI_FAMILY=WINAPI_FAMILY_PC_APP
WIN10=_WIN32_WINNT_WIN10
WIN32_LEAN_AND_MEAN
NOMINMAX
_UNICODE
UNICODE
NTDDI_VERSION=NTDDI_WIN10_RS2
_WIN32_WINNT=0x0A00
WINVER=0x0A00
NVALGRIND
DYNAMIC_ANNOTATIONS_ENABLED=0
WEBRTC_ENABLE_PROTOBUF=1
WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
RTC_ENABLE_VP9
HAVE_SCTP
WEBRTC_LIBRARY_IMPL
WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0
WEBRTC_WIN
ABSL_ALLOCATOR_NOTHROW=1
GOOGLE_PROTOBUF_NO_RTTI
GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
```

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

Click on **OK** and you should be good to use WebRTC APIs on your UWP app! :)