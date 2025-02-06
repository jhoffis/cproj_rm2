#include "btn.h"
#include "file_util.h"
#include "game_state.h"
#include "sprite2d.h"
#include "text.h"
#include "window.h"

image_data btn_img;
u32 sprite_id;

void init_btns(void) {
    sprite_id = sprite2D_create("button", 0, anchor_bottom_left);
}

void render_btn_func(char *text, void (*func)(void)) {

}

void render_btn(char *text, f32_v2 pos) {

    bool above = false;
    if (mouse_xpos > pos.x * game_state.window.width &&
        mouse_ypos > pos.y * game_state.window.height) {
        printf("above %f %f \n", mouse_xpos, mouse_ypos);
    }

    sprite2D_pos(sprite_id, pos);
    sprite2D_scale(sprite_id, 0.25);
    sprite2D_draw(sprite_id);
    pos.y += 0.04;
    pos.x += 0.04;
    render_text(text, pos, 1, (f32_v3) {1, 1, 1});
}
