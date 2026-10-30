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

    // BACKGROUND
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_BACKGROUND);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.background = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.background, 255);
    if (!app.texture.background) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // TILE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_TILE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.tile_width = (float) surface->w;
    app.tile_height = (float) surface->h;

    app.texture.tile = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.tile, 255);
    if (!app.texture.tile) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE NW
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_NW);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_nw = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_nw, 255);
    if (!app.texture.edge_nw) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE N
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_N);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_n = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_n, 255);
    if (!app.texture.edge_n) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE NE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_NE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_ne = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_ne, 255);
    if (!app.texture.edge_ne) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE SE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_SE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_se = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_se, 255);
    if (!app.texture.edge_se) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE S
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_S);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_s = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_s, 255);
    if (!app.texture.edge_s) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EDGE SW
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EDGE_SW);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.edge_sw = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.edge_sw, 255);
    if (!app.texture.edge_sw) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // HOVER
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_HOVER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.hover = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.hover, 255);
    if (!app.texture.hover) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // MARBLE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_MARBLE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.marble = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.marble, 255);
    if (!app.texture.marble) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // SALT
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_SALT);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.salt = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.salt, 255);
    if (!app.texture.marble) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // FIRE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_FIRE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.fire = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.fire, 255);
    if (!app.texture.fire) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // WATER
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_WATER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.water = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.water, 255);
    if (!app.texture.water) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // EARTH
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_EARTH);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.earth = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.earth, 255);
    if (!app.texture.earth) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // AIR
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_AIR);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.air = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.air, 255);
    if (!app.texture.air) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // VITAE
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_VITAE);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.vitae = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.vitae, 255);
    if (!app.texture.vitae) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // MORS
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_MORS);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.mors = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.mors, 255);
    if (!app.texture.mors) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // QUICKSILVER
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_QUICKSILVER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.quicksilver = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.quicksilver, 255);
    if (!app.texture.quicksilver) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // LEAD
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_LEAD);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.lead = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.lead, 255);
    if (!app.texture.lead) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // TIN
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_TIN);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.tin = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.tin, 255);
    if (!app.texture.tin) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // IRON
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_IRON);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.iron = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.iron, 255);
    if (!app.texture.iron) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // COPPER
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_COPPER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.copper = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.copper, 255);
    if (!app.texture.copper) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // SILVER
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_SILVER);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.silver = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.silver, 255);
    if (!app.texture.silver) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    // GOLD
    surface = SDL_LoadPNG(ASSETS_DIR TEXTURE_GOLD);
    if (!surface) { SDL_Log("Couldn't load png: %s", SDL_GetError()); return 1; }

    app.texture.gold = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_SetTextureAlphaMod(app.texture.gold, 255);
    if (!app.texture.gold) { SDL_Log("Couldn't create static texture: %s", SDL_GetError()); return 1; }

    return 0;
}

int sigmar_GetSymbolTexture(MarbleType type, SDL_Texture **texture) {
    switch (type) {
        case MARBLE_SALT:
            *texture = app.texture.salt;
            break;
        case MARBLE_FIRE:
            *texture = app.texture.fire;
            break;
        case MARBLE_WATER:
            *texture = app.texture.water;
            break;
        case MARBLE_EARTH:
            *texture = app.texture.earth;
            break;
        case MARBLE_AIR:
            *texture = app.texture.air;
            break;
        case MARBLE_VITAE:
            *texture = app.texture.vitae;
            break;
        case MARBLE_MORS:
            *texture = app.texture.mors;
            break;
        case MARBLE_QUICKSILVER:
            *texture = app.texture.quicksilver;
            break;
        case MARBLE_LEAD:
            *texture = app.texture.lead;
            break;
        case MARBLE_TIN:
            *texture = app.texture.tin;
            break;
        case MARBLE_IRON:
            *texture = app.texture.iron;
            break;
        case MARBLE_COPPER:
            *texture = app.texture.copper;
            break;
        case MARBLE_SILVER:
            *texture = app.texture.silver;
            break;
        case MARBLE_GOLD:
            *texture = app.texture.gold;
            break;
        default:
            *texture = app.texture.salt;
            break;
    }
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

    // Setup edges
    const int half_board = (int) (app.sigmar->board_size / 2);
    app.edges_length = half_board * 6;
    app.edges = (Edge*) malloc(sizeof(Edge) * app.edges_length);
    next = 0;

    for (int i = 0; i < 5; i++) {
        app.edges[next].texture = app.texture.edge_nw;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * -1.0f - (float) app.tile_width / 2 * (float) i);
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.edges[next].texture = app.texture.edge_n;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) -1);
        next++;

        app.edges[next].texture = app.texture.edge_ne;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (3 + app.sigmar->board_size - (half_board - i)) - (float) app.tile_width / 2 * (float) (app.sigmar->board_size - (half_board - i)));
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) i);
        next++;

        app.edges[next].texture = app.texture.edge_se;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (float) (3 + half_board + (half_board - i)) - (float) app.tile_width / 2 * (float) (half_board + (half_board - i)));
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
        next++;

        app.edges[next].texture = app.texture.edge_s;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * 0.5f + (float) app.tile_width * (float) i);
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * app.sigmar->board_size);
        next++;

        app.edges[next].texture = app.texture.edge_sw;
        app.edges[next].x = hoffs + ((app.tile_width - app.tile_width_offset) * (-6 + (half_board + i)) - (float) app.tile_width / 2 * (float) (half_board + i - 1));
        app.edges[next].y = voffs + ((app.tile_height - app.tile_height_offset) * (float) (half_board + i + 1));
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
    SDL_SetTextureColorMod(app.texture.marble, 0, 0, 255);
    switch (app.sigmar->board[tile->index].type) {
        case MARBLE_EMPTY:
            has_marble = 0;
            break;
        case MARBLE_SALT:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_SALT);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "S";            
            has_marble = 1;
            break;
        case MARBLE_FIRE:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_FIRE);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "F";
            has_marble = 1;
            break;
        case MARBLE_WATER:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_WATER);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "W";
            has_marble = 1;
            break;
        case MARBLE_EARTH:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_EARTH);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "E";
            has_marble = 1;
            break;
        case MARBLE_AIR:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_AIR);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "A";
            has_marble = 1;
            break;
        case MARBLE_VITAE:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_VITAE);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "V";
            has_marble = 1;
            break;
        case MARBLE_MORS:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_MORS);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "M";
            has_marble = 1;
            break;
        case MARBLE_QUICKSILVER:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_QUICKSILVER);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "Q";
            has_marble = 1;
            break;
        case MARBLE_LEAD:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_LEAD);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "L";
            has_marble = 1;
            break;
        case MARBLE_TIN:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_TIN);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "T";
            has_marble = 1;
            break;
        case MARBLE_IRON:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_IRON);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "I";
            has_marble = 1;
            break;
        case MARBLE_COPPER:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_COPPER);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "C";
            has_marble = 1;
            break;
        case MARBLE_SILVER:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_SILVER);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "S";
            has_marble = 1;
            break;
        case MARBLE_GOLD:
            SDL_SetTextureColorMod(app.texture.marble, COLOR_GOLD);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            character = "G";
            has_marble = 1;
            break;
        default:
            SDL_SetTextureColorMod(app.texture.marble, 0, 0, 0);
            sigmar_GetSymbolTexture(app.sigmar->board[tile->index].type, &symbol);
            break;
    }

    SDL_RenderTexture(app.renderer, app.texture.tile, NULL, &rect);

    if (has_marble == 1) {
        SDL_RenderTexture(app.renderer, app.texture.marble, NULL, &rect);
        SDL_RenderTexture(app.renderer, symbol, NULL, &rect);
    }

    //SDL_RenderDebugText(app.renderer, rect.x + (rect.w / 2), rect.y + (rect.h / 2), character);

    return 0;
}

int sigmar_RenderBackground() {
    SDL_FRect rect = {0, 0, app.render_width, app.render_height};

    SDL_RenderTexture(app.renderer, app.texture.background, NULL, &rect);
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

            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && app.mouse_tile != -1 && app.sigmar->board[app.mouse_tile].state == MARBLE_FREE) {
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

    for (int i = 0; i < app.edges_length; i++) {
        rect.x = app.edges[i].x;
        rect.y = app.edges[i].y;
        SDL_RenderTexture(app.renderer, app.edges[i].texture, NULL, &rect);
    }

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

    sigmar_RenderHud();

    SDL_RenderPresent(app.renderer);

    return SDL_APP_CONTINUE;
}

void sigmar_SDL_AppQuit(void *appstate, SDL_AppResult result) {}