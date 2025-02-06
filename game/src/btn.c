#include "btn.h"
#include "file_util.h"
#include "sprite2d.h"
#include "text.h"

image_data btn_img;
u32 sprite_id;

void init_btns(void) {
    sprite_id = sprite2D_create("button", 0, anchor_bottom_left);
}

void render_btn_func(char *text, void (*func)(void)) {

}

void render_btn(char *text, f32_v2 pos) {
    sprite2D_pos(sprite_id, pos);
    sprite2D_scale(sprite_id, 0.2);
    sprite2D_draw(sprite_id);
    render_text(text, pos, 1, (f32_v3) {1, 1, 1});
}
