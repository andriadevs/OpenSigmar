#include "Platform_SDL.h"
#include "Platform.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "Sigmar.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const float MILLISECONDS_PER_FRAME = 1000.0f / FPS;

const char animation_length[ANIMATION_INVALID] = {
    [ANIMATION_BOARD_CLEAR] = 13,
    [ANIMATION_BOARD_GENERATE] = 13
};

const char texture_filename[TEXTURE_INVALID][24] = {
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
    [TEXTURE_HIGHLIGHT] = "highlight.png",
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

const char sound_filename[SOUND_INVALID][36] = {
    [SOUND_SELECT] = "solitaire_select.wav",
    [SOUND_MATCH] = "solitaire_match.wav",
    [SOUND_START] = "solitaire_start.wav",
    [SOUND_END] = "solitaire_end.wav",
    [SOUND_MUSIC] = "solitaire_music.wav"
};

const int type_symbol[] = {
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

const ColorRGBA type_color[] = {
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
    [MARBLE_EMPTY] = {0, 0, 0, 0}
};

static AppState app;

SigmarResultSDL sigmar_LoadConfigs() {
    int rc;
    size_t filesize;
    char *file = (char *) SDL_LoadFile(CONFIG_FILE, &filesize);
    if (!file) {
        // Create new config.ini file.
        app.music_volume = 0.8f;
        app.sound_volume = 0.8f;
        app.reset_key = atoi(CONFIG_RESET_KEY_DEFAULT);
        app.debug_key = atoi(CONFIG_DEBUG_KEY_DEFAULT);

        const char default_ini[] = 
            CONFIG_MUSIC_VOLUME "=" CONFIG_MUSIC_VOLUME_DEFAULT "\n"
            CONFIG_SOUND_VOLUME "=" CONFIG_SOUND_VOLUME_DEFAULT "\n"
            CONFIG_RESET_KEY "=" CONFIG_RESET_KEY_DEFAULT "\n"
            CONFIG_DEBUG_KEY "=" CONFIG_DEBUG_KEY_DEFAULT;

        rc = SDL_SaveFile(CONFIG_FILE, default_ini, sizeof(default_ini) - 1);
        if (!rc) { SDL_Log("Couldn't generate default config.ini: %s", SDL_GetError()); return SIGMAR_ERROR; }

    } else {
        // Try to load configs from file.
        float m_vol, s_vol;
        SDL_Keycode r_key, d_key;

        const char format_string[] = 
            CONFIG_MUSIC_VOLUME "=%f\n"
            CONFIG_SOUND_VOLUME "=%f\n"
            CONFIG_RESET_KEY "=%u\n"
            CONFIG_DEBUG_KEY "=%u";

        rc = SDL_sscanf(file, format_string, &m_vol, &s_vol, &r_key, &d_key);

        SDL_free(file);

        if (rc != 4) { SDL_Log("Couldn't load config.ini"); }
        else {
            app.music_volume = m_vol;
            app.sound_volume = s_vol;
            app.reset_key = r_key;
            app.debug_key = d_key;
        }
    }

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_LoadTextures() {
    char num_fails = 0;
    char dir[128];
    SDL_Surface *surface = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    app.texture[TEXTURE_INVALID] = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_free(surface);

    for (int i = 0; i < TEXTURE_INVALID; i++) {
        strcpy(dir, SDL_GetBasePath());
        strcat(dir, TEXTURE_DIR);
        strcat(dir, texture_filename[i]);
        surface = SDL_LoadPNG(dir);
        if (surface) { 
            app.texture[i] = SDL_CreateTextureFromSurface(app.renderer, surface);
            SDL_SetTextureAlphaMod(app.texture[i], 255);
            SDL_free(surface);
            if (app.texture[i]) { continue; }
            else { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); }
        } else { SDL_Log("Couldn't load png: %s", SDL_GetError()); }

        // If texture fails to load, set texture to the error texture.
        app.texture[i] = app.texture[TEXTURE_INVALID];
        num_fails++;
    }

    app.tile_width = app.texture[TEXTURE_TILE]->w;
    app.tile_height = app.texture[TEXTURE_TILE]->h;

    app.hud_height = app.tile_height * 1.25f;

    if (num_fails == 0) { return SIGMAR_OK; }
    else { return num_fails == NUM_TEXTURES ? SIGMAR_NO_TEXTURES : SIGMAR_MISSING_TEXTURES; }
}

SigmarResultSDL sigmar_CheckSounds() {
    char num_fails = 0;
    char dir[128];
    SDL_AudioSpec spec;
    Uint8 *audio_buf;
    Uint32 audio_len;

    for (int i = 0; i < SOUND_INVALID; i++) {
        strcpy(dir, SDL_GetBasePath());
        strcat(dir, SOUND_DIR);
        strcat(dir, sound_filename[i]);
        SDL_LoadWAV(dir, &spec, &audio_buf, &audio_len);

        if (!audio_buf) { SDL_Log("Couldn't load .wav file: %s", SDL_GetError()); num_fails++; }
        else { *app.audio_spec = spec; }

        SDL_free(audio_buf);
    }

    if (num_fails == 0) { return SIGMAR_OK; }
    else { return num_fails == NUM_SOUNDS ? SIGMAR_NO_SOUNDS : SIGMAR_MISSING_SOUNDS; }
}

SigmarResultSDL sigmar_SetupBoard() {
    const int half_board = (int) (BOARD_SIZE / 2);

    app.tiles_length = BOARD_LENGTH;
    app.tiles = (Tile*) malloc(sizeof(Tile) * app.tiles_length);
    int next = 0;

    float hoffs = (app.render_width / 2.0f) - ((app.tile_width - app.tile_width_offset) * (BOARD_SIZE + 1) * 0.25f) - (app.tile_width_offset * 0.5f);
    float voffs = ((app.render_height - app.hud_height)  / 2.0f) - ((app.tile_height - app.tile_height_offset) * BOARD_SIZE * 0.5f) - (app.tile_height_offset * 0.5f);

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            app.tiles[next].index = next;
            app.tiles[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) x - (float) app.tile_width / 2 * (float) y);
            app.tiles[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) y);
            app.tiles[next].x_offset = 0;
            app.tiles[next].y_offset = 0;
            app.tiles[next].marble_alpha_mult = 1.0f;

            next++;
        }
    }

    // Setup sprites
    app.sprites_length = (half_board * 6) + 6;
    app.sprites = (Sprite*) malloc(sizeof(Sprite) * app.sprites_length);
    next = 0;

    // Border sprites
    for (int i = 0; i < 5; i++) {
        app.sprites[next].texture = TEXTURE_EDGE_NW;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.sprites[next].texture = TEXTURE_EDGE_N;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) -1);
        next++;

        app.sprites[next].texture = TEXTURE_EDGE_NE;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (half_board + i + 1) - (float) app.tile_width / 2 * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.sprites[next].texture = TEXTURE_EDGE_SE;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) BOARD_SIZE - (float) app.tile_width / 2 * (float) (half_board + i + 1));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;

        app.sprites[next].texture = TEXTURE_EDGE_S;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * BOARD_SIZE);
        next++;

        app.sprites[next].texture = TEXTURE_EDGE_SW;
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) i - (float) app.tile_width / 2 * (float) (half_board + i + 1));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;
    }

    // Border corner sprites
    app.sprites[next].texture = TEXTURE_CORNER_NE;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) half_board - (float) app.tile_width / 2 * -1.0f);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * -1.0f);
    next++;

    app.sprites[next].texture = TEXTURE_CORNER_W;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) half_board);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * half_board);
    next++;

    app.sprites[next].texture = TEXTURE_CORNER_SW;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) half_board - (float) app.tile_width / 2 * (float) BOARD_SIZE);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * BOARD_SIZE);
    next++;

    app.sprites[next].texture = TEXTURE_CORNER_SE;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) BOARD_SIZE - (float) app.tile_width / 2 * (float) BOARD_SIZE);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * BOARD_SIZE);
    next++;

    app.sprites[next].texture = TEXTURE_CORNER_E;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) BOARD_SIZE - (float) app.tile_width / 2 * (float) half_board);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * half_board);
    next++;

    app.sprites[next].texture = TEXTURE_CORNER_NW;
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) -1.0f);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * -1.0f);
    next++;

    // Setup buttons
    app.buttons_length = NUM_MARBLES + 1;
    app.buttons = (Button *) malloc(sizeof(Button) * app.buttons_length);
    next = 0;

    SDL_FRect rect = {
        (app.render_width / 2.0f) - (app.tile_width * NUM_MARBLES * 0.5f),
        (app.render_height) - app.tile_height,
        64,
        64
    };

    // Marble highlighter buttons
    for (int i = 0; i < MARBLE_INVALID; i++) {
        app.buttons[next].bound = rect;
        app.buttons[next].texture = TEXTURE_INVALID;
        app.buttons[next].type = BUTTON_HIGHLIGHT;
        app.buttons[next].info.marble_type = i;
        app.buttons[next].hovered = 0;
        app.buttons[next].pressed = 0;

        rect.x += app.tile_width;
        next++;
    }

    // Reset button
    app.buttons[next].bound.x = 16.0f;
    app.buttons[next].bound.y = app.render_height - ((app.hud_height / 2.0f) + (app.texture[TEXTURE_WINS]->h / 2.0f));
    app.buttons[next].bound.w = 64.0f;
    app.buttons[next].bound.h = 64.0f;
    app.buttons[next].texture = TEXTURE_RESET;
    app.buttons[next].type = BUTTON_RESET;
    app.buttons[next].hovered = 0;
    app.buttons[next].pressed = 0;
    next++;

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_RenderTile(Tile *tile) {
    SDL_FRect rect = {tile->x + tile->x_offset, tile->y + tile->y_offset, app.tile_width, app.tile_height};

    const MarbleType marble = app.sigmar->board[tile->index].type;
    const MarbleState state = app.sigmar->board[tile->index].state;

    SDL_Texture *symbol;
    ColorRGBA color;

    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_TILE], NULL, &rect);

    if (marble != MARBLE_EMPTY && marble != MARBLE_INVALID) {

        if (tile->highlight == 1 && state != MARBLE_SELECTED) {
            SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HIGHLIGHT], NULL, &rect);
        }

        symbol = app.texture[type_symbol[marble]];
        color = type_color[marble];

        if (state == MARBLE_SELECTED) {
            rect.y -= 2;
            SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HOVER], NULL, &rect);
        } else if (state == MARBLE_LOCKED) { color.a = 150; }

        SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(app.texture[TEXTURE_MARBLE], color.a * tile->marble_alpha_mult);
        SDL_SetTextureAlphaMod(symbol, color.a * tile->marble_alpha_mult);

        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_MARBLE], NULL, &rect);
        SDL_RenderTexture(app.renderer, symbol, NULL, &rect);

    }

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_RenderNumber(float x, float y, ColorRGBA *color, int number) {
    int n = number;
    int digits = 0;
    while (n >= 10) { digits++; n /= 10; }

    SDL_FRect srect = {16 * number, 0, 16, 32};
    SDL_FRect rect = {x, y, 16, 32};

    if (color != NULL) {
        SDL_SetTextureColorMod(app.texture[TEXTURE_NUMBER], color->r, color->g, color->b);
        SDL_SetTextureAlphaMod(app.texture[TEXTURE_NUMBER], color->a);
    } else {
        SDL_SetTextureColorMod(app.texture[TEXTURE_NUMBER], 255, 255, 255);
        SDL_SetTextureAlphaMod(app.texture[TEXTURE_NUMBER], 255);
    }

    for (digits = digits; digits >= 0; digits--) {
        srect.x = 16 * (number % 10);
        rect.x = x + 16 * digits;

        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_NUMBER], &srect, &rect);
        
        number /= 10;
    }

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_RenderBackground() {
    SDL_FRect rect = {0, 0, app.render_width, app.render_height};

    // Background image
    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_BACKGROUND], NULL, &rect);

    // HUD background
    rect.y = app.render_height - app.hud_height;
    rect.h = app.hud_height;

    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(app.renderer, &rect);

    SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(app.renderer, rect.x, rect.y, rect.x + app.render_width, rect.y);

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_RenderHud() {
    int i;
    const int hud_width = 90;
    const int hud_height = 610;

    SDL_FRect rect = {
        0.0f,
        (app.render_height) - (app.tile_height * 3.0f),
        app.render_width,
        app.tile_height * 2.0f
    };

    rect.x = (app.render_width / 2.0f) - (app.tile_width * (MARBLE_GOLD + 1) * 0.5f);
    rect.y = (app.render_height) - app.tile_height;
    rect.w = 64;
    rect.h = 64;

    SDL_Texture *symbol;
    ColorRGBA color;

    for (int i = 0; i < MARBLE_INVALID; i++) {

        symbol = app.texture[type_symbol[i]];
        color = type_color[i];

        if (app.buttons[i].hovered == 1) { SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HIGHLIGHT], NULL, &rect); }

        //SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE_OUTLINE], color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(app.texture[TEXTURE_MARBLE_OUTLINE], color.a * 0.5f);
        SDL_SetTextureAlphaMod(symbol, color.a * 0.5f);

        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_MARBLE_OUTLINE], NULL, &rect);
        SDL_RenderTexture(app.renderer, symbol, NULL, &rect);

        if (i <= MARBLE_AIR && app.sigmar->marble_counts[i] % 2 != 0) { SDL_SetRenderDrawColor(app.renderer, 200, 0, 0, 255); }
        else { SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, 255); }
        
        SDL_RenderDebugTextFormat(app.renderer, rect.x + 29, rect.y - 6, "%i", app.sigmar->marble_counts[i]);

        rect.x += app.tile_width;
    }

    // Wins
    rect.x = app.render_width - 16.0f - app.texture[TEXTURE_WINS]->w;
    rect.y = app.render_height - ((app.hud_height / 2.0f) + (app.texture[TEXTURE_WINS]->h / 2.0f));
    rect.w = app.texture[TEXTURE_WINS]->w;
    rect.h = app.texture[TEXTURE_WINS]->h;

    SDL_SetTextureAlphaMod(app.texture[TEXTURE_WINS], 200);
    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_WINS], NULL, &rect);

    SDL_RenderDebugTextFormat(app.renderer, rect.x + 58, rect.y + 42, "%10lu", app.data->wins);

    // Buttons
    for (int i = 0; i < app.buttons_length; i++) {
        SDL_SetTextureAlphaMod(app.texture[app.buttons[i].texture], 200);
        SDL_RenderTexture(app.renderer, app.texture[app.buttons[i].texture], NULL, &app.buttons[i].bound);
    }

    // Board clear message
    if (app.board_clear) {
        rect.x=app.render_width / 2.0f - 200;
        rect.y=app.render_height / 2.0f - 50;
        rect.w=400;
        rect.h=50;
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(app.renderer, &rect);

        SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderRect(app.renderer, &rect);

        SDL_RenderDebugText(app.renderer, rect.x + 25, rect.y + 25, "You cleared the board! Press space to reset.");
    }

    // Debug mode overlay
    if (app.debug_mode) {
        for (int i = 0; i < BOARD_LENGTH; i++) {
            if (app.sigmar->board[app.tiles[i].index].type != MARBLE_INVALID) {
                rect.x=app.tiles[i].x + (app.tile_width / 2) - 13;
                rect.y=app.tiles[i].y + (app.tile_height / 2) - 5;
                rect.w=26;
                rect.h=10;
                SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(app.renderer, &rect);
                SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDebugTextFormat(app.renderer, rect.x + 1, rect.y + 1, "%i", i);
            }
        }

        SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDebugTextFormat(app.renderer, 4, 4, "PATTERN: %i", app.sigmar->pattern);
    }

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_PlaySound(SoundID sound, int stream, int flush) {
    SDL_AudioSpec spec;
    Uint8 *audio_buf;
    Uint32 audio_len;

    char dir[256];
    strcpy(dir, SDL_GetBasePath());
    strcat(dir, SOUND_DIR);
    strcat(dir, sound_filename[sound]);

    SDL_LoadWAV(dir, &spec, &audio_buf, &audio_len);
    if (!audio_buf) { return SIGMAR_ERROR; }

    if (flush) { SDL_ClearAudioStream(app.audio_stream[stream]); }
    SDL_PutAudioStreamData(app.audio_stream[stream], audio_buf, audio_len);

    SDL_free(audio_buf);
    return SIGMAR_OK;
}

SigmarResultSDL sigmar_PlayAnimation(AnimationID animation) {
    int i;

    for (i = 0; i < app.tiles_length; i++) {
        app.tiles[i].x_offset = 0;
        app.tiles[i].y_offset = 0;
        app.tiles[i].marble_alpha_mult = 1.0f;
    }

    app.current_animation = animation;
    app.animation_position = 0.0f;

    return SIGMAR_OK;
}

SigmarResultSDL sigmar_AnimationFrame(double delta) {
    if (app.current_animation == ANIMATION_INVALID) { return 0; }

    int i, x, y;
    double dist;
    const int frame = (int) (app.animation_position * MILLISECONDS_PER_FRAME * ANIMATION_FPS);
    const int center_tile = (int) BOARD_LENGTH * 0.5;
    const int center_x = center_tile % BOARD_SIZE;
    const int center_y = center_tile / BOARD_SIZE;

    switch (app.current_animation) {
        case ANIMATION_BOARD_CLEAR:
            for (i = 0; i < app.tiles_length; i++) {
                x = i % BOARD_SIZE;
                y = i / BOARD_SIZE;
                dist = SDL_sqrt(SDL_pow(x - center_x, 2) + SDL_pow(y - center_y, 2));

                if (dist <= frame + 0.5f && dist >= frame - 0.5f) { app.tiles[i].y_offset = -2; }
                else { app.tiles[i].y_offset = 0; }
            }
            break;

        case ANIMATION_BOARD_GENERATE:
            for (i = 0; i < app.tiles_length; i++) {
                x = i % BOARD_SIZE;
                y = i / BOARD_SIZE;
                dist = SDL_sqrt(SDL_pow(x - center_x, 2) + SDL_pow(y - center_y, 2));

                if (dist <= frame + 0.5f) {
                    app.tiles[i].marble_alpha_mult = 1.0f;

                    if (dist >= frame - 0.5f) { app.tiles[i].y_offset = -2; }
                    else { app.tiles[i].y_offset = 0; }
                } else {
                    app.tiles[i].marble_alpha_mult = 0.0f;
                }
            }
            break;

        default:
            break;
    }

    if (frame >= animation_length[app.current_animation]) { sigmar_PlayAnimation(ANIMATION_INVALID); }
    else { app.animation_position += delta; }

    return SIGMAR_OK;
}

int sigmar_DataInit(SigmarData **data) {
    *data = malloc(sizeof(SigmarData));

    (*data)->wins = 0;

    return 0;
}

int sigmar_LoadData(SigmarData *data) {
    SDL_IOStream *file = SDL_IOFromFile(SAVE_FILE, "rb");
    if (!file) { return 1; }

    SDL_SeekIO(file, sizeof(SAVE_FILE_HEADER), SEEK_CUR);

    SDL_ReadIO(file, data, sizeof(SigmarData));
    
    SDL_CloseIO(file);

    return 0;
}

int sigmar_SaveData(SigmarData *data) {
    SDL_IOStream *file = SDL_IOFromFile(SAVE_FILE, "wb");
    if (!file) { return 1; }

    char header[] = SAVE_FILE_HEADER;
    SDL_WriteIO(file, &header, sizeof(header));

    SDL_WriteIO(file, data, sizeof(SigmarData));
    
    SDL_CloseIO(file);

    return 0;
}

int sigmar_PregenerateBoards(Sigmar *sigmar, unsigned int count) {
    char board_data[56];

    SDL_IOStream *file = SDL_IOFromFile(DATA_FILE, "wb");
    if (!file) { return 1; }

    char header[] = DATA_FILE_HEADER;
    SDL_WriteIO(file, &header, sizeof(header));

    SDL_WriteIO(file, &count, sizeof(unsigned int));

    unsigned int seed = time(NULL);

    for (int i = 0; i < count; i++) {
        sigmar_GenerateBoard(sigmar, seed++);
        sigmar_SerializeBoard(sigmar, board_data);
        SDL_WriteIO(file, board_data, 56);
    }

    SDL_CloseIO(file);

    return 0;
}

int sigmar_LoadBoard(Sigmar *sigmar) {
    char board_data[56];

    SDL_IOStream *file = SDL_IOFromFile(DATA_FILE, "rb");
    if (!file) { return 1; }

    SDL_SeekIO(file, sizeof(DATA_FILE_HEADER), SEEK_CUR);

    unsigned int count;
    SDL_ReadIO(file, &count, sizeof(unsigned int));
    
    if (app.last_board == UINT_MAX) { app.last_board = rand() % count; }
    else if (app.last_board > count - 1) { app.last_board = 0; }

    int offs = app.last_board * 56;
    app.last_board++;

    SDL_SeekIO(file, offs, SEEK_CUR);
    SDL_ReadIO(file, board_data, 56);

    sigmar_DeserializeBoard(sigmar, board_data);
    
    SDL_CloseIO(file);

    return 0;
}

SDL_AppResult sigmar_SDL_AppInit(void **appstate, int argc, char *argv[]) {
    int rc;

    app.window = NULL;
    app.renderer = NULL;
    app.music_volume = 0.0;
    app.sound_volume = 0.0;
    app.reset_key = SDLK_SPACE;
    app.debug_key = SDLK_D;
    app.render_width = 1280;
    app.render_height = 720;
    app.mouse_position.x = app.render_width / 2.0f;
    app.mouse_position.y = app.render_height / 2.0f;
    app.mouse_tile = -1;
    app.tile_width = 0;
    app.tile_height = 0;
    app.tile_width_offset = 0;
    app.tile_height_offset = 16;
    app.tiles = NULL;
    app.tiles_length = 0;
    app.current_animation = ANIMATION_INVALID;
    app.animation_position = 0;
    app.last_board = UINT_MAX;
    app.board_clear = 0;
    app.debug_mode = 0;

    rc = sigmar_LoadConfigs();
    if (rc != SIGMAR_OK) { SDL_Log("No config.ini could be loaded/created, using default configurations."); }

    rc = sigmar_DataInit(&(app.data));
    if (rc != 0) { return SDL_APP_FAILURE; }
    else { sigmar_LoadData(app.data); }

    rc = sigmar_Init(&(app.sigmar));
    if (rc != 0) { return SDL_APP_FAILURE; }

    // OPTIONS:
    //  -g [number] : Pregenerates 'number' boards & writes them to the data file.
    //  -d : Starts OpenSigmar in debug mode.
    if (argc == 3 && !memcmp("-g", argv[1], 3)) {
        printf("Generating new boards! ... ");
        fflush(NULL);
        int count = atoi(argv[2]);

        sigmar_PregenerateBoards(app.sigmar, count);

        printf("Done!\n");
        return SDL_APP_SUCCESS;
    } else if (argc == 2 && !memcmp("-d", argv[1], 3)) {
        printf("Starting OpenSigmar in debug mode...");
        app.debug_mode = 1;
    }

    sigmar_Reset(app.sigmar);
    sigmar_LoadBoard(app.sigmar);

    SDL_SetAppMetadata("OpenSigmar", "0.1", "com.andria.opensigmar");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("OpenSigmar", app.render_width, app.render_height, 0, &app.window, &app.renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Surface *icon = SDL_LoadPNG(TEXTURE_DIR "icon.png");
    if (!icon) { SDL_Log("Couldn't load png: %s, window icon will appear as default.", SDL_GetError()); }

    SDL_SetWindowIcon(app.window, icon);

    SDL_SetRenderLogicalPresentation(app.renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    app.audio_spec = malloc(sizeof(SDL_AudioSpec));

    // Check Sounds
    rc = sigmar_CheckSounds();
    if (rc == SIGMAR_OK || rc == SIGMAR_MISSING_SOUNDS) {
        // Initialize Audio Streams
        for (int i = 0; i < NUM_AUDIO_STREAMS; i++) {
            app.audio_stream[i] = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, app.audio_spec, NULL, NULL);
            if (!app.audio_stream[i]) {
                SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
                return SDL_APP_FAILURE;
            } else {
                SDL_SetAudioStreamGain(app.audio_stream[i], BASE_GAIN * app.sound_volume);
                SDL_ResumeAudioStreamDevice(app.audio_stream[i]);
            }
        }

        SDL_SetAudioStreamGain(app.audio_stream[0], BASE_GAIN * app.music_volume);

    } else if (rc == SIGMAR_NO_SOUNDS) { SDL_Log("No sound files found, audio streams will not be initialized."); }

    // Load Textures
    rc = sigmar_LoadTextures();
    if (rc == SIGMAR_NO_TEXTURES) { SDL_Log("No texture files found."); }

    // Setup Board
    rc = sigmar_SetupBoard();
    if (rc == SIGMAR_ERROR) { return SDL_APP_FAILURE; }

    // Bind Events
    sigmar_Bind(app.sigmar, EVENT_BOARD_RESET, sigmar_OnBoardReset);
    sigmar_Bind(app.sigmar, EVENT_BOARD_CLEAR, sigmar_OnBoardClear);

    return SDL_APP_CONTINUE;
}

SDL_AppResult sigmar_SDL_AppEvent(void *appstate, SDL_Event *event) {
    int i;
    SDL_FRect bounds;
    Marble *marble;

    switch (event->type) {
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            SDL_ConvertEventToRenderCoordinates(app.renderer, event);
            app.mouse_position.x = event->motion.x;
            app.mouse_position.y = event->motion.y;
            app.mouse_tile = BOARD_LENGTH;

            // Checks whether the mouse is hovering over a valid tile.
            bounds.w = app.tile_width;
            bounds.h = app.tile_height;
            for (i = 0; i < app.tiles_length; i++) {
                marble = &app.sigmar->board[app.tiles[i].index];
                bounds.x = app.tiles[i].x;
                bounds.y = app.tiles[i].y;

                if (SDL_PointInRectFloat(&app.mouse_position, &bounds) &&
                    marble->type != MARBLE_INVALID &&
                    marble->type != MARBLE_EMPTY) { app.mouse_tile = i; break; }
            }

            // Handle buttons
            for (int i = 0; i < app.buttons_length; i++) {
                if (SDL_PointInRectFloat(&app.mouse_position, &app.buttons[i].bound)) {
                    if (app.buttons[i].hovered == 0) {
                        app.buttons[i].hovered = 1;
                        sigmar_OnButtonHover(app.buttons[i].type, &app.buttons[i]);
                        app.mouse_button = i;
                    }

                    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && app.buttons[i].pressed == 0) {
                        app.buttons[i].pressed = 1;
                        sigmar_OnButtonPress(app.buttons[i].type, &app.buttons[i]);
                    }

                } else {
                    if (app.buttons[i].hovered == 1) {
                        app.buttons[i].hovered = 0;
                        sigmar_OnButtonLeave(app.buttons[i].type, &app.buttons[i]);
                    }

                    if (app.buttons[i].pressed == 1) {
                        app.buttons[i].pressed = 0;
                        sigmar_OnButtonRelease(app.buttons[i].type, &app.buttons[i]);
                    }
                }
            }

            // Handles mouse click event, attempts a select/match if applicable.
            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (app.mouse_tile != BOARD_LENGTH && app.sigmar->board[app.mouse_tile].state != MARBLE_LOCKED) {
                    SelectResult rc = sigmar_Select(app.sigmar, app.tiles[app.mouse_tile].index);

                    switch (rc) {
                        case SELECT_SUCCESS:
                            sigmar_PlaySound(SOUND_SELECT, 1, 1);
                            break;

                        case SELECT_DESELECT:
                            sigmar_PlaySound(SOUND_SELECT, 1, 1);
                            break;

                        case SELECT_MATCH:
                            sigmar_PlaySound(SOUND_MATCH, 1, 1);
                            break;

                        default:
                            break;
                    }
                }
            }

            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_MOUSE_BUTTON_UP:
            for (int i = 0; i < app.buttons_length; i++) {
                if (app.buttons[i].pressed == 1) {
                    app.buttons[i].pressed = 0;
                    sigmar_OnButtonRelease(app.buttons[i].type, &app.buttons[i]);
                }
            }

            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == app.reset_key) { sigmar_Reset(app.sigmar); }
            else if (event->key.key == app.debug_key) { app.debug_mode = !app.debug_mode; }

            return SDL_APP_CONTINUE;

        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;

        default:
            return SDL_APP_CONTINUE;
    }
}

SDL_AppResult sigmar_SDL_AppIterate(void *appstate) {
    SDL_FRect rect;

    clock_t tick = clock();
    double delta = ((double) tick - app.last_tick) / CLOCKS_PER_SEC;
    app.last_tick = tick;

    // Process animation
    sigmar_AnimationFrame(delta);

    // Clear render
    SDL_SetRenderDrawColor(app.renderer, 30, 30, 30, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app.renderer);

    // Render background
    sigmar_RenderBackground();

    // Render tiles
    SDL_SetRenderDrawColor(app.renderer, 255, 100, 0, SDL_ALPHA_OPAQUE);
    rect.w = app.tile_width;
    rect.h = app.tile_height;
    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (app.sigmar->board[app.tiles[i].index].type != MARBLE_INVALID) { sigmar_RenderTile(&app.tiles[i]); }
    }

    //Render sprites
    for (int i = 0; i < app.sprites_length; i++) {
        rect.x = app.sprites[i].x;
        rect.y = app.sprites[i].y;
        SDL_RenderTexture(app.renderer, app.texture[app.sprites[i].texture], NULL, &rect);
    }

    // Render hover
    if (app.mouse_tile != BOARD_LENGTH && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE && app.sigmar->board[app.mouse_tile].type != MARBLE_EMPTY) {
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        rect.x = (float) app.tiles[app.mouse_tile].x;
        rect.y = (float) app.tiles[app.mouse_tile].y;
        rect.w = app.tile_width;
        rect.h = app.tile_height;
        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HOVER], NULL, &rect);
    }
    
    sigmar_RenderHud();

    SDL_RenderPresent(app.renderer);

    if (SDL_GetAudioStreamQueued(app.audio_stream[0]) < 100) {
        sigmar_PlaySound(SOUND_MUSIC, 0, 0);
    }

    SDL_Delay(MILLISECONDS_PER_FRAME - delta);

    return SDL_APP_CONTINUE;
}

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result) {
    sigmar_SaveData(app.data);
}

void sigmar_OnButtonHover(ButtonType type, Button *button) {
    switch (type) {
        default:
            break;
    }
}

void sigmar_OnButtonLeave(ButtonType type, Button *button) {
    switch (type) {
        default:
            break;
    }
}

void sigmar_OnButtonPress(ButtonType type, Button *button) {
    switch (type) {
        case BUTTON_HIGHLIGHT:
            for (int i = 0; i < app.tiles_length; i++) {
                if (app.sigmar->board[app.tiles[i].index].type == button->info.marble_type) {
                    app.tiles[i].highlight = 1;
                }
            }
            break;

        case BUTTON_RESET:
            sigmar_Reset(app.sigmar);
            break;

        default:
            break;
    }
}

void sigmar_OnButtonRelease(ButtonType type, Button *button) {
    switch (type) {
        case BUTTON_HIGHLIGHT:
            for (int i = 0; i < app.tiles_length; i++) {
                if (app.sigmar->board[app.tiles[i].index].type == button->info.marble_type) {
                    app.tiles[i].highlight = 0;
                }
            }
            break;
        
        default:
            break;
    }
}

void sigmar_OnBoardReset() {
    if (app.debug_mode) { sigmar_GenerateBoard(app.sigmar, time(NULL)); }
    else { sigmar_LoadBoard(app.sigmar); }

    app.board_clear = 0;
    app.last_tick = clock();
    sigmar_PlaySound(SOUND_START, 2, 1);
    sigmar_PlayAnimation(ANIMATION_BOARD_GENERATE);
}

void sigmar_OnBoardClear() {
    app.board_clear = 1;
    app.data->wins++;
    sigmar_PlaySound(SOUND_END, 2, 1);
    sigmar_PlayAnimation(ANIMATION_BOARD_CLEAR);
}

