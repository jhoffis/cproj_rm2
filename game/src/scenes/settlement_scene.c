#include "settlement_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include <stdio.h>

static void goback_btn(void) {
    printf("click\n");
    change_scene(scene_map_exploring, false);
}

static void enter_store_btn(void) {
    change_scene(scene_store, true);
}

void settlement_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Exit settlement", (f32_v2){0.2, 0}, goback_btn, anchor_top_left);
    render_btn("Enter store", (f32_v2){0}, enter_store_btn, anchor_right);
}
