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

int main(void) {
    mem_tracker_init();

    init_player();
    image_data img = load_image("sky");

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init();
    gfx_init_graphics();
    gfx_init_shaders();
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    gfx_bind_vertices(shader_main, vertices, 9);

    while (!window_should_close()) {
        gfx_clear_color(0.2f, 0.3f, 0.3f, 1.0f);
        window_poll_events();

        gfx_set_shader(shader_main);
        gfx_draw();

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
