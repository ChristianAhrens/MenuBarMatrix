![Showreel.001.png](Resources/Documentation/Showreel/Showreel.001.png "MenuBarMatrix Headline Icons")

MenuBarMatrix is a programming experiment to try out how a macOS menu bar tool can be made that provides audio matrix routing functionality using JUCE framework - e.g. to route BlackHole 16ch virtual device to system output to overcome AppleMusic behaviour on macOS to only play out higher order surround sound formats when exactly 16 output channels are presented by selected system audio output device.

It is accompanied by a separate tool MenuBarMatrixMonitor to monitor the incoming and outgoing audio via network. It connects to MenuBarMatrix through a TCP connection and supports discovering the available instances through a multicast service announcement done by ManuBarMatrix.

Its sourcecode and prebuilt binaries are made publicly available to enable interested users to experiment, extend and create own adaptations.

Use what is provided here at your own risk!

|Appveyor CI build|Status|
|:----------------|:-----|
|Windows Visual Studio| [![Build status](https://ci.appveyor.com/api/projects/status/2wi23e7i44xh1fye/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix) |
|macOS Xcode| [![Build status](https://ci.appveyor.com/api/projects/status/m1jqb2d19wv9e74o/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-06mmy) |
|Linux makefile| [![Build status](https://ci.appveyor.com/api/projects/status/3lwra86k9i0233io/branch/main?svg=true)](https://ci.appveyor.com/project/ChristianAhrens/menubarmatrix-y20bx) |

![Showreel.002.png](Resources/Documentation/Showreel/Showreel.002.png "MenuBarMatrix UI")
