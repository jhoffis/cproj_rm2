#ifndef TEST_MODE
#include "game_state.h"
#include "sprite2d.h"
#include "renderer.h"
#include "shader.h"
#include "timer_util.h"
#include "window.h"
#include "file_util.h"
#include "allocator.h"
#include "player.h"
#include "nums.h"
#include <stdio.h>
#include "wav_loader.h"
#include "mesh3d.h"

#ifdef _WIN32
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API
#endif

#ifdef DEBUG
static bool exit_hotreload = false;
#endif


static bool mouse_reset = true;
static f32 movement_speed = 0.1f, mouse_sensitivity = 0.0025f;
static f32_v2 new_mouse_pos;
static f32_v2 last_mouse_pos;
static f32 move_forward, move_back, move_left, move_right, move_up, move_down;

static void key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
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

static void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods) {
    // printf("asdfff\n");
    if (!game_state.free_cam) return;
    
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

    game_state.cam_rot.x -= dy;
    game_state.cam_rot.y = fmodf(game_state.cam_rot.y - dx, 360.f);
}

static void resize_cb(u32 width, u32 height) {

}

#ifdef DEBUG
MODULE_API void *module_main(void *state) {
#elif defined(_WIN32)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    void *state = NULL;
#else
int main(void) {
    void *state = NULL;
#endif
    if (state != NULL) {
        memcpy(&game_state, state, sizeof(game_state_t));
    }
    game_state.test++;
    printf("gamestate test %d\n", game_state.test);
    mem_tracker_init();

    init_player();
    sprite2D_init();

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init(key_cb, mouse_cb, resize_cb);
    window_set_cursor_visible(!game_state.free_cam);
    gfx_init_graphics();
    gfx_init_shaders();

    auto sky = sprite2D_create("sky", 0, anchor_bottom_left);
    auto yinyang = sprite2D_create("yinyang", 0, anchor_bottom_left);

    sprite2D_pos(sky, (f32_v2){0.7, 0.3});
    sprite2D_camera_pos((f32_v2){0.7, 0.3});

    // Load model
    auto model_mesh = load_model("decentra");
    if (!model_mesh) {
        printf("Failed to load model\n");
        exit(1);
    }

    image_data img = load_image("DecentraPaint");
    gfx_finalize_image(&img);
    image_data img_tire = load_image("Tires");
    gfx_finalize_image(&img_tire);

    // Set up 3D mesh shader
    gfx_set_shader(shader_mesh3d);
    bind_model_group(model_mesh, 0);
    gfx_set_shader(shader_tire_mesh3d);
    // bind_model_from_to_group(model_mesh, 0, 1);
    bind_model_from_group(model_mesh, 1);
    // bind_model_group(model_mesh, 1);

    f32 pos = 0;

    destroy_model(model_mesh);
    while (!window_should_close()) {

        f32 x = sin(game_state.cam_rot.y);
        f32 z = cos(game_state.cam_rot.y);
        f32 moveX = move_left + move_right;
        f32 moveY = move_up + move_down;
        f32 moveZ = move_forward + move_back;
        // Forward and backwards + side to side
        game_state.cam_pos.x += (x * moveZ) + (z * moveX);
        game_state.cam_pos.z += (z * moveZ) - (x * moveX); 
        game_state.cam_pos.y += moveY; 

        f32_m4x4 mvp = {0};
        mat4x4_unit(mvp);
        mat4x4_translate(mvp, mvp, (f32_v4) {pos,0.5,-15.0,0}); 
        // pos -= 0.1;

        f32_m4x4 view = {0};
        create_world_view(view, game_state.cam_pos, game_state.cam_rot);

        f32_m4x4 persp = {0};
        mat4x4_perspective(persp, 70, window_aspect_ratio(), 1, 10000);
        // printf("%f\n", rot);
        mat4x4_multiply(mvp, mvp, view);
        mat4x4_multiply(mvp, mvp, persp);
        gfx_clear_color(0.2f, 0.3f, 0.3f, 1.0f);
        window_poll_events();

        // Draw 3D model
        gfx_set_shader(shader_mesh3d);
        gfx_activate_texture(0, img.texture); 
        gfx_uniform_f32_mat4x4(1, mvp);
        gfx_draw();

        gfx_set_shader(shader_tire_mesh3d);
        gfx_activate_texture(0, img_tire.texture); 
        gfx_uniform_f32_mat4x4(1, mvp);
        gfx_draw();

        // sprite2D_draw();
        gfx_swap();
    }

    // Cleanup
    
    sprite2D_cleanup();
    gfx_cleanup_shaders();
    window_cleanup();
    mem_tracker_cleanup();
#ifdef DEBUG
    if (exit_hotreload) {
        if (state != NULL) {
            free(state);
        }
        state = malloc(sizeof(game_state_t));
        memcpy(state, &game_state, sizeof(game_state_t));
        return state;
    }
    exit(0);
#else
    return 0;
#endif
}

#else
#include <stdio.h>
int main(void) {
    printf("Passed all tests!\n");
    return 0;
}
#endif // TEST_MODE
