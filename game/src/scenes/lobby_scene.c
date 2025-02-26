#include "lobby_scene.h"
#include "btn.h"
#include "scenes.h"

static void goback_btn(void) {
    change_scene_back();
}

static void ready_btn(void) {
    change_scene(scene_race, false);
}

void lobby_scene_render(void) {
    render_btn("Go back", (f32_v2){0, 0}, goback_btn, anchor_top_left);
    render_btn("Ready", (f32_v2){-0.025, 0.025}, ready_btn, anchor_right);
}
