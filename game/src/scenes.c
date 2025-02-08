#include "scenes.h"
#include "scenes/finish_scene.h"
#include "scenes/lobby_scene.h"
#include "scenes/main_scene.h"
#include "scenes/map_creator_scene.h"
#include "scenes/options_scene.h"
#include "scenes/race_scene.h"
#include "scenes/store_scene.h"
#include "scenes/win_scene.h"

scene_t current_scene = scene_main;

void scenes_render(void) {
    switch (current_scene) {
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
