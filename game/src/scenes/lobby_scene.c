#include "lobby_scene.h"
#include "btn.h"
#include "scenes.h"

static void goback_btn(void) {
    change_scene_back();
}

void lobby_scene_render(void) {
    render_btn("Go back", (f32_v2){0, 0}, goback_btn, anchor_top_left);
}
