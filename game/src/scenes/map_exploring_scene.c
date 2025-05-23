#include "map_exploring_scene.h"
#include "GLFW/glfw3.h"
#include "btn.h"
#include "game_state.h"
#include "renderer.h"
#include "scenes.h"
#include "selection_box.h"
#include "text.h"
#include "timer_util.h"
#include <stdio.h>

/*
 * queue av ticks der nårenn ticken kommer så skal ting skje, i stedet for å inkrementere hver eneste pop.
 */

static s2D map_character;
static bool move_cam_up = false, move_cam_down = false, move_cam_left = false, move_cam_right = false;

void map_init(void) {
    map_character = sprite2D_create("map_character", 0);
    sprite2D_scale(map_character, .25);
    sprites[map_character].anchor = anchor_mid;
}

static void enter_settlement(void) {
    change_scene(scene_settlement, false);
}

static void enter_conversation(void) {
    change_scene(scene_conversation, true);
}

static void enter_inventory(void) {
    change_scene(scene_inventory, true);
}

static void enter_party(void) {
    change_scene(scene_party, true);
}

static void enter_menu(void) {
    change_scene(scene_pause_menu, true);
}

void map_exploring_scene_render(void) {

    const auto cam_movespd = 1.6;
    if (move_cam_up) {
        game_state.cam2D.pos.y -= cam_movespd * timer_delta();
    }
    if (move_cam_down) {
        game_state.cam2D.pos.y += cam_movespd * timer_delta();
    }
    if (move_cam_left) {
        game_state.cam2D.pos.x += cam_movespd * timer_delta();
    }
    if (move_cam_right) {
        game_state.cam2D.pos.x -= cam_movespd * timer_delta();
    }

    gfx_set_depth(false);
    sprite2D_pos(map_character, game_state.cam2D.pos);
    sprite2D_draw(map_character);
    render_btn("Enter town (FIX)", (f32_v2){0}, enter_settlement, anchor_right);
    render_btn("Convo (FIX)", (f32_v2){-.2, 0}, enter_conversation, anchor_right);
    render_btn("Inventory", (f32_v2){-.4, 0}, enter_inventory, anchor_right);
    render_btn("Party", (f32_v2){-.6, 0}, enter_party, anchor_right);
    render_btn("Open menu", (f32_v2){-.8, 0}, enter_menu, anchor_right);
    render_print("mappp");
    sel_box_render();
}
/**
 * TODO           if (SceneData::dragging) {
                Camera::m_cam.pos.x += SceneData::xWorldDragCam - SceneData::xWorld;
                Camera::m_cam.pos.y += SceneData::yWorldDragCam - SceneData::yWorld;
            }
 */

void map_exploring_scene_key(i32 key, i32 scancode, i32 action, i32 mods) {
    if (action == GLFW_PRESS) {
        switch (key) { 
            case GLFW_KEY_ESCAPE: 
                enter_menu();
                break;
            case GLFW_KEY_UP: 
                move_cam_up = true;
                break;
            case GLFW_KEY_DOWN: 
                move_cam_down = true;
                break;
            case GLFW_KEY_LEFT: 
                move_cam_left = true;
                break;
            case GLFW_KEY_RIGHT: 
                move_cam_right = true;
                break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) { 
            case GLFW_KEY_UP: 
                move_cam_up = false;
                break;
            case GLFW_KEY_DOWN: 
                move_cam_down = false;
                break;
            case GLFW_KEY_LEFT: 
                move_cam_left = false;
                break;
            case GLFW_KEY_RIGHT: 
                move_cam_right = false;
                break;
        }
    }
}
