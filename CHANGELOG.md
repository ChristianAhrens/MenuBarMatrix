# Mema Changelog
All notable changes to Mema will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [0.3.2] 2024-12-05
### Added
- Added configurable 2D surround field visualization to Mema.Mo

### Changed

### Fixed
- Fixed Mema crosspoint patch to not be reset to unity when new Mema.Mo connections are established

## [0.3.1] 2024-11-23
### Added
- Added automatic update detection

### Changed

### Fixed

## [0.3.0] 2024-11-16
### Added

### Changed
- Project is now called 'Mema' accompanied by 'Mema.Mo' app

### Fixed
- Fixed storing and loading audio IO setup to config file

## [0.2.2] 2024-11-13
### Added
- Added visualization of network connections load incl. ovl to MenuBarMatrix
- Added support for portrait UI layout to MenuBarMatrixMonitor

### Changed

### Fixed
- Fixed potential macOS silent crash by checking network connection connected state before sending

## [0.2.1] 2024-11-06
### Added
- Added support for multiple MenuBarMatrixMontor connections to single MenuBarMatrix instance
- Added network decoupling from UI incl. dedicated network load monitoring per connection

### Changed

### Fixed
- Fixed MenuBarMatrix service advertisement for Windows

## [0.2.0] 2024-11-01
### Added
- Added improved README contents
- Added AppVeyor CI for macOS/Windows/Linux

### Changed
- Changed macOS app Sandbox to on
- Improved performance

### Fixed
- Fixed macOS, iOS provisioning
- Fixed typo and UI sizing issues 

## [0.1.1] 2024-10-26
### Added
- Added capability to manually select LookAndFeel to MenuBarMatrixMonitor
- Added capability to manually disconnect from host to MenuBarMatrixMonitor

### Changed

### Fixed
- Fixed automated build environment for macOS, Windows, Linux
- Fixed incorrect link to github in MenuBarMatrixMonitor 'about'

## [0.1.0] 2024-10-23
### Added
- Initially added MenuBarMatrix toolbar audio rooting tool, incl. TCP/IP remote monitoring interface
- Initially added MenuBarMatrixMonitor Desktop-, Mobile- and Embedded-App to connect to MenuBarMatrix instances to provide level metering

### Changed

### Fixed
