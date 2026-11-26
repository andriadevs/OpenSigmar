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

#include <SDL3/SDL_keycode.h>
#include <time.h>

#define NUM_AUDIO_STREAMS 3 // Number of audio streams to use. First stream is always dedicated solely to background music.
#define BASE_GAIN 0.2f // Base value used for gain, volume minimums and maximums are based on this value.

#define FPS 60 // Target FPS. App iterations are delayed so as to meet this value.
#define ANIMATION_FPS 30 // Frames-per-second for animations

#define ASSETS_DIR "./assets/"
#define TEXTURE_DIR ASSETS_DIR "texture/"
#define SOUND_DIR ASSETS_DIR "sound/"

#define SAVE_FILE_HEADER "#!"
#define SAVE_FILE "./save"
#define DATA_FILE_HEADER "#!"
#define DATA_FILE ASSETS_DIR "solitaire.dat"

#define CONFIG_FILE "./config.ini"
#define CONFIG_MUSIC_VOLUME "music volume"
#define CONFIG_MUSIC_VOLUME_DEFAULT "0.8"
#define CONFIG_SOUND_VOLUME "sound volume"
#define CONFIG_SOUND_VOLUME_DEFAULT "0.8"
#define CONFIG_RESET_KEY "reset key"
#define CONFIG_RESET_KEY_DEFAULT "32"
#define CONFIG_DEBUG_KEY "debug mode toggle key"
#define CONFIG_DEBUG_KEY_DEFAULT "100"

#define NUM_TEXTURES (TEXTURE_INVALID)
#define NUM_SOUNDS (SOUND_INVALID)

typedef struct Sigmar Sigmar; // Forward declaration.
typedef struct SigmarData SigmarData; // Forward declaration.

/*
 * Stores an RGBA32 color.
 */
typedef struct ColorRGBA {
    Uint8 r, g, b, a;
} ColorRGBA;

/*
 * Return codes for various functions.
 */
typedef enum : char {
    SIGMAR_ERROR,
    SIGMAR_OK,
    SIGMAR_MISSING_TEXTURES, // Returned when some, but not all, texture files failed to load.
    SIGMAR_NO_TEXTURES, // Returned when all texture files failed to load.
    SIGMAR_MISSING_SOUNDS, // Returned when some, but not all, sounds files failed to load.
    SIGMAR_NO_SOUNDS, // Returned when all sound files failed to load.
    SIGMAR_INVALID
} SigmarResultSDL;

/*
 * Identifiers for valid textures that can be retrieved from the texture array.
 */
typedef enum : char {
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
    TEXTURE_HIGHLIGHT,
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

/*
 * Identifiers for valid sounds that can be triggered with sigmar_PlaySound()
 */
typedef enum : char {
    SOUND_SELECT,
    SOUND_MATCH,
    SOUND_START,
    SOUND_END,
    SOUND_MUSIC,
    SOUND_INVALID
} SoundID;

/*
 * Identifiers for valid animations that can be triggered with sigmar_PlayAnimation()
 */
typedef enum : char {
    ANIMATION_BOARD_CLEAR,
    ANIMATION_BOARD_GENERATE,
    ANIMATION_INVALID
} AnimationID;

/*
 * Identifiers for each unique button type.
 */
typedef enum {
    BUTTON_RESET,
    BUTTON_HIGHLIGHT,
    BUTTON_INVALID
} ButtonType;

/*
 * Stores info specific to a given button.
 */
typedef union {
    MarbleType marble_type; // Used by highlight buttons to determine which marble type to highlight.
} ButtonInfo;

/*
 * Stores the render coordinates and the index of the represented marble for each tile on the board.
 */
typedef struct {
    float x, y; // Render coordinates for the tile.
    int x_offset, y_offset; // Coordinate offsets used when animating tiles.
    float marble_alpha_mult; // Multiplier applied to the alpha when rendering marbles, used in animations.
    char highlight; // Whether the tile should be highlighted.
    unsigned int index; // Index of the marble represented by the tile.
} Tile;

/*
 * Stores the render coordinates and texture of an uninteractible sprite.
 */
typedef struct {
    float x, y;
    TextureID texture;
} Sprite;

typedef struct {
    SDL_FRect bound;
    char hovered; // Whether or not the mouse is currently hovering this Button
    char pressed; // Whether or not the mouse is currently pressing this Button
    ButtonType type;
    ButtonInfo info;
    TextureID texture;
} Button;

typedef struct SigmarData {
    unsigned long wins;
} SigmarData;

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
    AnimationID current_animation;
    double animation_position;

    double music_volume;
    double sound_volume;

    SDL_Keycode reset_key;
    SDL_Keycode debug_key;
    
    unsigned int render_width;
    unsigned int render_height;
    unsigned int hud_height;

    SDL_FPoint mouse_position;
    unsigned char mouse_tile; // Index of the tile the mouse hovered over last.
    unsigned char mouse_button; // Index of the button the mouse hovered over last.
    
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

    unsigned int last_board;
    char board_clear;
    char debug_mode;

    clock_t last_tick;
} AppState;

extern const float MILLISECONDS_PER_FRAME;

extern const char animation_length[ANIMATION_INVALID];

extern const char texture_filename[TEXTURE_INVALID][24];

extern const char sound_filename[SOUND_INVALID][36];

extern const int type_symbol[]; // Maps a valid MarbleType to its respective symbol texture.

extern const ColorRGBA type_color[]; // Maps a valid MarbleType to its respective ColorRGBA.

/*
 * Loads settings from the config file into SigmarData
 */
SigmarResultSDL sigmar_LoadConfigs();

/*
 * Loads textures from assets directory and sets the tile_width and tile_height from the dimensions of the tile texture.
 */
SigmarResultSDL sigmar_LoadTextures();

/*
 * Checks for sound files in the assets directory and attempts to set the audio_spec.
 */
SigmarResultSDL sigmar_CheckSounds();

/*
 * Builds the tile array, calculates the render coordinates of all the tiles on the board for later.
 *
 * Also calculates and arranges sprites and any other visual elements that are positioned in relation to the board.
 */
SigmarResultSDL sigmar_SetupBoard();

/*
 * Renders a single tile on the board, as well as the marble at that location if there is one.
 */
SigmarResultSDL sigmar_RenderTile(Tile *tile);

/*
 * Renders an integer at the given coordinates.
 */
SigmarResultSDL sigmar_RenderNumber(float x, float y, ColorRGBA *color, int number);

/*
 * Renders the background texture.
 */
SigmarResultSDL sigmar_RenderBackground();

/*
 * Renders the HUD to the screen.
 */
SigmarResultSDL sigmar_RenderHud();

/*
 * Plays the sound with the given SoundID.
 * 
 * If flush == 1, flushes the buffer of the specified audio stream.
 */
SigmarResultSDL sigmar_PlaySound(SoundID sound, int stream, int flush);

/*
 * Resets animated values to their defaults and sets the current animation to the given AnimationID.
 *
 * Using sigmar_PlayAnimation(ANIMATION_INVALID) cancels any currently playing animation.
 */
SigmarResultSDL sigmar_PlayAnimation(AnimationID animation);

/*
 * Processes the next animation frame and increments the animation frame counter.
 */
SigmarResultSDL sigmar_AnimationFrame(double delta);

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

/*
 * Called when the mouse hovers over a button.
 */
void sigmar_OnButtonHover(ButtonType type, Button *button);

/*
 * Called when the mouse is no longer hovering over a button.
 */
void sigmar_OnButtonLeave(ButtonType type, Button *button);

/*
 * Called when the mouse clicks on a button.
 */
void sigmar_OnButtonPress(ButtonType type, Button *button);

/*
 * Called when the mouse is released after clicking on a button.
 */
void sigmar_OnButtonRelease(ButtonType type, Button *button);

/*
 * Called when the board is reset.
 */
void sigmar_OnBoardReset();

/*
 * Called when the board is cleared.
 */
void sigmar_OnBoardClear();

#endif //OPENSIGMAR_PLATFORM_SDL_H