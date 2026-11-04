#ifdef SIGMAR_SDL
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static AppState app;

int sigmar_LoadTextures() {

    int rc = 0;
    char dir[128];
    SDL_Surface *surface = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    app.texture[TEXTURE_INVALID] = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_free(surface);

    for (int i = 0; i < TEXTURE_INVALID; i++) {
        strcpy(dir, TEXTURE_DIR);
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
        rc = 1;
    }

    app.tile_width = app.texture[TEXTURE_TILE]->w;
    app.tile_height = app.texture[TEXTURE_TILE]->h;

    app.hud_height = app.tile_height * 1.25f;

    return rc;

}

int sigmar_LoadSounds() {
    
    char rc = 0;
    char dir[128];
    SDL_AudioSpec spec;

    for (int i = 0; i < SOUND_INVALID; i++) {
        strcpy(dir, SOUND_DIR);
        strcat(dir, sound_filename[i]);
        SDL_LoadWAV(dir, &spec, &app.sound[i], &app.sound_length[i]);

        if (!app.sound[i]) { SDL_Log("Couldn't load .wav file: %s", SDL_GetError()); rc = 1;}
        else { *app.audio_spec = spec; }
    }

    return rc;

}

int sigmar_SetupBoard() {

    const int half_board = (int) (app.sigmar->board_size / 2);

    app.tiles_length = app.sigmar->board_size * app.sigmar->board_size;
    app.tiles = (Tile*) malloc(sizeof(Tile) * app.tiles_length);
    int next = 0;

    float hoffs = (app.render_width / 2.0f) - ((app.tile_width - app.tile_width_offset) * (app.sigmar->board_size + 1) * 0.25f) - (app.tile_width_offset * 0.5f);
    float voffs = ((app.render_height - app.hud_height)  / 2.0f) - ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size * 0.5f) - (app.tile_height_offset * 0.5f);

    for (int y = 0; y < app.sigmar->board_size; y++) {
        for (int x = 0; x < app.sigmar->board_size; x++) {
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
        app.sprites[next].texture = app.texture[TEXTURE_EDGE_NW];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_N];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) -1);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_NE];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (half_board + i + 1) - (float) app.tile_width / 2 * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_SE];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) app.sigmar->board_size - (float) app.tile_width / 2 * (float) (half_board + i + 1));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_S];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_SW];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) i - (float) app.tile_width / 2 * (float) (half_board + i + 1));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;
    }

    // Border corner sprites
    app.sprites[next].texture = app.texture[TEXTURE_CORNER_NE];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) half_board - (float) app.tile_width / 2 * -1.0f);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * -1.0f);
    next++;

    app.sprites[next].texture = app.texture[TEXTURE_CORNER_W];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) half_board);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * half_board);
    next++;

    app.sprites[next].texture = app.texture[TEXTURE_CORNER_SW];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) half_board - (float) app.tile_width / 2 * (float) app.sigmar->board_size);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size);
    next++;

    app.sprites[next].texture = app.texture[TEXTURE_CORNER_SE];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) app.sigmar->board_size - (float) app.tile_width / 2 * (float) app.sigmar->board_size);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size);
    next++;

    app.sprites[next].texture = app.texture[TEXTURE_CORNER_E];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) app.sigmar->board_size - (float) app.tile_width / 2 * (float) half_board);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * half_board);
    next++;

    app.sprites[next].texture = app.texture[TEXTURE_CORNER_NW];
    app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) -1.0f);
    app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * -1.0f);
    next++;

    // Setup buttons

    app.buttons_length = 1;
    app.buttons = (Button *) malloc(sizeof(Button) * app.buttons_length);

    app.buttons[0].rect.x = 16.0f;
    app.buttons[0].rect.y = app.render_height - ((app.hud_height / 2.0f) + (app.texture[TEXTURE_WINS]->h / 2.0f));
    app.buttons[0].rect.w = 64.0f;
    app.buttons[0].rect.h = 64.0f;
    app.buttons[0].texture = app.texture[TEXTURE_RESET];
    app.buttons[0].action = BUTTON_RESET;
    
}

int sigmar_RenderTile(Tile *tile) {

    SDL_FRect rect = {tile->x + tile->x_offset, tile->y + tile->y_offset, app.tile_width, app.tile_height};

    const MarbleType marble = app.sigmar->board[tile->index].type;
    const MarbleState state = app.sigmar->board[tile->index].state;

    SDL_Texture *symbol;
    ColorRGBA color;

    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_TILE], NULL, &rect);

    if (marble != MARBLE_EMPTY && marble != MARBLE_INVALID) {

        symbol = app.texture[type_symbol[marble]];
        color = type_color[marble];

        if (state == MARBLE_SELECTED) { rect.y -= 2; }
        else if (state == MARBLE_LOCKED) { color.a = 150; }

        SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(app.texture[TEXTURE_MARBLE], color.a * tile->marble_alpha_mult);
        SDL_SetTextureAlphaMod(symbol, color.a * tile->marble_alpha_mult);

        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_MARBLE], NULL, &rect);
        SDL_RenderTexture(app.renderer, symbol, NULL, &rect);

    }

    return 0;
}

int sigmar_RenderNumber(float x, float y, ColorRGBA *color, int number) {

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

    return 0;
}

int sigmar_RenderBackground() {

    SDL_FRect rect = {0, 0, app.render_width, app.render_height};

    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_BACKGROUND], NULL, &rect);

    // HUD background rendering
    rect.y = app.render_height - app.hud_height;
    rect.h = app.hud_height;

    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(app.renderer, &rect);

    SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(app.renderer, rect.x, rect.y, rect.x + app.render_width, rect.y);

    return 0;

}

int sigmar_RenderHud() {

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

    rect.x = app.render_width - 16.0f - app.texture[TEXTURE_WINS]->w;
    rect.y = app.render_height - ((app.hud_height / 2.0f) + (app.texture[TEXTURE_WINS]->h / 2.0f));
    rect.w = app.texture[TEXTURE_WINS]->w;
    rect.h = app.texture[TEXTURE_WINS]->h;

    SDL_SetTextureAlphaMod(app.texture[TEXTURE_WINS], 200);
    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_WINS], NULL, &rect);

    SDL_RenderDebugTextFormat(app.renderer, rect.x + 58, rect.y + 42, "%10lu", app.data->wins);


    //rect.x = 16.0f;
    //SDL_SetTextureAlphaMod(app.texture[TEXTURE_RESET], 200);
    //SDL_RenderTexture(app.renderer, app.texture[TEXTURE_RESET], NULL, &rect);

    for (int i = 0; i < app.buttons_length; i++) {
        SDL_SetTextureAlphaMod(app.buttons[i].texture, 200);
        SDL_RenderTexture(app.renderer, app.buttons[i].texture, NULL, &app.buttons[i].rect);
    }
    
    //SDL_RenderDebugTextFormat(app.renderer, rect.x + 30, rect.y + 18, "WINS:");
    
    //SDL_RenderDebugTextFormat(app.renderer, rect.x + 30, rect.y + 30, "%lu", app.data->wins);

    if (app.is_board_clear) {
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

    return 0;
}

int sigmar_PlaySound(SoundID sound, int stream) {

    if (stream == 0 || // Sounds cannot be played in stream 0.
        app.sound[sound] == NULL) { return 1; }

    SDL_ClearAudioStream(app.audio_stream[stream]);
    SDL_PutAudioStreamData(app.audio_stream[stream], app.sound[sound], app.sound_length[sound]);

    return 0;
}

int sigmar_PlayMusic(SoundID sound) {

    if (app.sound[sound] == NULL) { return 1; }

    // Add more data to the buffer if the currently queued data is shorter than the song to be queued.
    if (SDL_GetAudioStreamQueued(app.audio_stream[0]) < (int) app.sound_length[sound]) {
        SDL_PutAudioStreamData(app.audio_stream[0], app.sound[sound], app.sound_length[sound]);
    }

    return 0;
}

int sigmar_PlayAnimation(AnimationID animation) {

    int i;

    for (i = 0; i < app.tiles_length; i++) {
        app.tiles[i].x_offset = 0;
        app.tiles[i].y_offset = 0;
        app.tiles[i].marble_alpha_mult = 1.0f;
    }

    app.current_animation = animation;
    app.animation_position = 0.0f;

    return 0;

}

int sigmar_AnimationFrame(double delta) {

    if (app.current_animation == ANIMATION_INVALID) { return 0; }

    int i, x, y;
    double dist;
    const int frame = (int) (app.animation_position * ANIMATION_FPS);
    const int center_tile = (int) app.sigmar->board_size * app.sigmar->board_size * 0.5;
    const int center_x = center_tile % app.sigmar->board_size;
    const int center_y = center_tile / app.sigmar->board_size;

    switch (app.current_animation) {
        case ANIMATION_BOARD_CLEAR:
            for (i = 0; i < app.tiles_length; i++) {
                x = i % app.sigmar->board_size;
                y = i / app.sigmar->board_size;
                dist = SDL_sqrt(SDL_pow(x - center_x, 2) + SDL_pow(y - center_y, 2));

                if (dist <= frame + 0.5f && dist >= frame - 0.5f) { app.tiles[i].y_offset = -2; }
                else { app.tiles[i].y_offset = 0; }
            }
            break;

        case ANIMATION_BOARD_GENERATE:
            for (i = 0; i < app.tiles_length; i++) {
                x = i % app.sigmar->board_size;
                y = i / app.sigmar->board_size;
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

    return 0;
}

int sigmar_DataInit(SigmarData **data) {

    *data = malloc(sizeof(SigmarData));

    (*data)->wins = 0;
    
    (*data)->music_volume = 0.75f;
    (*data)->sound_volume = 0.75f;

    return 0;

}

int sigmar_LoadData(SigmarData *data) {

    size_t filesize = sizeof(SigmarData);
    SigmarData *file = (SigmarData *) SDL_LoadFile("./sav", &filesize);

    if (!file) { return 1; }

    data->wins = file->wins;
    
    data->music_volume = file->music_volume;
    data->sound_volume = file->sound_volume;

    SDL_free(file);

    return 0;

}

int sigmar_SaveData(SigmarData *data) {

    SDL_SaveFile("./sav", data, sizeof(SigmarData));

    return 0;

}

SDL_AppResult sigmar_SDL_AppInit(void **appstate, int argc, char *argv[]) {

    int rc;

    app.window = NULL;
    app.renderer = NULL;
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

    rc = sigmar_DataInit(&(app.data));

    if (rc != 0) { return SDL_APP_FAILURE; }
    else { sigmar_LoadData(app.data); }

    rc = sigmar_Init(&(app.sigmar));

    if (rc != 0) { return SDL_APP_FAILURE; }

    SDL_SetAppMetadata("OpenSigmar", "1.0", "com.andria.opensigmar");

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

    // Load Sounds
    rc = sigmar_LoadSounds();
    if (rc == 1) { SDL_Log("WARNING: One or more sound files failed to load!"); }

    // Initialize Audio Streams
    for (int i = 0; i < NUM_AUDIO_STREAMS; i++) {
        app.audio_stream[i] = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, app.audio_spec, NULL, NULL);
        if (!app.audio_stream[i]) {
            SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        } else {
            SDL_SetAudioStreamGain(app.audio_stream[i], BASE_GAIN * app.data->sound_volume);
            SDL_ResumeAudioStreamDevice(app.audio_stream[i]);
        }
    }

    SDL_SetAudioStreamGain(app.audio_stream[0], BASE_GAIN * app.data->music_volume);

    // Load Textures
    rc = sigmar_LoadTextures();
    if (rc == 1) { SDL_Log("WARNING: One or more texture files failed to load!"); }

    // Setup Board
    rc = sigmar_SetupBoard();
    if (rc == 1) { return SDL_APP_FAILURE; }

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
            app.mouse_tile = -1;

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

            // Handles mouse click event, attempts a select/match if applicable.
            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state != MARBLE_LOCKED) {
                    SelectResult rc = sigmar_Select(app.sigmar, app.tiles[app.mouse_tile].index);

                    switch (rc) {
                        case SELECT_SUCCESS:
                            sigmar_PlaySound(SOUND_SELECT, 1);
                            break;

                        case SELECT_DESELECT:
                            sigmar_PlaySound(SOUND_SELECT, 1);
                            break;

                        case SELECT_MATCH:
                            sigmar_PlaySound(SOUND_MATCH, 1);
                            break;

                        default:
                            break;
                    }
                } else {
                    // Handle interactable ui
                    for (int i = 0; i < app.buttons_length; i++) {
                        if (SDL_PointInRectFloat(&app.mouse_position, &app.buttons[i].rect)) {
                            sigmar_OnButtonPress(app.buttons[i].action);
                        }
                    }
                }
            }

            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_MOUSE_BUTTON_UP:
            
            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_KEY_DOWN:
            // Temporary key to reset the board, will be replaced with a proper configurable reset key later on.
            if (event->key.key == SDLK_SPACE) {
                sigmar_ResetBoard(app.sigmar);
            }

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
    SDL_SetRenderDrawColor(app.renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app.renderer);

    // Render background
    sigmar_RenderBackground();

    // Render tiles
    SDL_SetRenderDrawColor(app.renderer, 255, 100, 0, SDL_ALPHA_OPAQUE);
    rect.w = app.tile_width;
    rect.h = app.tile_height;
    for (int i = 0; i < app.sigmar->board_size * app.sigmar->board_size; i++) {
        if (app.sigmar->board[app.tiles[i].index].type != MARBLE_INVALID) { sigmar_RenderTile(&app.tiles[i]); }
    }

    //Render sprites
    for (int i = 0; i < app.sprites_length; i++) {
        rect.x = app.sprites[i].x;
        rect.y = app.sprites[i].y;
        SDL_RenderTexture(app.renderer, app.sprites[i].texture, NULL, &rect);
    }

    // Render hover
    if (app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE) {
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        rect.x = (float) app.tiles[app.mouse_tile].x;
        rect.y = (float) app.tiles[app.mouse_tile].y;
        rect.w = app.tile_width;
        rect.h = app.tile_height;
        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HOVER], NULL, &rect);
    }

    // Render selection
    if (app.sigmar->num_selected != 0) {
        for (int i = 0; i < app.sigmar->num_selected; i++) {
            rect.x = (float) app.tiles[app.sigmar->prev_selected[i].index].x;
            rect.y = (float) app.tiles[app.sigmar->prev_selected[i].index].y;
            rect.w = app.tile_width;
            rect.h = app.tile_height;
            SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HOVER], NULL, &rect);
        }
    }

    sigmar_RenderHud();

    SDL_RenderPresent(app.renderer);

    sigmar_PlayMusic(SOUND_MUSIC);

    return SDL_APP_CONTINUE;
}

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result) {
    
    sigmar_SaveData(app.data);

}

void sigmar_OnButtonPress(ButtonAction action) {

    switch (action) {
        case BUTTON_RESET:
            sigmar_ResetBoard(app.sigmar);
            break;

        default:
        
            break;

    }

}

void sigmar_OnBoardReset() {

    app.is_board_clear = 0;
    sigmar_PlaySound(SOUND_START, 2);
    sigmar_PlayAnimation(ANIMATION_BOARD_GENERATE);

}

void sigmar_OnBoardClear() {

    app.is_board_clear = 1;
    app.data->wins++;
    sigmar_PlaySound(SOUND_END, 2);
    sigmar_PlayAnimation(ANIMATION_BOARD_CLEAR);

}
#endif