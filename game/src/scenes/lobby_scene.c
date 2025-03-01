#include "lobby_scene.h"
#include "btn.h"
#include "play/gm.h"
#include "renderer.h"
#include "scenes.h"
#include "scenes/race_scene.h"
#include "text.h"

static void goback_btn(void) {
    change_scene(scene_main, false);
}

static void ready_btn(void) {
    gm_init();
    prepare_new_race();
    change_scene(scene_race, false);
}

void lobby_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Go back", (f32_v2){0.4, 0}, goback_btn, anchor_top_left);
    render_btn("Ready", (f32_v2){-0.025, 0.025}, ready_btn, anchor_right);

    render_print("Number of players %d", num_players);
    for (int i = 0; i < num_players; i++) {
        render_print("Player %d", i);
    }
}
