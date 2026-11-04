#ifdef SIGMAR_SDL
#ifndef OPENSIGMAR_PLATFORM_SDL_H
#define OPENSIGMAR_PLATFORM_SDL_H



/*
 * Platform-specific entry point for platforms that use SDL (Linux, etc.)
 */

#include "SDL3/SDL_rect.h"
#include "Sigmar.h"

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

#include <time.h>

#define NUM_AUDIO_STREAMS 3 // Number of audio streams to use. First stream is always dedicated solely to background music.
#define BASE_GAIN 0.2f // Base value used for gain, volume minimums and maximums are based on this value.

#define ANIMATION_FPS 20 // Frames-per-second for animations

typedef struct Sigmar Sigmar; // Forward declaration.
typedef struct SigmarData SigmarData; // Forward declaration.

typedef struct ColorRGBA {
    Uint8 r, g, b, a;
} ColorRGBA;

#define ASSETS_DIR "./assets/"
#define TEXTURE_DIR ASSETS_DIR "texture/"
#define SOUND_DIR ASSETS_DIR "sound/"

typedef enum {
    TEXTURE_TILE,
    TEXTURE_NUMBER,
    TEXTURE_BACKGROUND,
    TEXTURE_EDGE_NW,
    TEXTURE_EDGE_N,
    TEXTURE_EDGE_NE,
    TEXTURE_EDGE_SE,
    TEXTURE_EDGE_S,
    TEXTURE_EDGE_SW,
    TEXTURE_CORNER_NW,
    TEXTURE_CORNER_W,
    TEXTURE_CORNER_SW,
    TEXTURE_CORNER_SE,
    TEXTURE_CORNER_E,
    TEXTURE_CORNER_NE,
    TEXTURE_WINS,
    TEXTURE_RESET,
    TEXTURE_HOVER,
    TEXTURE_MARBLE,
    TEXTURE_MARBLE_OUTLINE,
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

typedef enum {
    SOUND_SELECT,
    SOUND_MATCH,
    SOUND_START,
    SOUND_END,
    SOUND_MUSIC,
    SOUND_INVALID
} SoundID;

typedef enum {
    ANIMATION_BOARD_CLEAR,
    ANIMATION_BOARD_GENERATE,
    ANIMATION_INVALID
} AnimationID;

typedef enum {
    BUTTON_RESET,
    BUTTON_INVALID
} ButtonAction;

static char animation_length[ANIMATION_INVALID] = {
    [ANIMATION_BOARD_CLEAR] = 13,
    [ANIMATION_BOARD_GENERATE] = 13
};

static char texture_filename[TEXTURE_INVALID][24] = {
    [TEXTURE_TILE] = "tile_indent.png",
    [TEXTURE_NUMBER] = "number.png",
    [TEXTURE_BACKGROUND] = "background.png",
    [TEXTURE_EDGE_NW] = "border_nw.png",
    [TEXTURE_EDGE_N] = "border_n.png",
    [TEXTURE_EDGE_NE] = "border_ne.png",
    [TEXTURE_EDGE_SE] = "border_se.png",
    [TEXTURE_EDGE_S] = "border_s.png",
    [TEXTURE_EDGE_SW] = "border_sw.png",
    [TEXTURE_CORNER_NW] = "corner_nw.png",
    [TEXTURE_CORNER_W] = "corner_w.png",
    [TEXTURE_CORNER_SW] = "corner_sw.png",
    [TEXTURE_CORNER_SE] = "corner_se.png",
    [TEXTURE_CORNER_E] = "corner_e.png",
    [TEXTURE_CORNER_NE] = "corner_ne.png",
    [TEXTURE_WINS] = "wins.png",
    [TEXTURE_RESET] = "reset.png",
    [TEXTURE_HOVER] = "hover.png",
    [TEXTURE_MARBLE] = "marble.png",
    [TEXTURE_MARBLE_OUTLINE] = "marble_outline.png",
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

static char sound_filename[SOUND_INVALID][32] = {
    [SOUND_SELECT] = "solitaire_select.wav",
    [SOUND_MATCH] = "solitaire_match.wav",
    [SOUND_START] = "solitaire_start_short.wav",
    [SOUND_END] = "solitaire_end.wav",
    [SOUND_MUSIC] = "solitaire_music.wav"
};

// Maps a valid MarbleType to its respective symbol texture.
static int type_symbol[] = {
    [MARBLE_EMPTY] = TEXTURE_SALT,
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

// Maps a valid MarbleType to its respective ColorRGBA.
static ColorRGBA type_color[] = {
    [MARBLE_SALT] = {190, 190, 170, 255},
    [MARBLE_FIRE] = {160, 40, 10, 255},
    [MARBLE_WATER] = {40, 110, 110, 255},
    [MARBLE_EARTH] = {70, 130, 20, 255},
    [MARBLE_AIR] = {130, 180, 250, 255},
    [MARBLE_VITAE] = {200, 175, 150, 255},
    [MARBLE_MORS] = {60, 60, 50, 255},
    [MARBLE_QUICKSILVER] = {190, 190, 180, 255},
    [MARBLE_LEAD] = {120, 140, 140, 255},
    [MARBLE_TIN] = {180, 180, 150, 255},
    [MARBLE_IRON] = {110, 90, 90, 255},
    [MARBLE_COPPER] = {150, 100, 80, 255},
    [MARBLE_SILVER] = {90, 90, 80, 255},
    [MARBLE_GOLD] = {160, 100, 45, 255},
    [MARBLE_EMPTY] = {0, 0, 0, 0},
};

/*
 * Stores the render coordinates and the index of the represented marble for each tile on the board.
 */
typedef struct {
    float x, y; // Render coordinates for the tile.
    int x_offset, y_offset; // Coordinate offsets used when animating tiles.
    float marble_alpha_mult; // Multiplier applied to the alpha when rendering marbles, used in animations.
    unsigned int index; // Index of the marble represented by the tile.
} Tile;

/*
 * Stores the render coordinates and texture of an uninteractible sprite.
 */
typedef struct {
    float x, y;
    SDL_Texture *texture;
} Sprite;

typedef struct {
    SDL_FRect rect;
    SDL_Texture *texture;
    ButtonAction action;
    float alpha_mult;
} Button;

/*
 * Stores all of the persistent data used by the game & SDL.
 */
typedef struct {
    Sigmar *sigmar;
    SigmarData *data;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioSpec *audio_spec;
    SDL_AudioStream *audio_stream[3]; // [0]: music, [1]: short, frequent sound effects, [2]: long, infrequent sound effects
    SDL_Texture *texture[TEXTURE_INVALID + 1];
    Uint8 *sound[SOUND_INVALID];
    Uint32 sound_length[SOUND_INVALID];
    AnimationID current_animation;
    double animation_position;
    
    int render_width;
    int render_height;
    int hud_height;
    SDL_FPoint mouse_position;
    unsigned int mouse_tile;
    
    float tile_width;
    float tile_height;
    float tile_width_offset;
    float tile_height_offset;

    Sprite *sprites;
    unsigned short sprites_length;

    Tile *tiles;
    unsigned short tiles_length;

    Button *buttons;
    unsigned short buttons_length;

    int is_board_clear;

    clock_t last_tick;
} AppState;

/*
 * Loads textures from assets directory and sets the tile_width and tile_height from the dimensions of the tile texture.
 */
int sigmar_LoadTextures();

/*
 * Loads sounds from assets directory.
 */
int sigmar_LoadSounds();

/*
 * Builds the tile array, calculates the render coordinates of all the tiles on the board for later.
 *
 * Also calculates and arranges sprites and any other visual elements that are positioned in relation to the board.
 */
int sigmar_SetupBoard();

/*
 * Renders a single tile on the board, as well as the marble at that location if there is one.
 */
int sigmar_RenderTile(Tile *tile);

/*
 * Renders an integer at the given coordinates.
 */
int sigmar_RenderNumber(float x, float y, ColorRGBA *color, int number);

/*
 * Renders the background texture.
 */
int sigmar_RenderBackground();

/*
 * Renders the HUD to the screen.
 */
int sigmar_RenderHud();

/*
 * Clears the buffer for the given audio stream & plays the sound with the given SoundID.
 */
int sigmar_PlaySound(SoundID sound, int stream);

/*
 * Continues playing music with the given SoundID, puts data in the buffer if and only if the remaining data in the buffer is less than the amount of data to be added.
 */
int sigmar_PlayMusic(SoundID sound);

/*
 * Resets animated values to their defaults and sets the current animation to the given AnimationID.
 *
 * Using sigmar_PlayAnimation(ANIMATION_INVALID) cancels any currently playing animation.
 */
int sigmar_PlayAnimation(AnimationID animation);

/*
 * Processes the next animation frame and increments the animation frame counter.
 */
int sigmar_AnimationFrame(double delta);

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

void sigmar_OnButtonPress(ButtonAction action);

/*
 * Called when the board is reset.
 */
void sigmar_OnBoardReset();

/*
 * Called when the board is cleared.
 */
void sigmar_OnBoardClear();

#endif //OPENSIGMAR_PLATFORM_SDL_H
#endif