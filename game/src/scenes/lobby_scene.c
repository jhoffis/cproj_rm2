#include "lobby_scene.h"
#include "btn.h"
#include "play/car.h"
#include "play/gm.h"
#include "play/player.h"
#include "renderer.h"
#include "scenes.h"
#include "text.h"

/****
 * Character creation and initial settings
 ****/

static s2D background;
static s2D boy;
static s2D girl;

void lobby_init(void) {
    background = sprite2D_create("lobby_background", 0);
    sprite2D_scale(background, 2);
    boy = sprite2D_create("boy", 0);
    girl = sprite2D_create("girl", 0);
}

static void goback_btn(void) {
    change_scene(scene_main, false);
}

static void ready_btn(void) {
    gm_init();
    change_scene(scene_map_exploring, false);

}

static void next_gender_btn(void) {
    my_player->body = (GET_INT_VALUE(my_player->body, BODY_GENDER) + 1) % 2;
}

void lobby_scene_render(void) {
    gfx_set_depth(false);
    sprite2D_draw(background);
    switch (GET_INT_VALUE(my_player->body, BODY_GENDER)) {
        case BODY_GENDER_MALE:
            sprite2D_draw(boy);
            break;
        case BODY_GENDER_FEMALE:
            sprite2D_draw(girl);
            break;
    }
    render_btn("Go back", (f32_v2){0.4, 0}, goback_btn, anchor_top_left);
    render_btn("Ready", (f32_v2){-0.025, 0.025}, ready_btn, anchor_right);
    render_btn("Next gender", (f32_v2){-0., 0.025}, next_gender_btn, anchor_mid_bottom);

    render_print("gender %d", my_player->body);
    render_print("Number of players %d", num_players);
    for (int i = 0; i < num_players; i++) {
        render_print("Player %d", i);
    }
}
