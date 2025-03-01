#include "finish_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include <stdio.h>

static void enter_store_btn(void) {
    change_scene(scene_store, false);
}

void finish_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Enter store", (f32_v2){0}, enter_store_btn, anchor_right);
}
