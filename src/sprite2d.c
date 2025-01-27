#include "sprite2d.h"
#include "shader.h"



u32 sprite2D_create(const char *name, u32 scene_id) {
    u32 id = 0;

    image_data img = load_image(name);
    float vertices[] = {
        // positions         // colors
         1.0f, -0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.0f, -0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,    // top 
        -0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,    // top 
    };    

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,
        0, 3, 2,
    };  
    gfx_bind_vertices(shader_sprite2D, vertices, 24, indices, 6);

    float texCoords[] = {
        0.0f, 1.0f,  // lower-left corner  
        1.0f, 1.0f,  // lower-right corner
        0.0f, 0.0f,   // top-center corner
        1.0f, 0.0f   // top-center corner
    };
    gfx_bind_texture(shader_sprite2D, texCoords, 8);
    gfx_finalize_image(&img);
    return id;
}

void sprite2D_visible(u32 id, bool visible) {

}

void sprite2D_pos(u32 id, f32_v2 pos) {

}

void sprite2D_draw() {
    gfx_set_shader(shader_sprite2D);

    gfx_uniform_f32_v4(0, (f32_v4) {1, 0, 0.5, 1});
    gfx_uniform_f32_v2(1, (f32_v2) {.1, 0.2});
    gfx_uniform_f32(2, window_aspect_ratio());
    gfx_uniform_f32(3, (f32) img.w / (f32) img.h);
    gfx_activate_texture(0, img.texture);
    gfx_draw();
    gfx_uniform_f32_v2(1, (f32_v2) {.5, -.1});
    gfx_uniform_f32(3, (f32) img2.w / (f32) img2.h);
    gfx_activate_texture(0, img2.texture);
    gfx_draw();
}
