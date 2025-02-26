#include "options_scene.h"
#include "btn.h"
#include "scenes.h"
#include "text.h"
#include <stdio.h>

static void goback_btn(void) {
    printf("click\n");
    change_scene_back();
}

void options_scene_render(void) {
    render_btn("Go back", (f32_v2){0, 0}, goback_btn, anchor_top_left);
    render_text("Here be options roaming... in the future", (f32_v2){0, 0}, anchor_mid);
}
