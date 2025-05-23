#include "input_handler.h"
#include "btn.h"
#include "game_state.h"
#include "renderer.h"
#include "selection_box.h"
#include "shader.h"
#include "window.h"
#include <stdio.h>


#ifdef DEBUG
bool exit_hotreload = false;
#endif

static bool mouse_reset = true;
static f32 movement_speed = 0.1f, mouse_sensitivity = 0.0025f;
static f32_v2 new_mouse_pos;
static f32_v2 last_mouse_pos;
f32_v2 mouse_world_pos = {0};
f32 move_forward, move_back, move_left, move_right, move_up, move_down;

void key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
    scenes_key_cb(key, scancode, action, mods);

#ifdef DEBUG
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_F1) {
            // console
        } else if (key == GLFW_KEY_G) {
            mouse_reset = true;
            game_state.free_cam = !game_state.free_cam;
            window_set_cursor_visible(!game_state.free_cam);
        } else if (key == GLFW_KEY_F5) {
            printf("Hot-reloading shaders...\n");
            gfx_reload_shaders();
        } else if (key == GLFW_KEY_F6) {
            printf("Hot-reloading program...\n");
            exit_hotreload = true;
            window_close();
        } else if (key == GLFW_KEY_X) {
            if (game_state.render_method != GFX_TRIANGLES) {
                game_state.render_method = GFX_TRIANGLES;
            } else {
                game_state.render_method = GFX_WIRE;
            }
        }
        switch (key) {
            case GLFW_KEY_E:
                move_forward += movement_speed;
                break;
            case GLFW_KEY_D:
                move_back -= movement_speed;
                break;
            case GLFW_KEY_S:
                move_left += movement_speed;
                break;
            case GLFW_KEY_F:
                move_right -= movement_speed;
                break;
            case GLFW_KEY_A:
                move_up -= movement_speed;
                break;
            case GLFW_KEY_B:
                move_down += movement_speed;
                break;
        }
    } else if (action == GLFW_PRESS) { 
        switch (key) {
            case GLFW_KEY_E:
                move_forward -= movement_speed;
                break;
            case GLFW_KEY_D:
                move_back += movement_speed;
                break;
            case GLFW_KEY_S:
                move_left -= movement_speed;
                break;
            case GLFW_KEY_F:
                move_right += movement_speed;
                break;
            case GLFW_KEY_A:
                move_up += movement_speed;
                break;
            case GLFW_KEY_B:
                move_down -= movement_speed;
                break;
        }
    }
#endif
}

void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods) {
    // printf("asdfff\n");
    if (game_state.free_cam) {
        new_mouse_pos.x = xpos;
        new_mouse_pos.y = ypos;

        f32 dx = 0, dy = 0;
        if (mouse_reset) {
            mouse_reset = false;
        } else {
            dx = (float) (new_mouse_pos.x - last_mouse_pos.x) * mouse_sensitivity * window_aspect_ratio();
            dy = (float) (new_mouse_pos.y - last_mouse_pos.y) * mouse_sensitivity;
        }

        last_mouse_pos.x = new_mouse_pos.x;
        last_mouse_pos.y = new_mouse_pos.y;

        game_state.cam3D.rot.x += dy;
        game_state.cam3D.rot.y = fmodf(game_state.cam3D.rot.y - dx, 360.f);

        return;
    }

    auto world_based = game_state.window.height; 
    mouse_world_pos.x = (xpos / world_based) + game_state.cam2D.pos.x;
    mouse_world_pos.y = 1. - (ypos / world_based) + game_state.cam2D.pos.y;
    
    if (action == GLFW_RELEASE) {
        click_btn();
        sel_box_stop_dragging(button == GLFW_MOUSE_BUTTON_LEFT);
    } else if (action == GLFW_PRESS) {
        sel_box_start_dragging(mouse_world_pos);
    } else {
        if (sel_box_is_visible()) {
            sel_box_drag(mouse_world_pos);
        }
    }
}

void resize_cb(u32 width, u32 height) {

}
