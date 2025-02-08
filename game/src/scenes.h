#pragma once

typedef enum {
    scene_main, 
    scene_options, 
    scene_map_creator, 
    scene_lobby, 
    scene_store, 
    scene_race, 
    scene_finish, 
    scene_win, 
} scene_t;
extern scene_t current_scene;

void scenes_render(void);
