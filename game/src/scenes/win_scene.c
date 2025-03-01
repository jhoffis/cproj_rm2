#include "win_scene.h"
#include "btn.h"
#include "renderer.h"
#include "scenes.h"

static void go_to_lobby_btn(void) {
    change_scene(scene_lobby, false);
}

void win_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Lobby", (f32_v2){-0.025, 0.025}, go_to_lobby_btn, anchor_right);
}
