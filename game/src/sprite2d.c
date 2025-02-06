#include "sprite2d.h"
#include "allocator.h"
#include "shader.h"
#include "window.h"

static f32_v2 camera = {0};

typedef struct {
    f32_v2 position;
    f32 scale;
    bool visible;
    sprite2D_anchor anchor;
    image_data *img;
} sprite2D_t;

static sprite2D_t *sprites;
static image_data *imgs;
static u32 max_size = 8;
static u32 actual_size = 0;

void sprite2D_init(void) {
    sprites = xcalloc(max_size, sizeof(sprite2D_t));
    imgs = xcalloc(max_size, sizeof(image_data));
}

void sprite2D_cleanup(void) {
    xfree(imgs);
    xfree(sprites);
}

void sprite2D_update_window_aspect(f32 aspect) {

}

u32 sprite2D_create(const char *name, u32 scene_id, sprite2D_anchor anchor) {
    u32 id = actual_size;
    actual_size++;
    sprites[id].position = (f32_v2) {0,0};
    sprites[id].visible = true;
    imgs[id] = load_image(name);
    sprites[id].img = &imgs[id];
    sprites[id].anchor = anchor;
    sprites[id].scale = 1;

    f32_v3 vertices[] = {
       { 1.0f, -0.0f, 0.0f},
       {-0.0f, -0.0f, 0.0f},
       { 1.0f,  1.0f, 0.0f},
       {-0.0f,  1.0f, 0.0f},
    };    

    u32 indices[] = {  // note that we start from 0!
        0, 3, 1,
        0, 2, 3,
    };  
    gfx_bind_vertices(shader_sprite2D, vertices, 4, indices, 6);

    f32_v2 texCoords[] = {
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f} 
    };
    gfx_bind_texture(shader_sprite2D, texCoords, 4);
    gfx_finalize_image(&imgs[id]);
    return id;
}

void sprite2D_visible(u32 id, bool visible) {
    sprites[id].visible = visible;
}

void sprite2D_pos(u32 id, f32_v2 pos) {
    sprites[id].position = pos;
}

void sprite2D_scale(u32 id, f32 scale) {
    sprites[id].scale = scale;
}

void sprite2D_camera_pos(f32_v2 pos) {
    camera = pos;
}

void sprite2D_draw(u32 i) {
    gfx_set_shader(shader_sprite2D);
    
    gfx_uniform_f32_v4(0, (f32_v4) {1, 0, 0.5, 1});
    f32_v2 pos = {0};
    if (!sprites[i].visible) return;
    pos.x = sprites[i].position.x - camera.x;
    pos.y = sprites[i].position.y - camera.y;
    auto img = sprites[i].img;
    gfx_uniform_f32_v2(1, pos);
    gfx_uniform_f32(2, window_aspect_ratio());
    gfx_uniform_f32(3, (f32) img->w / (f32) img->h);
    gfx_uniform_f32(4, sprites[i].scale);
    gfx_uniform_i8(5, sprites[i].anchor);
    gfx_activate_texture(0, img->texture);
    gfx_draw();
}
