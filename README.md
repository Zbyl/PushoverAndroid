# Pushover

This is an open source re-implementation of the Pushover game:
https://www.mobygames.com/game/pushover

It is a fork of version 0.0.5 of http://pushover.sourceforge.net/ project, by Andreas R�ver et al.
It is distributed under GPL version 3, as mandated by the license of the original project.

The primary goal of this fork is to provide Android support.

# Compilation

This project uses Android Studio 3 and requires the following libraries:
- SDL2-2.0.7
- SDL2_mixer-2.0.2
  Note: Set SUPPORT_MP3_MPG123 ?= false in Android.mk for x86 build to work.
  This should be fixed in newer versions of SDL2_mixer.
- SDL2_ttf-2.0.14
- zlib1211
- lpng1634
- lua-5.3.4

# Todo

- Fix application exit.
- Fix level fade in animation.
- Fix sound enable bug?
- Fix sound playing bug? (Falling  blocks for example.)
- Domino fallen on blocker is draw on top after a while.
- Dominos flash behind/on top of ladders.
- Loading level sets in levelset.cpp
- Find all stat references.
- Remove file IO from pngloader. Use SDL.
- Remove AROS support.
- Remove config.h. Introduce constant for version.
- Fix t display (long cast) in userData_c::userData_c.
- Add click feedback on windows.

- Re-enable localization
- Cleanup fullscreen handling.
- Re-enable windowed mode on desktops.
- Prepare CMake projects for Windows and Linux.
- Remove dirty rects tracking.
- Text input handling (backspace) is not utf-clean.
- Input window won't work well with big keyboards.

- Refactor a lot...

# SDL on Android

The project was created by following tutorial "Setting up SDL 2 on Windows Android Studio 3.0.1" by Lazy Foo' Productions:
http://lazyfoo.net/tutorials/SDL/52_hello_mobile/android_windows/index.php
