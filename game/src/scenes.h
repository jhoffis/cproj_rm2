#pragma once
#include "nums.h"

typedef enum {
    scene_main, 
    scene_pause_menu,
    scene_options, 
    scene_lobby, 
    scene_map_creator, 
    scene_map_exploring, 
    scene_in_world, 
    scene_conversation,
    scene_inventory,
    scene_settlement,
    scene_party,
    scene_store, 
    scene_win, 
} scene_t;
extern scene_t current_scene;

void change_scene(scene_t s, bool remember);
void change_scene_back(void);
void change_scene_clear_stack(void);
void scenes_render(void);
void scenes_key_cb(i32 key, i32 scancode, i32 action, i32 mods);
