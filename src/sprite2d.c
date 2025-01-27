#include "sprite2d.h"
#include "allocator.h"
#include "shader.h"
#include "window.h"

static f32_v2 camera = {0};

static f32_v2 *positions;
static bool *visibles;
static image_data *imgs;
static sprite2D_anchor *anchors;
static u32 max_size = 8;
static u32 actual_size = 0;

void sprite2D_init(void) {
    positions = xcalloc(max_size, sizeof(f32_v2));
    visibles = xcalloc(max_size, sizeof(bool));
    imgs = xcalloc(max_size, sizeof(image_data));
    anchors = xcalloc(max_size, sizeof(sprite2D_anchor));
}

void sprite2D_cleanup(void) {
    xfree(anchors);
    xfree(imgs);
    xfree(visibles);
    xfree(positions);
}

void sprite2D_update_window_aspect(f32 aspect) {

}

u32 sprite2D_create(const char *name, u32 scene_id, sprite2D_anchor anchor) {
    u32 id = actual_size;
    actual_size++;
    positions[id] = (f32_v2) {0,0};
    visibles[id] = true;
    imgs[id] = load_image(name);
    anchors[id] = anchor;

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
    gfx_finalize_image(&imgs[id]);
    return id;
}

void sprite2D_visible(u32 id, bool visible) {
    visibles[id] = visibles;
}

void sprite2D_pos(u32 id, f32_v2 pos) {
    positions[id] = pos;
}

void sprite2D_camera_pos(f32_v2 pos) {
    camera = pos;
}

void sprite2D_draw() {
    gfx_set_shader(shader_sprite2D);
    
    gfx_uniform_f32_v4(0, (f32_v4) {1, 0, 0.5, 1});
    f32_v2 pos = {0};
    for (auto i = 0; i < actual_size; i++) {
        if (!visibles[i]) continue;
        pos.x = positions[i].x - camera.x;
        pos.y = positions[i].y - camera.y;
        gfx_uniform_f32_v2(1, pos);
        gfx_uniform_f32(2, window_aspect_ratio());
        gfx_uniform_f32(3, (f32) imgs[i].w / (f32) imgs[i].h);
        gfx_uniform_i8(4, anchors[i]);
        gfx_activate_texture(0, imgs[i].texture);
        gfx_draw();
    }
}
