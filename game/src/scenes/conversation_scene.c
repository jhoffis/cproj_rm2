#include "conversation_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"
#include <stdio.h>

static void goback_btn(void) {
    printf("click\n");
    change_scene_back();
}

void conversation_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Hi how are ya?", (f32_v2){0, 0.2}, goback_btn, anchor_mid_bottom);
    render_btn("Good bye!", (f32_v2){0, 0}, goback_btn, anchor_mid_bottom);
}
