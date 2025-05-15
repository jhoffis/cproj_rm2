#include "btn.h"
#include "file_util.h"
#include "game_state.h"
#include "sprite2d.h"
#include "text.h"
#include "window.h"
#include <stdlib.h>

u32 sprite_id;
bool next_hovered = false;
bool something_hovered = false;
void (*hovered_func)(void);


void init_btns(void) {
    sprite_id = sprite2D_create("button", 0);
}

void render_btn(char *text, f32_v2 pos, void (*func)(void), sprite2D_anchor anchor) {
    auto scale = 0.20;
    bool above = false;

    f32 sizeX = ((f32) game_state.window.height / 2.0 * sprites[sprite_id].img->ratio * scale);
    f32 sizeY = ((f32) game_state.window.height / 2.0 * scale);
    u32 realposX = 0;
    u32 realposY = 0;
    switch (anchor) {
        case anchor_left:
            realposX = pos.x * game_state.window.height;
            realposY = pos.y * game_state.window.height;
            break;
        case anchor_right:
            realposX = game_state.window.width + pos.x * game_state.window.height - sizeX;
            realposY = pos.y * game_state.window.height;
            break;
        case anchor_mid:
            realposX = (u32) ((f32) game_state.window.width / 2) + pos.x * game_state.window.height / 2 - sizeX / 2;
            realposY = (u32) ((f32) game_state.window.height / 2) + pos.y * game_state.window.height / 2 - sizeY / 2;
            break;
        case anchor_mid_bottom:
            realposX = (u32) ((f32) game_state.window.width / 2) + pos.x * game_state.window.height / 2 - sizeX / 2;
            realposY = pos.y * game_state.window.height;
            break;
        case anchor_top_left:
            realposX = pos.x * game_state.window.height;
            realposY = game_state.window.height + pos.y * game_state.window.height - sizeY;
            break;        
        case anchor_top_right:
            realposX = game_state.window.width + pos.x * game_state.window.height - sizeX;
            realposY = game_state.window.height + pos.y * game_state.window.height - sizeY;
            break;
    }
    u32 sizeposX = realposX + sizeX;
    u32 sizeposY = realposY + sizeY;
    if     (mouse_xpos > realposX &&
            mouse_ypos > realposY && 
            mouse_xpos < sizeposX &&
            mouse_ypos < sizeposY) {
        // printf("above %f %f \n", mouse_xpos, mouse_ypos);
        sprites[sprite_id].hovered = mouse_down ? 2 : 1;
        next_hovered = true;
        hovered_func = func;
    } else {
        sprites[sprite_id].hovered = false;
    }

    sprites[sprite_id].anchor = anchor;
    sprite2D_pos(sprite_id, pos);
    sprite2D_scale(sprite_id, scale);
    sprite2D_draw(sprite_id);

    render_btn_text(text, pos, scale / 200, 
                (f32_v3) {1, 1, 1}, 
                (f32_v2) {sprites[sprite_id].img->ratio  * scale,
                          scale}, 
                anchor);
}

void clear_hovered_btns(void) {
    if (something_hovered != next_hovered) {
        something_hovered = next_hovered;
    }
    next_hovered = false;
}

void click_btn(void) {
    if (something_hovered) {
        hovered_func();
    }
}
