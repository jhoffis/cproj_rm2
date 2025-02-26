#include "scenes.h"
#include "game_state.h"
#include "scenes/finish_scene.h"
#include "scenes/lobby_scene.h"
#include "scenes/main_scene.h"
#include "scenes/map_creator_scene.h"
#include "scenes/options_scene.h"
#include "scenes/race_scene.h"
#include "scenes/store_scene.h"
#include "scenes/win_scene.h"

static scene_t history_previous_scene;

void change_scene(scene_t s, bool remember) {
    if (remember) history_previous_scene = game_state.current_scene;
    game_state.current_scene = s;
}

void change_scene_back(void) {
    game_state.current_scene = history_previous_scene;
}

void scenes_render(void) {
    switch (game_state.current_scene) {
        case scene_main:
            main_scene_render();
            break;
        case scene_options:
            options_scene_render();
            break;
        case scene_map_creator:
            map_creator_scene_render();
            break;
        case scene_lobby:
            lobby_scene_render();
            break;
        case scene_store:
            store_scene_render();
            break;
        case scene_race:
            race_scene_render();
            break;
        case scene_finish:
            finish_scene_render();
            break;
        case scene_win:
            win_scene_render();
            break;
    }
}
