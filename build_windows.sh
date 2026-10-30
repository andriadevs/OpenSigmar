# Temporary bash script to build for windows until I figure out how to work cross compiling with cmake

rm bin/OpenSigmar.exe && rm bin/SDL3.dll

x86_64-w64-mingw32-gcc -Ivendored/SDL/include -Llib -o bin/OpenSigmar.exe src/main.c src/Platform_SDL.c src/Sigmar.c -lSDL3 -mwindows --define-macro=SIGMAR_SDL

cp lib/SDL3.dll bin
