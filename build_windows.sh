# Temporary bash script to build for windows until I figure out how to work cross compiling with cmake

rm bin/OpenSigmar.exe && rm bin/SDL3.dll

x86_64-w64-mingw32-gcc -Ivendored/SDL/include -Iinclude -Llib -o bin/OpenSigmar.exe src/sdl/main.c src/sdl/Platform_SDL.c src/Sigmar.c -lSigmar -lSDL3 -mwindows --define-macro=SIGMAR_SDL

cp lib/SDL3.dll bin
