#include "store_scene.h"
#include "btn.h"
#include "play/player.h"
#include "renderer.h"
#include "scenes.h"
#include "text.h"

static void ready_btn(void) {
    // prepare_new_race();
    // change_scene(scene_race, false);
}

void store_scene_render(void) {
    gfx_set_depth(false);
    render_btn("Ready", (f32_v2){-0.025, 0.025}, ready_btn, anchor_right);
    render_print("My money: $%d", my_player->bank.money);
    render_print("My points: %dp", my_player->bank.points);

    for (int i = 0; i < num_players; i++) {
        if (my_player->player_index == i) continue;
        render_print("Player %d, $%d, %dp", i, all_players[i].bank.money, all_players[i].bank.points);
    }
}
