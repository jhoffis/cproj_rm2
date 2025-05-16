#include "map_exploring_scene.h"
#include "GLFW/glfw3.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include "text.h"
#include <stdio.h>

/*
 * queue av ticks der nårenn ticken kommer så skal ting skje, i stedet for å inkrementere hver eneste pop.
 */

static s2D map_character;


void map_init(void) {
    map_character = sprite2D_create("map_character", 0);
    sprite2D_scale(map_character, .25);
    sprites[map_character].anchor = anchor_mid;
}

static void enter_settlement(void) {
    change_scene(scene_settlement, false);
}

static void enter_conversation(void) {
    change_scene(scene_conversation, false);
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
    gfx_set_depth(false);
    sprite2D_draw(map_character);
    render_btn("Enter town (FIX)", (f32_v2){0}, enter_settlement, anchor_right);
    render_btn("Convo (FIX)", (f32_v2){-.2, 0}, enter_conversation, anchor_right);
    render_btn("Inventory", (f32_v2){-.4, 0}, enter_inventory, anchor_right);
    render_btn("Party", (f32_v2){-.6, 0}, enter_party, anchor_right);
    render_btn("Open menu", (f32_v2){-.8, 0}, enter_menu, anchor_right);
    render_print("mappp");
}


void map_exploring_scene_key(i32 key, i32 scancode, i32 action, i32 mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) enter_menu();
}
