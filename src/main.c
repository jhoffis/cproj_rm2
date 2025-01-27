#include "sprite2d.h"
#ifndef TEST_MODE

#include "renderer.h"
#include "shader.h"
#include "timer_util.h"
#include "window.h"
#include "file_util.h"
#include "allocator.h"
#include "model_loader.h"
#include "player.h"
#include "nums.h"
#include <stdio.h>
#include "wav_loader.h"

static void key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_F5) {
            printf("Hot-reloading shaders...\n");
            gfx_reload_shaders();
        }
    }
}

static void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods) {
    // printf("asdfff\n");
}

static void resize_cb(u32 width, u32 height) {

}

int main(void) {
    mem_tracker_init();

    init_player();

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init(key_cb, mouse_cb, resize_cb);
    gfx_init_graphics();
    gfx_init_shaders();

    sprite2D_create("sky");
    sprite2D_create("yinyang");

    while (!window_should_close()) {
        gfx_clear_color(0.2f, 0.3f, 0.3f, 1.0f);
        window_poll_events();

        sprite2D_draw();
        gfx_swap();
    }

    gfx_cleanup_shaders();
    window_cleanup();
    mem_tracker_cleanup();
    return 0;
}


#else
#include <stdio.h>
int main(void) {
    printf("Passed all tests!\n");
    return 0;
}
#endif // TEST_MODE
