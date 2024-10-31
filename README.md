![Showreel.001.png](Resources/Documentation/Showreel/Showreel.001.png "MenuBarMatrix Headline Icons")
![Showreel.002.png](Resources/Documentation/Showreel/Showreel.002.png "MenuBarMatrix Headline Icons")

See [LATEST RELEASE](../../releases/latest) for available binary packages or join iOS TestFlight Beta:

<img src="Resources/AppStore/TestFlightQRCode.png" alt="TestFlight QR Code" width="15%">


|Appveyor CI build status|MenuBarMatrix|MenuBarMatrixMonitor|
|:----------------|:-----|:-----|
|macOS Xcode| [![Build status](https://ci.appveyor.com/api/projects/status/m1jqb2d19wv9e74o/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-06mmy) |
|Windows Visual Studio| [![Build status](https://ci.appveyor.com/api/projects/status/2wi23e7i44xh1fye/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix) |
|Linux makefile| [![Build status](https://ci.appveyor.com/api/projects/status/3lwra86k9i0233io/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-y20bx) |


<a name="toc" />

## Table of contents

* [Introduction](#introduction)
* [Usecase: Studio sidecar monitoring](#rackmonitoringusecase)
* [Usecase: Mobile recording monitoring](#mobilerecordingusecase)
* [App Architecture](#architectureoverview)


<a name="introduction" />

## Introduction

MenuBarMatrix is a programming experiment to try out how a macOS menu bar tool can be made that provides audio matrix routing functionality using JUCE framework - e.g. to route BlackHole 16ch virtual device to system output to overcome AppleMusic behaviour on macOS to only play out higher order surround sound formats when exactly 16 output channels are presented by selected system audio output device.

It is accompanied by a separate tool MenuBarMatrixMonitor to monitor the incoming and outgoing audio via network. It connects to MenuBarMatrix through a TCP connection and supports discovering the available instances through a multicast service announcement done by ManuBarMatrix.

Its sourcecode and prebuilt binaries are made publicly available to enable interested users to experiment, extend and create own adaptations.

Use what is provided here at your own risk!

![Showreel.003.png](Resources/Documentation/Showreel/Showreel.003.png "MenuBarMatrix UI")


<a name="rackmonitoringusecase" />

## Usecase: Studio sidecar monitoring

![UseCase-RackMonitoring.png](Resources/Documentation/UseCase-RackMonitoring.png "Homestudio setup")


<a name="mobilerecordingusecase" />

## Usecase: Mobile recording monitoring

![UseCase-RackMonitoring.png](Resources/Documentation/UseCase-RackMonitoring.png "Mobile rig")