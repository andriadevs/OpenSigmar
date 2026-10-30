#include "Platform_SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "Sigmar.h"

#include <stdlib.h>
#include <stdio.h>

static AppState app;

int sigmar_LoadTextures() {

    SDL_Surface *surface = NULL;
    char dir[128];

    for (int i = 0; i < TEXTURE_INVALID; i++) {
        strcpy(dir, TEXTURE_DIR);
        strcat(dir, filename_map[i]);
        surface = SDL_LoadPNG(dir);
        if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

        app.texture[i] = SDL_CreateTextureFromSurface(app.renderer, surface);
        SDL_SetTextureAlphaMod(app.texture[i], 255);
        if (!app.texture[i]) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }
    }

    app.tile_width = app.texture[TEXTURE_TILE]->w;
    app.tile_height = app.texture[TEXTURE_TILE]->h;

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

    // Setup sprites
    const int half_board = (int) (app.sigmar->board_size / 2);
    app.sprites_length = half_board * 6;
    app.sprites = (Sprite*) malloc(sizeof(Sprite) * app.sprites_length);
    next = 0;

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
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (3 + app.sigmar->board_size - (half_board - i)) - (float) app.tile_width / 2 * (float) (app.sigmar->board_size - (half_board - i)));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_SE];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (3 + half_board + (half_board - i)) - (float) app.tile_width / 2 * (float) (half_board + (half_board - i)));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_S];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size);
        next++;

        app.sprites[next].texture = app.texture[TEXTURE_EDGE_SW];
        app.sprites[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (-6 + (half_board + i)) - (float) app.tile_width / 2 * (float) (half_board + i - 1));
        app.sprites[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;
    }

    return 0;
}

int sigmar_RenderTile(Tile *tile) {

    SDL_FRect rect = {tile->x, tile->y, app.tile_width, app.tile_height};

    char *character = malloc(2);
    character = " ";

    SDL_Texture *symbol;

    int has_marble;
    SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], 0, 0, 0);
    switch (app.sigmar->board[tile->index].type) {
        case MARBLE_EMPTY:
            has_marble = 0;
            break;
        case MARBLE_SALT:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_SALT);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "S";            
            has_marble = 1;
            break;
        case MARBLE_FIRE:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_FIRE);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "F";
            has_marble = 1;
            break;
        case MARBLE_WATER:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_WATER);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "W";
            has_marble = 1;
            break;
        case MARBLE_EARTH:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_EARTH);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "E";
            has_marble = 1;
            break;
        case MARBLE_AIR:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_AIR);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "A";
            has_marble = 1;
            break;
        case MARBLE_VITAE:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_VITAE);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "V";
            has_marble = 1;
            break;
        case MARBLE_MORS:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_MORS);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "M";
            has_marble = 1;
            break;
        case MARBLE_QUICKSILVER:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_QUICKSILVER);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "Q";
            has_marble = 1;
            break;
        case MARBLE_LEAD:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_LEAD);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "L";
            has_marble = 1;
            break;
        case MARBLE_TIN:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_TIN);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "T";
            has_marble = 1;
            break;
        case MARBLE_IRON:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_IRON);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "I";
            has_marble = 1;
            break;
        case MARBLE_COPPER:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_COPPER);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "C";
            has_marble = 1;
            break;
        case MARBLE_SILVER:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_SILVER);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "S";
            has_marble = 1;
            break;
        case MARBLE_GOLD:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], COLOR_GOLD);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            character = "G";
            has_marble = 1;
            break;
        default:
            SDL_SetTextureColorMod(app.texture[TEXTURE_MARBLE], 0, 0, 0);
            symbol = app.texture[symbol_map[app.sigmar->board[tile->index].type]];
            break;
    }

    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_TILE], NULL, &rect);

    if (has_marble == 1) {
        if (app.sigmar->board[tile->index].state == MARBLE_SELECTED) { rect.y -= 2; }
        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_MARBLE], NULL, &rect);
        SDL_RenderTexture(app.renderer, symbol, NULL, &rect);
    }

    //SDL_RenderDebugText(app.renderer, rect.x + (rect.w / 2), rect.y + (rect.h / 2), character);

    return 0;
}

int sigmar_RenderBackground() {
    SDL_FRect rect = {0, 0, app.render_width, app.render_height};

    SDL_RenderTexture(app.renderer, app.texture[TEXTURE_BACKGROUND], NULL, &rect);

    return 0;
}

int sigmar_RenderHud() {

    const int hud_width = 100;
    const int hud_height = 500;

    SDL_FRect rect = {app.render_width / 8.0f, app.render_height / 2.0f - hud_height / 2.0f, hud_width, hud_height};

    const float text_offs = 16.0f;

    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(app.renderer, &rect);

    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(app.renderer, &rect);

    for (int i = 0; i < NUM_MARBLES; i++) {
        char string[32];
        sprintf(string, "%d", app.sigmar->marble_counts[i]);
        SDL_RenderDebugText(app.renderer, rect.x + text_offs, rect.y + text_offs + (text_offs * i), string);
    }

    int board_clear;
    sigmar_IsBoardClear(app.sigmar, &board_clear);
    if (board_clear == 1) {
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
    app.tile_height_offset = 16;
    app.tiles = NULL;
    app.tiles_length = 0;

    rc = sigmar_Init(&(app.sigmar));

    if (rc != 0) { return SDL_APP_FAILURE; }

    SDL_SetAppMetadata("OpenSigmar", "1.0", "com.andria.opensigmar");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("OpenSigmar", app.render_width, app.render_height, 0, &app.window, &app.renderer)) {
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

            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state != MARBLE_LOCKED) {
                SelectResult rc = sigmar_Select(app.sigmar, app.tiles[app.mouse_tile].index);
            }

            return SDL_APP_CONTINUE;
        
        case SDL_EVENT_KEY_DOWN:
            // Temporary key to reset the board, will be replaced with a proper configurable reset key later on.
            if (event->key.key == SDLK_SPACE) {
                sigmar_Restart(app.sigmar);
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

    // Render background
    sigmar_RenderBackground();

    SDL_SetRenderDrawColor(app.renderer, 255, 100, 0, SDL_ALPHA_OPAQUE);
    rect.w = app.tile_width;
    rect.h = app.tile_height;
    for (int i = 0; i < app.sigmar->board_size * app.sigmar->board_size; i++) {
        if (app.sigmar->board[app.tiles[i].index].type != MARBLE_INVALID) { sigmar_RenderTile(&app.tiles[i]); }
    }

    /*SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x = app.mouse_x;
    rect.y = app.mouse_y;
    rect.w = rect.h = 10;
    SDL_RenderFillRect(app.renderer, &rect);*/

    for (int i = 0; i < app.sprites_length; i++) {
        rect.x = app.sprites[i].x;
        rect.y = app.sprites[i].y;
        SDL_RenderTexture(app.renderer, app.sprites[i].texture, NULL, &rect);
    }

    if (app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE) {
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        rect.x = (float) app.tiles[app.mouse_tile].x;
        rect.y = (float) app.tiles[app.mouse_tile].y;
        rect.w = app.tile_width;
        rect.h = app.tile_height;
        SDL_RenderTexture(app.renderer, app.texture[TEXTURE_HOVER], NULL, &rect);
    }

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

    return SDL_APP_CONTINUE;
}

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result) {}