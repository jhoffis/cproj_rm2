#pragma once
#include "file_util.h"
#include "nums.h"


typedef enum {
    shader_sprite2D,
    shader_len 
} shader_types;

void gfx_init_shaders(void);
void gfx_reload_shaders(void);
void gfx_cleanup_shaders(void);
void gfx_set_shader(shader_types type);
void gfx_bind_vertices(shader_types type, 
                       f32 *vertices, 
                       u32 vertices_amount,
                       u32 *indices,
                       u32 indices_amount);
void gfx_bind_texture(shader_types type,
                      f32 *texture_coordinates,
                      u32 amount);
void gfx_finalize_image(image_data *img);
void gfx_activate_texture(u32 texture_pipe, u32 texture);
void gfx_uniform_f32_v4(u32 location, f32_v4 vec4);
void gfx_uniform_f32_v3(u32 location, f32_v3 vec4);
void gfx_uniform_f32(u32 location, f32 num);
void gfx_uniform_i8(u32 location, i8 num);
void gfx_draw(void);
