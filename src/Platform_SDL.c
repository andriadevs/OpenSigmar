#include "Platform_SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_rect.h"
#include "Sigmar.h"

#include <X11/Xproto.h>
#include <stdlib.h>

static AppState app;

int sigmar_LoadTextures() {

    SDL_Surface *surface = NULL;

    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_TILE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.tile_width = (float) surface->w;
    app.tile_height = (float) surface->h;

    app.texture.tile = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.tile, 255);
    if (!app.texture.tile) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_HOVER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.hover = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.hover, 255);
    if (!app.texture.hover) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_MARBLE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.marble = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.marble, 255);
    if (!app.texture.marble) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    return 0;
}

int sigmar_SetupTiles() {

    app.tiles_length = app.sigmar->board_size * app.sigmar->board_size;
    app.tiles = (Tile*) malloc(sizeof(Tile) * app.tiles_length);
    int next = 0;

    // It's not perfectly centered, but it'll have to do for the time being.
    float hoffs = (app.render_width / 2.0f) - (app.tile_width * app.sigmar->board_size * 0.25);
    float voffs = (app.render_height / 2.0f) - ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size * 0.5);

    for (int y = 0; y < app.sigmar->board_size; y++) {
        for (int x = 0; x < app.sigmar->board_size; x++) {
            app.tiles[next].index = next;
            app.tiles[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) x - (float) app.tile_width / 2 * (float) y);
            app.tiles[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) y);

            next++;
        }
    }

    return 0;
}

int sigmar_RenderTile(Tile *tile) {

    SDL_FRect rect = {tile->x, tile->y, app.tile_width, app.tile_height};

    char *character = malloc(2);
    character = " ";

    int has_marble;
    SDL_SetTextureColorMod(app.texture.marble, 0, 0, 255);
    switch (app.sigmar->board[tile->index].type) {
        case MARBLE_EMPTY:
            has_marble = 0;
            break;
        case MARBLE_SALT:
            SDL_SetTextureColorMod(app.texture.marble, 200, 200, 200);
            character = "S";
            has_marble = 1;
            break;
        case MARBLE_FIRE:
            SDL_SetTextureColorMod(app.texture.marble, 255, 0, 0);
            character = "F";
            has_marble = 1;
            break;
        case MARBLE_WATER:
            SDL_SetTextureColorMod(app.texture.marble, 0, 0, 255);
            character = "W";
            has_marble = 1;
            break;
        case MARBLE_EARTH:
            SDL_SetTextureColorMod(app.texture.marble, 0, 255, 0);
            character = "E";
            has_marble = 1;
            break;
        case MARBLE_AIR:
            SDL_SetTextureColorMod(app.texture.marble, 0, 255, 255);
            character = "A";
            has_marble = 1;
            break;
        case MARBLE_VITAE:
            SDL_SetTextureColorMod(app.texture.marble, 255, 200, 200);
            character = "V";
            has_marble = 1;
            break;
        case MARBLE_MORS:
            SDL_SetTextureColorMod(app.texture.marble, 100, 0, 100);
            character = "M";
            has_marble = 1;
            break;
        case MARBLE_QUICKSILVER:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "Q";
            has_marble = 1;
            break;
        case MARBLE_LEAD:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "L";
            has_marble = 1;
            break;
        case MARBLE_TIN:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "T";
            has_marble = 1;
            break;
        case MARBLE_IRON:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "I";
            has_marble = 1;
            break;
        case MARBLE_COPPER:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "C";
            has_marble = 1;
            break;
        case MARBLE_SILVER:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "S";
            has_marble = 1;
            break;
        case MARBLE_GOLD:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            character = "G";
            has_marble = 1;
            break;
        default:
            SDL_SetTextureColorMod(app.texture.marble, 100, 100, 100);
            break;
    }

    SDL_RenderTexture(app.renderer, app.texture.tile, NULL, &rect);

    if (has_marble == 1) {
        SDL_RenderTexture(app.renderer, app.texture.marble, NULL, &rect);
    }

    SDL_RenderDebugText(app.renderer, rect.x + (rect.w / 2), rect.y + (rect.h / 2), character);

    return 0;
}

SDL_AppResult sigmar_SDL_AppInit(void **appstate, int argc, char *argv[]) {

    int rc;

    app.window = NULL;
    app.renderer = NULL;
    app.render_width = 1280;
    app.render_height = 720;
    app.mouse_x = 0;
    app.mouse_y = 0;
    app.mouse_tile = -1;
    app.tile_width = 0;
    app.tile_height = 0;
    app.tile_width_offset = 0;
    app.tile_height_offset = 14;
    app.tiles = NULL;
    app.tiles_length = 0;

    rc = sigmar_Init(&(app.sigmar));

    if (rc != 0) { return SDL_APP_FAILURE; }

    SDL_SetAppMetadata("OpenSigmar", "1.0", "com.andria.opensigmar");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("OpenSigmar", app.render_width, app.render_height, SDL_WINDOW_RESIZABLE, &app.window, &app.renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(app.renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Load Textures
    rc = sigmar_LoadTextures();
    if (rc == 1) { return SDL_APP_FAILURE; }

    // Setup Tiles
    rc = sigmar_SetupTiles();
    if (rc == 1) { return SDL_APP_FAILURE; }

    return SDL_APP_CONTINUE;
}

SDL_AppResult sigmar_SDL_AppEvent(void *appstate, SDL_Event *event) {

    switch (event->type) {
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            SDL_ConvertEventToRenderCoordinates(app.renderer, event);
            app.mouse_x = event->motion.x;
            app.mouse_y = event->motion.y;
            app.mouse_tile = -1;

            for (int i = 0; i < app.tiles_length; i++) {
                if (app.tiles[i].x < app.mouse_x && app.tiles[i].x + app.tile_width > app.mouse_x
                    && app.tiles[i].y < app.mouse_y && app.tiles[i].y + app.tile_height > app.mouse_y) {

                    app.mouse_tile = app.sigmar->board[app.tiles[i].index].type == MARBLE_INVALID ? -1 : i;
                }
            }

            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE) {
                SelectResult rc = sigmar_Select(app.sigmar, app.tiles[app.mouse_tile].index);
            }

            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_KEY_DOWN:
            // Temporary key to reset the board, will be replaced with a proper configurable reset key later on.
            if (event->key.key == SDLK_SPACE) {
                sigmar_GenerateBoard(app.sigmar);
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

    // Clear render
    SDL_SetRenderDrawColor(app.renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app.renderer);

    SDL_SetRenderDrawColor(app.renderer, 255, 100, 0, SDL_ALPHA_OPAQUE);
    rect.w = app.tile_width;
    rect.h = app.tile_height;
    for (int i = 0; i < app.sigmar->board_size * app.sigmar->board_size; i++) {
        if (app.sigmar->board[app.tiles[i].index].type != MARBLE_INVALID) { sigmar_RenderTile(&app.tiles[i]); }
    }

    SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x = app.mouse_x;
    rect.y = app.mouse_y;
    rect.w = rect.h = 10;
    SDL_RenderFillRect(app.renderer, &rect);

    if (app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE) {
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        rect.x = (float) app.tiles[app.mouse_tile].x;
        rect.y = (float) app.tiles[app.mouse_tile].y;
        rect.w = app.tile_width;
        rect.h = app.tile_height;
        SDL_RenderTexture(app.renderer, app.texture.hover, NULL, &rect);
    }

    if (app.sigmar->num_selected != 0) {
        for (int i = 0; i < app.sigmar->num_selected; i++) {
            rect.x = (float) app.tiles[app.sigmar->prev_selected[i].index].x;
            rect.y = (float) app.tiles[app.sigmar->prev_selected[i].index].y;
            rect.w = app.tile_width;
            rect.h = app.tile_height;
            SDL_RenderTexture(app.renderer, app.texture.hover, NULL, &rect);
        }
    }

    SDL_RenderPresent(app.renderer);

    return SDL_APP_CONTINUE;
}

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result) {}