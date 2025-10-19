# DeadClock

An app to help you remember critical timings in valve's DeadLock game.

<img src="https://github.com/smokejohn/deadclock/blob/master/img/deadclock_ui.png" width="300">

## Features

### Alerts
Currently has alerts for the following in game events:
* Breakables and small camps spawning (2:00)
* Medium camps spawning (6:00)
* Sinners Sacrifice and large camps spawning (8:00)
* Mid-Boss and the Breakables above it spawning (10:00)
* Repeating alert for runes spawning (every 5:00)
* Repeating alert for urn spawning (first at 10:00, then every 5:00)

The lead time i.e. how many seconds before the event the alert occurs can be configured (5-30 seconds)

### Overlay Window
An overlay window that runs on top of DeadLock game window and shows a timer
right under the ingame DeadLock timer. This window can be repositioned and then locked
which makes it unresponsive to any input.

### Notifications
When enabled the overlay window shows notifications for alerts on a chosen position on the screen.

### Text-To-Speech
Uses the systems Text-To-Speech feature to read alert messages at specific times.
The volume of the TTS feature can be adjusted.

### Global Hotkey support
Configurable global hotkeys to start, stop and set the timer which mirrors the ingame clock
without having to tab out of the game.

### OCR (Optical Character Recognition)
Reads and updates the in-game timer to update it's own internal timer via OCR from your screen

### Crossplattform
Built with libraries that work on any OS this app should run on Windows, Linux and MacOS.
Currently only builds for Windows are published and Linux builds will follow. 
Mac builds will not be supplied since virtualizing MacOS is a PITA / not supported by Apple.

## Installing

Grab the .zip file for your operating system from the releases page and unzip it to a folder of
your choice. Then simply click the deadclock executable to start the app.

## Building

### Requirements / Dependencies

* CMake
* Qt6 6.9.3
    * Core
    * Multimedia
    * Gui
    * Qml
    * Quick
    * QuickControls2
    * TextToSpeech
* Tesseract OCR

### Building on Windows
Use MSYS2 to build the project with the MINGW-64 toolchain
* [MSYS 2](https://www.msys2.org/)

Once in the msys2 mingw64 shell install the needed dependencies:
```
# cmake
pacman -S mingw-w64-x86_64-cmake
# Qt6 Base, QML and components
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-declarative mingw-w64-x86_64-qt6-speech
# tesseract-ocr and language data
pacman -S mingw-w64-x86_64-tesseract-ocr mingw-w64-x84_64-tesseract-data-eng
```

Since there is a bug in the mingw-w64-x86_64-leptonica pkgconfig file that gets
installed as a dependency of tesseract-ocr we need to fix it by editing the .pc file
```
nano /mingw64/lib/pkgconfig/lept.pc
```

Edit the line that starts with `Requires.private:` and replace:
* `PNG` with `libpng`
* `JPEG` with `libjpeg`
* `TIFF` with `libtiff-4`


Then you can execute the msys2 build script (from the project root):
```
./scripts/build_msys2.sh
```

This builds a Release Build and places the finished and ready to share/deploy build in
`./msysbuild/deploy/`

### Building on Linux
After cloning the repository with git and installing the needed dependencies.
Navigate to the cloned repository root configure the build with:

```
cmake -S ./ -B build
```

Then build with:
```
cmake --build ./build
```
