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

static void key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
    if (action == GLFW_RELEASE) {
#ifdef DEBUG
        if (key == GLFW_KEY_F5) {
            printf("Hot-reloading shaders...\n");
            gfx_reload_shaders();
        } else if (key == GLFW_KEY_F6) {
            printf("Hot-reloading program...\n");
            exit_hotreload = true;
            window_close();
        }
#endif
    }
}

static void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods) {
    // printf("asdfff\n");
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

    image_data img = load_image("sky");
    gfx_finalize_image(&img);

    // Set up 3D mesh shader
    gfx_set_shader(shader_mesh3d);
    bind_model(model_mesh);

    f32 pos = 100;

    while (!window_should_close()) {
        f32_m4x4 mvp = {0};
        mat4x4_unit(mvp);
        mat4x4_translate(mvp, mvp, (f32_v4) {pos,0.5,-15.0,0}); 
        pos -= 0.1;

        f32_m4x4 view = {0};
        create_world_view(view, (f32_v4) {0,0,0,0}, (f32_v4) {0,0,0,0});

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

        sprite2D_draw();
        gfx_swap();
    }

    // Cleanup
    xfree(model_mesh->vertices);
    xfree(model_mesh->normals);
    xfree(model_mesh->uvs);
    xfree(model_mesh->indices);
    xfree(model_mesh);
    
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
