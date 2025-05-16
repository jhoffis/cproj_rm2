#include "scenes.h"
#include "game_state.h"
#include "scenes/conversation_scene.h"
#include "scenes/in_world_scene.h"
#include "scenes/inventory_scene.h"
#include "scenes/lobby_scene.h"
#include "scenes/main_scene.h"
#include "scenes/map_creator_scene.h"
#include "scenes/map_exploring_scene.h"
#include "scenes/options_scene.h"
#include "scenes/party_scene.h"
#include "scenes/pause_menu_scene.h"
#include "scenes/settlement_scene.h"
#include "scenes/store_scene.h"
#include "scenes/win_scene.h"
#include <stdio.h>

#define HISTORY_PREVIOUS_MAX_HEIGHT 16

static scene_t hist_prev_stack[HISTORY_PREVIOUS_MAX_HEIGHT];
static i8 hist_prev_i;

void change_scene(scene_t s, bool remember) {
    if (remember) {
        if (hist_prev_i >= HISTORY_PREVIOUS_MAX_HEIGHT) {
            printf("ERROR HISTORY_PREVIOUS_MAX_HEIGHT\n");
        }
        hist_prev_stack[hist_prev_i] = game_state.current_scene;
        hist_prev_i++;
    }
    game_state.current_scene = s;
    printf("Amount on scene stack %d\n", hist_prev_i);
}

void change_scene_back(void) {
    if (hist_prev_i < 0) {
        printf("ERROR HISTORY_PREVIOUS_BELOW 0\n");
    }
    hist_prev_i--;
    game_state.current_scene = hist_prev_stack[hist_prev_i];
}

void change_scene_clear_stack(void) {
    hist_prev_i = 0;
}

void scenes_render(void) {
    switch (game_state.current_scene) {
        case scene_main:
            main_scene_render();
            break;
        case scene_pause_menu:
            pause_menu_scene_render();
            break;
        case scene_options:
            options_scene_render();
            break;
        case scene_lobby:
            lobby_scene_render();
            break;
        case scene_map_creator:
            map_creator_scene_render();
            break;
        case scene_map_exploring:
            map_exploring_scene_render();
            break;
        case scene_in_world:
            in_world_scene_render();
            break;
        case scene_conversation:
            conversation_scene_render();
            break;
        case scene_store:
            store_scene_render();
            break;
        case scene_win:
            win_scene_render();
            break;
        case scene_inventory:
            inventory_scene_render();
            break;
        case scene_settlement:
            settlement_scene_render();
            break;
        case scene_party:
            party_scene_render();
            break;
    }
}

void scenes_key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
    switch (game_state.current_scene) {
        // case scene_main:
        //     main_scene_render();
        //     break;
        case scene_pause_menu:
            pause_menu_scene_key(key, scancode, action, mods);
            break;
        case scene_options:
            options_scene_key(key, scancode, action, mods);
            break;
        // case scene_lobby:
        //     lobby_scene_render();
        //     break;
        // case scene_map_creator:
        //     map_creator_scene_render();
        //     break;
        case scene_map_exploring:
            map_exploring_scene_key(key, scancode, action, mods);
            break;
        // case scene_in_world:
        //     in_world_scene_render();
        //     break;
        // case scene_conversation:
        //     conversation_scene_render();
        //     break;
        // case scene_store:
        //     store_scene_render();
        //     break;
        // case scene_win:
        //     win_scene_render();
        //     break;
        case scene_inventory:
            inventory_scene_key(key, scancode, action, mods);
            break;
        // case scene_settlement:
        //     settlement_scene_render();
        //     break;
        case scene_party:
            party_scene_key(key, scancode, action, mods);
            break;
    }
}
