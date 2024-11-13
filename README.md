![Showreel.001.png](Resources/Documentation/Showreel/Showreel.001.png "MenuBarMatrix Headline Icons")
![Showreel.002.png](Resources/Documentation/Showreel/Showreel.002.png "MenuBarMatrix Headline Icons")

See [LATEST RELEASE](https://github.com/ChristianAhrens/MenuBarMatrix/releases/latest) for available binary packages or join iOS TestFlight Beta:

<img src="Resources/AppStore/TestFlightQRCode.png" alt="TestFlight QR Code" width="15%">


|Appveyor CI build status|MenuBarMatrix|MenuBarMatrixMonitor|
|:----------------|:-----|:-----|
|macOS Xcode| [![Build status](https://ci.appveyor.com/api/projects/status/m1jqb2d19wv9e74o?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-06mmy) | [![Build status](https://ci.appveyor.com/api/projects/status/va72we76lhlkxxs5?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-ubusx) |
|Windows Visual Studio| [![Build status](https://ci.appveyor.com/api/projects/status/2wi23e7i44xh1fye?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix) | [![Build status](https://ci.appveyor.com/api/projects/status/hp8ixwxa5ccumjcj?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-t97el) |
|Linux makefile| [![Build status](https://ci.appveyor.com/api/projects/status/3lwra86k9i0233io?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-y20bx) | [![Build status](https://ci.appveyor.com/api/projects/status/j0qj6sqtaa5mq4dc?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-vhiu1) |


<a name="toc" />

## Table of contents

* [Introduction](#introduction)
  * [MenuBarMatrix and MenuBarMatrixMonitor](#menubarmatrixNmonitor)
  * [MenuBarMatrix UI](#menubarmatrixUI)
  * [MenuBarMatrixMonitor UI](#menubarmatrixmonitorUI)
* [How to build the tools](#howtobuild)
  * [MenuBarMatrix](#buildmenubarmatrix)
  * [MenuBarMatrixMonitor](#buildmenubarmatrixmonitor)
    * [Note on running on RaspberryPiOS Bullseye](#runonbullseye)
* [Usecase: Studio sidecar monitoring](#rackmonitoringusecase)
* [Usecase: Mobile recording monitoring](#mobilerecordingusecase)
* [App Architecture](#architectureoverview)


<a name="introduction" />

## Introduction

MenuBarMatrix is an project initially created to try out if a macOS menubar tool based on JUCE C++ framework can be created, that provides audio matrix routing functionality - e.g. to route [BlackHole](https://github.com/ExistentialAudio/BlackHole) 16ch virtual device to system output to overcome AppleMusic behaviour on macOS to only play out higher order surround sound formats when exactly 16 output channels are presented by selected system audio output device.

It is accompanied by a separate tool MenuBarMatrixMonitor to monitor the audio IO as levelmeters via network. It connects to MenuBarMatrix through a TCP connection and supports discovering the available instances through a multicast service announcement done by ManuBarMatrix.

Its sourcecode and prebuilt binaries are made publicly available to enable interested users to experiment, extend and create own adaptations.

Use what is provided here at your own risk!

<a name="menubarmatrixNmonitor" />

### MenuBarMatrix and MenuBarMatrixMonitor

![Showreel.003.png](Resources/Documentation/Showreel/Showreel.003.png "MenuBarMatrix and -Monitor in action")

<a name="menubarmatrixUI" />

### MenuBarMatrix UI details

![Showreel.004.png](Resources/Documentation/Showreel/Showreel.004.png "MenuBarMatrix UI")

<a name="menubarmatrixmonitorUI" />

### MenuBarMatrixMonitor UI details

![Showreel.005.png](Resources/Documentation/Showreel/Showreel.005.png "MenuBarMatrixMonitor UI")


<a name="howtobuild" />

## How to build the tools

MenuBarMatrix and MenuBarMatrixMonitor are based on JUCE C++ framework, which is a submodule of this repository.

JUCE's Projucer tool can either be used from a local installation or from within the submodule (submodules/JUCE/extras/Projucer).

<a name="buildmenubarmatrix" />

### MenuBarMatrix

[MenuBarMatrix Projucer project](MenuBarMatrix.jucer) file can be found in repository root directory.

In [macOS buildscripts](Resources/Deployment/macOS), shellscripts for automated building of the app, dmg and notarization are kept. These require a properly prepared machine to run on (signing certificates, provisioning profiles, notarization cretentials).

In [iOS buildscripts](Resources/Deployment/iOS), shellscripts for automated building of the app and updloading to the appstore are kept. These require a properly prepared machine to run on (appstore cretentials).

In [Windows buildscripts](Resources/Deployment/Windows), bash scripts for automated building of the app and installer (Innosetup based) are kept. These require a properly prepared machine to run on (innosetup installation).

In [Linux buildscripts](Resources/Deployment/Linux), shell scripts for automated building of the app are kept. These are aimed at building on Debian/Ubuntu/RaspberryPiOS and TRY to collect the required dev packages via apt packetmanager automatically.

<a name="buildmenubarmatrixmonitor" />

### MenuBarMatrixMonitor

[MenuBarMatrixMonitor Projucer project](MenuBarMatrixMonitor/MenuBarMatrixMonitor.jucer) file can be found in /MenuBarMatrixMonitor subdirectory .

In [macOS buildscripts](Resources/Deployment/macOS), shellscripts for automated building of the app, dmg and notarization are kept. These require a properly prepared machine to run on (signing certificates, provisioning profiles, notarization cretentials).

In [iOS buildscripts](Resources/Deployment/iOS), shellscripts for automated building of the app and updloading to the appstore are kept. These require a properly prepared machine to run on (appstore cretentials).

In [Windows buildscripts](Resources/Deployment/Windows), bash scripts for automated building of the app and installer (Innosetup based) are kept. These require a properly prepared machine to run on (innosetup installation).

In [Linux buildscripts](Resources/Deployment/Linux), shell scripts for automated building of the app are kept. These are aimed at building on Debian/Ubuntu/RaspberryPiOS and TRY to collect the required dev packages via apt packetmanager automatically.

<a name="runonbullseye" />

#### Building and running MenuBarMatrixMonitor on RaspberryPiOS Bullseye

The build script `build_MenuBarMatrixMonitor_RaspberryPIOS.sh` in `Resources/Deployment/Linux` can be used on a vanilla installation of RaspberryPi OS to build the tool.

On RaspberriPi 3B it is required to run the build without graphical interface, to avoid the build failing due to going out of memory (e.g. `sudo raspi-config` -> System Options -> Boot -> Console Autologin).

The build result can be run in kind of a kiosk configuration by changing the system to not start the desktop session when running Xserver, but instead run MenuBarMatrixMonitor directly in the X session. To do this, edit or create `.xsession` in user home and simply add a line
```
exec <PATH_TO_REPO>/MenuBarMatrix/MenuBarMatrixMonitor/Builds/LinuxMakefile/build/MenuBarMatrixMonitor
```
Then configure the system to auto login to x session (e.g. `sudo raspi-config` -> System Options -> Boot -> Desktop Autologin).

This does only work up to Raspberry Pi OS Bullseye versions, that use X server as window manager. From Bookworm on, Wayland is used that requires differing approaches.

<a name="rackmonitoringusecase" />

## Usecase: Studio rack monitoring

![Showreel.006.png](Resources/Documentation/Showreel/Showreel.006.png "Homestudio setup")

![Showreel.007.png](Resources/Documentation/Showreel/Showreel.007.png "RaspberryPi rack DIY device")

* MenuBarMatrix on macOS
  * BlackHole 16ch used to route signal from LogicPro, Apple Music, etc. to MenuBarMatrix
  * Output to Allen&Heath QU-16 22ch audio driver interface
* MenuBarMatrixMonitor on DIY 19" rack display, based on RaspberryPi (32bit RaspberryPiOS, Bullseye)
  * 16 audio input channel metering visible
  * 22 audio output channel metering visible


<a name="mobilerecordingusecase" />

## Usecase: Mobile recording monitoring

![Showreel.008.png](Resources/Documentation/Showreel/Showreel.008.png "Mobile rig")

* MenuBarMatrix on macOS
  * BlackHole 16ch used to route signal from LogicPro, Apple Music, etc. to MenuBarMatrix
  * Output to stereo audio driver interface
* MenuBarMatrixMonitor on iPadOS in Stagemanager mode
  * 16 audio input channel metering visible
  * 2 audio output channel metering visible


<a name="architectureoverview" />

## App Architecture

_T.B.D._