#ifndef OPENSIGMAR_PLATFORM_SDL_H
#define OPENSIGMAR_PLATFORM_SDL_H

/*
 * Platform-specific entry point for platforms that use SDL (Linux, etc.)
 */

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

typedef struct Sigmar Sigmar; // Forward declaration.

#define ASSETS_DIR "./assets/"
#define TEXTURE_TILE "tile.png"
#define TEXTURE_HOVER "hover.png"
#define TEXTURE_MARBLE "marble.png"

/*
 * Stores SDL_Texture pointers for each texture. Only exists so that the AppState structure doesn't get flooded with a dozen SDL_Texture pointers.
 */
typedef struct {
    SDL_Texture *tile;
    SDL_Texture *hover;
    SDL_Texture *marble;
} Textures;


/*
 * Stores the render coordinates and the index of the represented marble for each tile on the board.
 */
typedef struct {
    float x, y; // Render coordinates for the tile.
    unsigned int index; // Index of the marble represented by the tile.
} Tile;

/*
 * Stores all of the persistent data used by the game & SDL.
 */
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


/*
 * Loads textures from assets directory and sets the tile_width and tile_height from the dimensions of the tile texture.
 */
int sigmar_LoadTextures();

/*
 * Builds the tile array, calculates the render coordinates of each tile on the board and stores it for later.
 */
int sigmar_SetupTiles();

/*
 * Renders a single tile on the board, as well as the marble at that location if there is one.
 */
int sigmar_RenderTile(Tile *tile);

/*
 * Called by main on SDL_AppInit() when SIGMAR_SDL is defined.
 */
SDL_AppResult sigmar_SDL_AppInit(void **appstate, int argc, char *argv[]);

/*
 * Called by main on SDL_AppEvent() when SIGMAR_SDL is defined.
 */
SDL_AppResult sigmar_SDL_AppEvent(void *appstate, SDL_Event *event);

/*
 * Called by main on SDL_AppIterate() when SIGMAR_SDL is defined.
 */
SDL_AppResult sigmar_SDL_AppIterate(void *appstate);

/*
 * Called by main on SDL_AppQuit() when SIGMAR_SDL is defined.
 */
void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result);

#endif //OPENSIGMAR_PLATFORM_SDL_H