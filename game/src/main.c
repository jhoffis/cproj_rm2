#include "input_handler.h"
#include "text.h"
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
    } else {
        game_state.initialize = true;
    }
    mem_tracker_init();

    init_text();

    init_player();
    sprite2D_init();

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init();
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
    game_state.initialize = false;
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
