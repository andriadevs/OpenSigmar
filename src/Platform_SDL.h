#ifndef OPENSIGMAR_PLATFORM_SDL_H
#define OPENSIGMAR_PLATFORM_SDL_H

/*
 * Platform-specific entry point for platforms that use SDL (Linux, etc.)
 */

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

typedef struct Sigmar Sigmar; // Forward declaration.

#define ASSETS_DIR "./assets/"
#define TEXTURE_BACKGROUND "workshop.png"
#define TEXTURE_TILE "tile_indent.png"
#define TEXTURE_EDGE_NW "edge_nw.png"
#define TEXTURE_EDGE_N "edge_n.png"
#define TEXTURE_EDGE_NE "edge_ne.png"
#define TEXTURE_EDGE_SE "edge_se.png"
#define TEXTURE_EDGE_S "edge_s.png"
#define TEXTURE_EDGE_SW "edge_sw.png"
#define TEXTURE_HOVER "hover.png"
#define TEXTURE_MARBLE "marble.png"
#define TEXTURE_SALT "salt.png"
#define TEXTURE_FIRE "fire.png"
#define TEXTURE_WATER "water.png"
#define TEXTURE_EARTH "earth.png"
#define TEXTURE_AIR "air.png"
#define TEXTURE_VITAE "vitae.png"
#define TEXTURE_MORS "mors.png"
#define TEXTURE_QUICKSILVER "quicksilver.png"
#define TEXTURE_LEAD "lead.png"
#define TEXTURE_TIN "tin.png"
#define TEXTURE_IRON "iron.png"
#define TEXTURE_COPPER "copper.png"
#define TEXTURE_SILVER "silver.png"
#define TEXTURE_GOLD "gold.png"

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

/*
 * Stores SDL_Texture pointers for each texture. Only exists so that the AppState structure doesn't get flooded with a dozen SDL_Texture pointers.
 */
typedef struct {
    SDL_Texture *background;
    SDL_Texture *tile;
    SDL_Texture *edge_nw;
    SDL_Texture *edge_n;
    SDL_Texture *edge_ne;
    SDL_Texture *edge_se;
    SDL_Texture *edge_s;
    SDL_Texture *edge_sw;
    SDL_Texture *hover;
    SDL_Texture *marble;
    SDL_Texture *salt;
    SDL_Texture *fire;
    SDL_Texture *water;
    SDL_Texture *earth;
    SDL_Texture *air;
    SDL_Texture *vitae;
    SDL_Texture *mors;
    SDL_Texture *quicksilver;
    SDL_Texture *lead;
    SDL_Texture *tin;
    SDL_Texture *iron;
    SDL_Texture *copper;
    SDL_Texture *silver;
    SDL_Texture *gold;
} Textures;

/*
 * Stores the render coordinates and the index of the represented marble for each tile on the board.
 */
typedef struct {
    float x, y; // Render coordinates for the tile.
    unsigned int index; // Index of the marble represented by the tile.
} Tile;

/*
 * Stores the render coordinates and texture of an "edge tile" of the board.
 */
typedef struct {
    float x, y;
    SDL_Texture *texture;
} Edge;

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
    unsigned short tiles_length;

    Edge *edges;
    unsigned short edges_length;
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