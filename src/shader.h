#pragma once
#include "nums.h"

#define MAX_UNIFORMS 64
#define MAX_ATTRIBUTES 16

typedef enum {
    shader_main,
    shader_len 
} shader_types;

typedef struct {
    u32 program;
    u32 vertex_shader;
    u32 fragment_shader;

    u32 vertex_buffer; // vbo
    u32 index_buffer; // ebo
    u32 vertex_attr_buffer; // vao

    u32 uniform_locations[MAX_UNIFORMS];    // Uniform locations
    u32 attribute_locations[MAX_ATTRIBUTES]; // Attribute locations
} shader;

void shaders_init(void);
void shaders_cleanup(void);
