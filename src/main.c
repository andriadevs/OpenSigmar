/*
 * Switches between platform-specific entry points, although only SDL is used ATM.
 */

#ifdef SIGMAR_SDL
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Platform_SDL.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) { return sigmar_SDL_AppInit(appstate, argc, argv); }
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) { return sigmar_SDL_AppEvent(appstate, event); }
SDL_AppResult SDL_AppIterate(void *appstate) { return sigmar_SDL_AppIterate(appstate); }
void SDL_AppQuit(void *appstate, SDL_AppResult result) { return sigmar_SDL_AppQuit(appstate, result); }

#else

// No other platforms implemented.
int (main) {
    return 0;
}


#endif