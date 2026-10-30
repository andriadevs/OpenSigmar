#ifndef OPENSIGMAR_PLATFORM_SDL_H
#define OPENSIGMAR_PLATFORM_SDL_H

/*
 * Platform-specific entry point for platforms that use SDL (Linux, etc.)
 */

#include "SDL3/SDL_render.h"
#include "Sigmar.h"
#include <SDL3/SDL.h>

typedef struct Sigmar Sigmar; // Forward declaration.

#define ASSETS_DIR "./assets/"
#define TEXTURE_DIR ASSETS_DIR "texture/"
#define SOUND_DIR ASSETS_DIR "sound/"

#define COLOR_SALT 190, 190, 170
#define COLOR_FIRE 160, 40, 10
#define COLOR_WATER 40, 110, 110
#define COLOR_EARTH 70, 130, 20
#define COLOR_AIR 130, 180, 250
#define COLOR_VITAE 200, 175, 150
#define COLOR_MORS 60, 60, 50
#define COLOR_QUICKSILVER 190, 190, 180
#define COLOR_LEAD 120, 140, 140
#define COLOR_TIN 180, 180, 150
#define COLOR_IRON 110, 90, 90
#define COLOR_COPPER 150, 100, 80
#define COLOR_SILVER 90, 90, 80
#define COLOR_GOLD 160, 100, 45

typedef enum {
    TEXTURE_BACKGROUND,
    TEXTURE_TILE,
    TEXTURE_EDGE_NW,
    TEXTURE_EDGE_N,
    TEXTURE_EDGE_NE,
    TEXTURE_EDGE_SE,
    TEXTURE_EDGE_S,
    TEXTURE_EDGE_SW,
    TEXTURE_HOVER,
    TEXTURE_MARBLE,
    TEXTURE_SALT,
    TEXTURE_FIRE,
    TEXTURE_WATER,
    TEXTURE_EARTH,
    TEXTURE_AIR,
    TEXTURE_VITAE,
    TEXTURE_MORS,
    TEXTURE_QUICKSILVER,
    TEXTURE_LEAD,
    TEXTURE_TIN,
    TEXTURE_IRON,
    TEXTURE_COPPER,
    TEXTURE_SILVER,
    TEXTURE_GOLD,
    TEXTURE_INVALID
} TextureID;


static char filename_map[][TEXTURE_INVALID] = {
    [TEXTURE_BACKGROUND] = "workshop.png",
    [TEXTURE_TILE] = "tile_indent.png",
    [TEXTURE_EDGE_NW] = "edge_nw.png",
    [TEXTURE_EDGE_N] = "edge_n.png",
    [TEXTURE_EDGE_NE] = "edge_ne.png",
    [TEXTURE_EDGE_SE] = "edge_se.png",
    [TEXTURE_EDGE_S] = "edge_s.png",
    [TEXTURE_EDGE_SW] = "edge_sw.png",
    [TEXTURE_HOVER] = "hover.png",
    [TEXTURE_MARBLE] = "marble.png",
    [TEXTURE_SALT] = "salt.png",
    [TEXTURE_FIRE] = "fire.png",
    [TEXTURE_WATER] = "water.png",
    [TEXTURE_EARTH] = "earth.png",
    [TEXTURE_AIR] = "air.png",
    [TEXTURE_VITAE] = "vitae.png",
    [TEXTURE_MORS] = "mors.png",
    [TEXTURE_QUICKSILVER] = "quicksilver.png",
    [TEXTURE_LEAD] = "lead.png",
    [TEXTURE_TIN] = "tin.png",
    [TEXTURE_IRON] = "iron.png",
    [TEXTURE_COPPER] = "copper.png",
    [TEXTURE_SILVER] = "silver.png",
    [TEXTURE_GOLD] = "gold.png"
};

static int symbol_map[] = {
    [MARBLE_SALT] = TEXTURE_SALT,
    [MARBLE_FIRE] = TEXTURE_FIRE,
    [MARBLE_WATER] = TEXTURE_WATER,
    [MARBLE_EARTH] = TEXTURE_EARTH,
    [MARBLE_AIR] = TEXTURE_AIR,
    [MARBLE_VITAE] = TEXTURE_VITAE,
    [MARBLE_MORS] = TEXTURE_MORS,
    [MARBLE_QUICKSILVER] = TEXTURE_QUICKSILVER,
    [MARBLE_LEAD] = TEXTURE_LEAD,
    [MARBLE_TIN] = TEXTURE_TIN,
    [MARBLE_IRON] = TEXTURE_IRON,
    [MARBLE_COPPER] = TEXTURE_COPPER,
    [MARBLE_SILVER] = TEXTURE_SILVER,
    [MARBLE_GOLD] = TEXTURE_GOLD
};

/*
 * Stores the render coordinates and the index of the represented marble for each tile on the board.
 */
typedef struct {
    float x, y; // Render coordinates for the tile.
    unsigned int index; // Index of the marble represented by the tile.
} Tile;

/*
 * Stores the render coordinates and texture of an uninteractible sprite.
 */
typedef struct {
    float x, y;
    SDL_Texture *texture;
} Sprite;

/*
 * Stores all of the persistent data used by the game & SDL.
 */
typedef struct {
    Sigmar *sigmar;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture[TEXTURE_INVALID];
    
    int render_width;
    int render_height;
    float mouse_x;
    float mouse_y;
    unsigned int mouse_tile;
    
    float tile_width;
    float tile_height;
    float tile_width_offset;
    float tile_height_offset;

    Sprite *sprites;
    unsigned short sprites_length;

    Tile *tiles;
    unsigned short tiles_length;
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
 * Renders the HUD to the screen.
 */
int sigmar_RenderHud();

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