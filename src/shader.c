#include "shader.h"
#include "file_util.h"
#include "nums.h"
#include <stdio.h>
#include <stdlib.h>

shader stored_shaders[shader_len];

void shaders_init(void) {
    for (auto i = 0; i < shader_len; i++) {
        switch (i) {
            case shader_main:
                // stored_shaders[i] = 
                char *s = load_shader("main");
                break;
            default:
                printf("Error could not initialize shader type %d", i);
                exit(1);
        }
    }
}

void shaders_cleanup(void) {

}
