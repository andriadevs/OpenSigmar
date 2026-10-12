#ifndef OPENSIGMAR_PLATFORM_SDL_H
#define OPENSIGMAR_PLATFORM_SDL_H

/*
 * Platform-specific entry point for platforms that use SDL (Linux, etc.)
 */

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

#define ASSETS_DIR "./assets/"
#define TEXTURE_TILE "tile.png"
#define TEXTURE_HOVER "hover.png"
#define TEXTURE_MARBLE "marble.png"

typedef struct Sigmar Sigmar;

typedef struct {
    SDL_Texture *tile;
    SDL_Texture *hover;
    SDL_Texture *marble;
} Textures;

// Stores the rendered coordinates and represented marble index of a given tile on the board.
typedef struct {
    float x, y; // Coordinates where this tile is rendered.
    unsigned int index; // Index of the marble represented by this Tile.
} Tile;

typedef struct {
    Sigmar *sigmar;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int render_width;
    int render_height;
    float mouse_x;
    float mouse_y;
    unsigned int mouse_tile;
    Textures texture;
    float tile_width;
    float tile_height;
    float tile_width_offset;
    float tile_height_offset;
    Tile *tiles;
    int len_tiles;
} AppState;

int sigmar_LoadTextures();

int sigmar_SetupTiles();

int sigmar_RenderTile(Tile *tile);

SDL_AppResult sigmar_SDL_AppInit(void **appstate, int argc, char *argv[]);

SDL_AppResult sigmar_SDL_AppEvent(void *appstate, SDL_Event *event);

SDL_AppResult sigmar_SDL_AppIterate(void *appstate);

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result);

#endif //OPENSIGMAR_PLATFORM_SDL_H