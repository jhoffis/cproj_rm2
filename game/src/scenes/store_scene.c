#include "store_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include <stdio.h>

static void ready_btn(void) {
    change_scene(scene_race, false);
}

void store_scene_render(void) {
    printf("main\n");
    gfx_set_depth(false);
    render_btn("Ready", (f32_v2){-0.025, 0.025}, ready_btn, anchor_right);
}
