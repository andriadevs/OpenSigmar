# OpenSigmar
### An Open Source, Multiplatform Game Inspired By Sigmar's Garden

## BUILDING:

All instructions assume binaries are being built from an x86_64 Linux system.

### For Linux:
Use `cmake --preset linux-x86_64` to setup the build directory and `cmake --build build/linux-x86_64` to build.

### For Windows:
Install mingw using your distribution's package manager and download the SDL3 latest Windows x86_64 release from the SDL3 GitHub repository and put 'libSDL3.dll.a' and 'SDL3.dll' in the lib directory.
Then, use `cmake --preset windows-x86_64` to setup the build directory and `cmake --build build/windows-x86_64` to build.

### For TI-84 Plus CE:
TI-84 Plus CE version is currently not yet implemented!

___
### PLEASE NOTE:
Sounds from Opus Magnum are not included for legal reasons, but if you own the game you can copy over "solitaire_select.wav", "solitaire_match.wav", "solitaire_start.wav", and "solitaire_end.wav" from the Content folder of your installation into the assets/sound directory for sounds. For music, get the file named "Solitaire.ogg" and convert it using software such as audacity into a .wav file, and rename it to "solitaire_music.wav", and put this file into the assets/sound directory as well.