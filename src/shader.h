#pragma once
#include "nums.h"


typedef enum {
    shader_main,
    shader_len 
} shader_types;

void gfx_init_shaders(void);
void gfx_cleanup_shaders(void);
void gfx_set_shader(shader_types type);
void gfx_bind_vertices(shader_types type, 
                       f32 *vertices, 
                       u32 vertices_amount,
                       u32 *indices,
                       u32 indices_amount);
void gfx_draw(void);
