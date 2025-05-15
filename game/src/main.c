#include "audio.h"
#include "btn.h"
#include "input_handler.h"
#include "scenes.h"
#include "scenes/lobby_scene.h"
#include "scenes/main_scene.h"
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
#include "play/player.h"
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

    if (!audio_init()) {
        fprintf(stderr, "Failed to initialize audio\n");
        exit(1);
    }

    init_player();
    sprite2D_init();

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init();
    gfx_init_graphics();
    gfx_init_shaders();
    init_text();
    init_btns();

    auto sky = sprite2D_create("sky", 0);

    // sprite2D_camera_pos((f32_v2){0.7, 0.3});

    // Load model
    auto model_mesh = load_model("decentra");
    if (!model_mesh) {
        printf("Failed to load model\n");
        exit(1);
    }

    // Set up 3D mesh shader
    gfx_set_shader(shader_mesh3d);
    bind_model_group(model_mesh, 0);
    gfx_set_shader(shader_tire_mesh3d);
    // bind_model_from_to_group(model_mesh, 0, 1);
    bind_model_from_group(model_mesh, 1);
    // bind_model_group(model_mesh, 1);

    lobby_init();
    main_scene_init();
    // race_init(0);

    destroy_model(model_mesh);
    game_state.initialize = false;
    while (!window_should_close()) {
        gfx_clear_color(0.f, 0.f, 0.f, 1.0f);
        window_poll_events();

        render_print("delta %f", timer_delta());
        timer_update_delta();

        scenes_render();
        clear_hovered_btns();
        reset_debug_pos();
        gfx_swap();
    }

    // Cleanup
    sprite2D_cleanup();
    gfx_cleanup_shaders();
    window_cleanup();
    audio_close();
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
