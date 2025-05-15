#include "map_exploring_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include "text.h"
#include <stdio.h>

static s2D map_character;

void map_init(void) {
    map_character = sprite2D_create("map_character", 0);
    sprite2D_scale(map_character, .25);
    sprites[map_character].anchor = anchor_mid;
}

static void enter_store_btn(void) {
    change_scene(scene_store, false);
}

void map_exploring_scene_render(void) {
    gfx_set_depth(false);
    sprite2D_draw(map_character);
    render_btn("Enter store", (f32_v2){0}, enter_store_btn, anchor_right);
    render_print("mappp");
}
