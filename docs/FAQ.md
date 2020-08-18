# Frequently Asked Questions
We will update this page periodically.

## What is WebRTC?
WebRTC, or Web Real Time Communication, is an open source software for the web that is widely used to establish communication between two or more clients. It paves the road for real-time, plugin-free video, audio and data communication and enables  developers to build powerful video, audio, and data communication solutions. 
	
## Why was WebRTC developed? 
Let’s say Alice wants to talk to Bob. One possible way for them to coordinate the communication is by using a Signaling Server through which they can start the peer-to-peer communication. However, if Alice and Bob are behind two different firewalls, they will have to use a STUN Server to figure out each other's public IP addresses. After that, they might need to use a Release Server to relay the communication. As a result, the procedure required to establish a secure connection between two peers can become complicated quickly, which is why WebRTC was introduced to simplify the process. 

## What is WinRTC, and how does it relate to WebRTC?
WinRTC brings the beauty of real time communication outside of the web and into the windows native development platform. Built upon the pre-existing WebRTC architecture, WinRTC is Microsoft's open-source software which enables real time communication for native windows applications. It helps to maximize efficiency for developers by having a lot less code, a lot less integration pieces to deal with, and a lot less packages to install.
	
## What are our guiding principles for building WinRTC?  
Our focus lies on enabling compatibility and key interoperability for our developers so that they are not only able to call any web native API, but also be aligned with the community's standards and with the larger WebRTC code base. 

## What is the architecture of WinRTC?

The overall architecture looks something like this:

<img alt="WinRTC architecture" src="https://raw.githubusercontent.com/microsoft/winrtc/master/docs/WinRTC_Architecture_Diagram.png">

WinRTC's compatibility extends to .Net and RN4W, as well as UWP and Win32 applications. Since the entire infrastructure of WinRTC is build on top of the WebRTC foundation, our team works hard to ensure that what we are building aligns with the larger WebRTC community's standards. 

### Windows Native Development Platform

#### [`Mixed Reality SDK`](https://docs.microsoft.com/en-us/windows/mixed-reality/unity-development-overview)
Mixed Reality Toolkit (MRTK) provides a set of components and features to accelerate cross-platform MR app development in Unity. The Mixed Reality SDK is purely UWP so this layer sits directly on top of WebRTC.  

#### [`.Net`](https://dotnet.microsoft.com/)
.Net is a free, cross-platform, open source developer platform for building many different types of applications. With .NET, you can use multiple languages, editors, and libraries to build for web, mobile, desktop, games, and IoT. When incorporating WinRTC, developers have the option to either use the XAML control or go directly to the WinRTC component.  

#### [`React Native for Windows (RN4W)`](https://microsoft.github.io/react-native-windows/) 
React Native is an open-source mobile application framework created by Facebook. React Native for Windows brings React Native support for the Windows 10 SDK. With this, you can use JavaScript to build native Windows apps for all devices supported by Windows 10 including PCs, tablets, 2-in-1s, Xbox, Mixed reality devices, etc. When incorporating WinRTC with React Native, use the XAML control on top of the WinRTC for the best WinRTC experience. 

#### [`Universal Windows Platform (UWP) App`](https://docs.microsoft.com/en-us/windows/apps/desktop/choose-your-platform#uwp)
The Universal Windows Platform provides a common type system, APIs, and application model for all devices that run Windows 10. UWP applications can be native or managed. When incorporating WinRTC, developers have the option to either include a XAML control, or go directly to using WinRTC, or even skip both and go straight to the WebRTC layer.

#### [`Win32 App`](https://docs.microsoft.com/en-us/windows/apps/desktop/choose-your-platform#win32) 
Win32 is the original platform for native C/C++ Windows applications that require direct access to Windows and hardware. This makes the Win32 API the platform of choice for applications that need the highest level of performance and direct access to system hardware. Developers creating a Win32 app have the option to incorporate WinRTC or WebRTC based on their needs. 

### Web Developer Platform

#### [`WebApp`](https://docs.microsoft.com/en-us/azure/devops/pipelines/targets/webapp?view=azure-devops&tabs=yaml)
WebApps, provided under Azure App Service, are an HTTP-based service for hosting web applications, REST APIs, and mobile back ends. Developers can use their favorite language, be it .NET, .NET Core, Java, Ruby, Node.js, PHP, or Python. Applications run and scale with ease on both Windows and Linux-based environments. To incorporate real time communication, WebApps should interact directly with WebRTC.  

#### `Progressive Web App (PWA)` 
A progressive web application is a type of application software delivered through the web, built using common web technologies including HTML, CSS and JavaScript. To incorporate real time communication, Progressive Web Apps should interact directly with WebRTC.  

#### `Chrome` 
Google Chrome is a cross-platform web browser developed by Google. It was first released in 2008 for Microsoft Windows, and was later ported to Linux, macOS, iOS, and Android where it is the default browser built into the OS.

## I don't see my question here! [Create an issue to ask a question or start a discussion](https://github.com/microsoft/winrtc/issues/new/choose).
