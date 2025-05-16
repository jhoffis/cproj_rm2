#include "pause_menu_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include <stdio.h>

static void return_to_whence_you_came(void) {
    change_scene_back();
}

static void exit_to_main_menu(void) {
    change_scene_clear_stack();
    change_scene(scene_main, false);
}

static void go_to_options(void) {
    change_scene(scene_options, true);
}

void pause_menu_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Return to the map", (f32_v2){0}, return_to_whence_you_came, anchor_mid);
    render_btn("Options", (f32_v2){0, -.2}, go_to_options, anchor_mid);
    render_btn("Exit to main menu", (f32_v2){0, -.4}, exit_to_main_menu, anchor_mid);
}
