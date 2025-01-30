#pragma once
#include "nums.h"

typedef struct {
    f32_v3 *vertices;
    f32_v3 *normals;
    f32_v2 *uvs;
    u32 *indices;
    u32 num_vertices;
    u32 num_indices;
} mesh3d;

mesh3d *load_model(const char *name);
void destroy_model(mesh3d *mesh);
void bind_model(mesh3d *mesh);
