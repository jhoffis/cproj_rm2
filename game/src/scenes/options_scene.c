#include "options_scene.h"
#include "GLFW/glfw3.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include "text.h"
#include <stdio.h>

static void goback_btn(void) {
    printf("click\n");
    change_scene_back();
}

void options_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Go back", (f32_v2){0, 0}, goback_btn, anchor_top_left);
    render_text((f32_v2){0, 0}, anchor_mid, "Here be options roaming... in the future");
}

void options_scene_key(i32 key, i32 scancode, i32 action, i32 mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) goback_btn();
}
