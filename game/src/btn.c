#include "btn.h"
#include "file_util.h"
#include "game_state.h"
#include "sprite2d.h"
#include "text.h"
#include "window.h"

u32 sprite_id;

void init_btns(void) {
    sprite_id = sprite2D_create("button", 0, anchor_bottom_left);
}

void render_btn_func(char *text, void (*func)(void)) {

}

void render_btn(char *text, f32_v2 pos) {

    auto scale = 0.25;
    bool above = false;
    u32 realposX = pos.x * game_state.window.height;
    u32 realposY = pos.y * game_state.window.height;
    u32 sizeposX = realposX + ((f32) game_state.window.height / 2.0 * sprites[sprite_id].img->ratio * scale);
    u32 sizeposY = realposY + ((f32) game_state.window.height / 2.0 * scale);
    if (mouse_xpos > realposX &&
        mouse_ypos > realposY && 
        mouse_xpos < sizeposX &&
        mouse_ypos < sizeposY) {
        // printf("above %f %f \n", mouse_xpos, mouse_ypos);
        sprites[sprite_id].hovered = true;
    } else {
        sprites[sprite_id].hovered = false;
    }

    sprite2D_pos(sprite_id, pos);
    sprite2D_scale(sprite_id, scale);
    sprite2D_draw(sprite_id);
    pos.y += 0.04;
    pos.x += 0.04;
    render_text(text, pos, 1, (f32_v3) {1, 1, 1});
}
