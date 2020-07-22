# Frequently Asked Questions
We will update this page periodically.

## What is WebRTC?
WebRTC, or Web Real Time Communication, is an open source software for the web that is widely used to establish communication between two or more clients. It paves the road for real-time, plugin-free video, audio and data communication and enables  developers to build powerful video, audio, and data communication solutions. 
	
## How are WebRTC and WinRTC related?
Built upon the pre-existing WebRTC architecture, WinRTC is Microsoft's open source software which enables real time communications for native windows applications. WinRTC streamlines the process of bringing WebRTC to Windows in more than just the browser by making it easier for our developers to use by having a lot less code, a lot less integration pieces to deal with and a lot less packages to install. 
	
## Why was Web/WinRTC developed? 
Let's say Alice wants to talk to Bob. One possible way for them to coordinate the communication is by using a Signaling Server through which they can start the peer-to-peer communication. However, if Alice and Bob are behind two different firewalls they will have to use a STUN Server to figure out each other's public IP addresses. After that, they might need to use a Release Server to relay the communication. As a result, the procedure required to establish a secure connection between two peers can become complicated quickly, which is why WinRTC has been introduced to simply the process. 
	
## What are our guiding principles for building WinRTC?  
Our focus lies on enabling compatibility and key interoperability for our developers so that they are not only able to call any web native APIs, but also be aligned with the community's standards and with the larger WebRTC code base. We also want to ensure that we contribute back the community standard. 

## What is the architecture of WinRTC?

<PIC>

The diagram above showcases the architecture of WinRTC. On the right side of the dotted green line, we have the web developer platform, where developers can build a Progressive Web App (PWA) or a WebApp. This is built on top of the Chrome/Anaheim framework, which sits on top of a Win32 App. This entire experience is built upon the foundation laid down by WebRTC.org

On the left side of the diagram we have the windows native platform experience, where WinRTC enables developers to include real time communication in their native windows applications. 

Starting from the left side of the dotted green line, 
* First developer builds a Win32 app, which sits on the WinRTC library, and that sits on WebRTC.org
* Second developer builds a UWP app and they have the option to either include a XAML control, or go straight to using WinRTC, or even skip both of these and go straight to the WebRTC.org layer. 
* Third developer builds a React Native for Windows application and that uses the XAML control on top of the WinRTC for the best WinRTC experience. 
* Fourth developer builds a .Net app, which has the option to either use the XAML control or go straight to the WinRTC component. 
* Fifth developer building a MixedReality SDK. This is purely UWP so this layer sits directly on top of WebRTC.org. 

Since the entire infrastructure of WinRTC is build atop the WebRTC foundation, our team works hard to ensure that what we are building aligns with the larger WebRTC community's standards. 

## I don't see my question here! [Create an issue to ask a question or start a discussion](https://github.com/microsoft/ProjectReunion/issues/new/choose).
