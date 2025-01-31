#pragma once
#include "nums.h"

typedef struct {
    u32 start_vertex_index;
    u32 start_group_index;
    u32 *offsetted_indices;
    u32 num_vertices;
    u32 num_indices;
    u32 num_indices_actual_size;
} mesh_group_offset;

typedef struct {
    f32_v3 *vertices;
    f32_v3 *normals;
    f32_v2 *uvs;
    u32 *indices;
    u32 num_vertices;
    u32 num_indices;
    
    // Track different groups in the mesh
    mesh_group_offset *groups;
    u32 num_groups;
} mesh3d;

mesh3d *load_model(const char *name);
void destroy_model(mesh3d *mesh);
void bind_model(mesh3d *mesh);
void bind_model_group(mesh3d *mesh, u32 group_index);
void bind_model_from_group(mesh3d *mesh, u32 group_index);
void bind_model_from_to_group(mesh3d *mesh, u32 group_from, u32 group_to);
