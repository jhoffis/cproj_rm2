#pragma once

#include "nums.h"

typedef struct {
    f32_v3 *positions;
    f32_v2 *texcoords;
    f32_v3 *normals;
    
    struct face_index {
        i32 pos_idx;
        i32 tex_idx;
        i32 norm_idx;
    } *faces;
    
    u32 position_count;
    u32 texcoord_count;
    u32 normal_count;
    u32 face_count;
} mesh_data;

// Loads OBJ file, returns NULL on failure
mesh_data *load_obj(const char* filename);

// Frees mesh resources
void free_mesh(mesh_data *mesh);

