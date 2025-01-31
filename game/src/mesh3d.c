#include "mesh3d.h"
#include "allocator.h"
#include "file_util.h"
#include "shader.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

static void tobj_file_reader_cb(void *ctx, const char *filename, int is_mtl, const char *obj_filename, char **buf, size_t *len) {
    (void)ctx;
    char *fixed_name;
    if (obj_filename != filename) {
        fixed_name = path_name("../res/models/", filename, "");
    } else {
        fixed_name = path_name("../res/models/", obj_filename, ".obj");
    }
    FILE *file = plt_fopen(fixed_name, "rb");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find model \"%s\"\n", filename);
        exit(1);
    }

    *buf = load_file_as_str(file, len); // TODO delete this? Unless it's already deleted, then manually untrack it.
}


mesh3d* load_model(const char *name) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    mesh3d* mesh = xmalloc(sizeof(mesh3d));
    *mesh = (mesh3d){0};

    u32 flags = TINYOBJ_FLAG_TRIANGULATE;
    i32 ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                &num_materials, name, tobj_file_reader_cb, NULL, flags);
    if (ret != TINYOBJ_SUCCESS) {
        printf("Failed to load OBJ file: %s\n", name);
        xfree(mesh);
        return NULL;
    }

    // Checker for if you forgot to triangulate your model at export!
    u32 shape_faces_num = 0;
    for (auto i = 0; i < num_shapes; i++) {
        shape_faces_num += shapes[i].length;
    }
    assert(shape_faces_num == attrib.num_face_num_verts);

    mesh->groups = xmalloc(sizeof(mesh_group_offset) * num_shapes);
    mesh->num_groups = num_shapes;

    // First pass: count unique vertices per group to allocate properly
    size_t total_vertices = 0;
    for (size_t i = 0; i < num_shapes; i++) {
        size_t group_start = shapes[i].face_offset * 3;
        size_t group_end = group_start + (shapes[i].length * 3);
        size_t unique_verts = 0;
        
        // Track which vertices we've seen
        bool *seen = xmalloc(sizeof(bool) * (shapes[i].length * 3));
        memset(seen, 0, sizeof(bool) * (shapes[i].length * 3));

        for (size_t f = group_start; f < group_end; f++) {
            tinyobj_vertex_index_t idx = attrib.faces[f];
            bool found = false;
            
            // Check if we've seen this vertex before
            for (size_t prev = group_start; prev < f; prev++) {
                tinyobj_vertex_index_t prev_idx = attrib.faces[prev];
                if (idx.v_idx == prev_idx.v_idx && 
                    idx.vn_idx == prev_idx.vn_idx && 
                    idx.vt_idx == prev_idx.vt_idx) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                unique_verts++;
            }
        }
        xfree(seen);
        
        mesh->groups[i].start_index = total_vertices;
        mesh->groups[i].num_vertices = unique_verts;
        mesh->groups[i].num_indices = shapes[i].length * 3;
        mesh->groups[i].offsetted_indices = xmalloc(sizeof(u32) * mesh->groups[i].num_indices);
        total_vertices += unique_verts;
    }

    // Allocate main arrays
    mesh->num_vertices = total_vertices;
    mesh->num_indices = attrib.num_face_num_verts * 3;
    mesh->vertices = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    mesh->indices = xmalloc(sizeof(u32) * mesh->num_indices);
    mesh->normals = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    mesh->uvs = xmalloc(sizeof(f32_v2) * mesh->num_vertices);

    // Set up group views into the main arrays
    for (size_t i = 0; i < num_shapes; i++) {
        mesh->groups[i].offsetted_vertices = &mesh->vertices[mesh->groups[i].start_index];
        mesh->groups[i].offsetted_normals = &mesh->normals[mesh->groups[i].start_index];
        mesh->groups[i].offsetted_uvs = &mesh->uvs[mesh->groups[i].start_index];
    }

    // Second pass: fill vertices and indices
    size_t vertex_index = 0;
    size_t index_index = 0;

    for (size_t i = 0; i < num_shapes; i++) {
        size_t group_start = shapes[i].face_offset * 3;
        size_t group_end = group_start + (shapes[i].length * 3);
        size_t group_vertex_start = mesh->groups[i].start_index;
        size_t group_vertex_index = 0;  // Start from 0 for each group's local indices
        
        // Map from face index to vertex index for this group
        u32 *vert_map = xmalloc(sizeof(u32) * (shapes[i].length * 3));
        
        for (size_t f = group_start; f < group_end; f++) {
            tinyobj_vertex_index_t idx = attrib.faces[f];
            u32 vert_idx = UINT32_MAX;
            
            // Check if we've seen this vertex before in this group
            for (size_t prev = group_start; prev < f; prev++) {
                tinyobj_vertex_index_t prev_idx = attrib.faces[prev];
                if (idx.v_idx == prev_idx.v_idx && 
                    idx.vn_idx == prev_idx.vn_idx && 
                    idx.vt_idx == prev_idx.vt_idx) {
                    vert_idx = vert_map[prev - group_start];
                    break;
                }
            }
            
            if (vert_idx == UINT32_MAX) {
                // New vertex
                vert_idx = group_vertex_index;
                vert_map[f - group_start] = vert_idx;
                
                // Add vertex data to main arrays
                if (idx.v_idx >= 0) {
                    mesh->vertices[group_vertex_start + group_vertex_index] = (f32_v3){
                        attrib.vertices[3 * idx.v_idx + 0],
                        attrib.vertices[3 * idx.v_idx + 1],
                        attrib.vertices[3 * idx.v_idx + 2]
                    };
                }
                if (idx.vn_idx >= 0) {
                    mesh->normals[group_vertex_start + group_vertex_index] = (f32_v3){
                        attrib.normals[3 * idx.vn_idx + 0],
                        attrib.normals[3 * idx.vn_idx + 1],
                        attrib.normals[3 * idx.vn_idx + 2]
                    };
                }
                if (idx.vt_idx >= 0) {
                    mesh->uvs[group_vertex_start + group_vertex_index] = (f32_v2){
                        attrib.texcoords[2 * idx.vt_idx + 0],
                        1.0f - attrib.texcoords[2 * idx.vt_idx + 1]
                    };
                } else {
                    mesh->uvs[group_vertex_start + group_vertex_index] = (f32_v2){0.0f, 0.0f};
                }
                
                group_vertex_index++;
            }
            
            // Set indices - global uses absolute index, group uses local index
            mesh->indices[index_index] = group_vertex_start + vert_idx;
            mesh->groups[i].offsetted_indices[f - group_start] = vert_idx;  // Local index starting from 0
            index_index++;
        }
        
        xfree(vert_map);
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return mesh;
}

void destroy_model(mesh3d *mesh) {
    if (mesh->groups) {
        for (u32 i = 0; i < mesh->num_groups; i++) {
            xfree(mesh->groups[i].offsetted_indices);
        }
        xfree(mesh->groups);
    }
    xfree(mesh->vertices);
    xfree(mesh->normals);
    xfree(mesh->uvs);
    xfree(mesh->indices);
    xfree(mesh);
}

void bind_model(mesh3d *mesh) {
    gfx_bind_vertices(shader_mesh3d, 
        mesh->vertices, 
        mesh->num_vertices, 
        mesh->indices,
        mesh->num_indices);
    gfx_bind_texture(shader_mesh3d, 
        mesh->uvs, 
        mesh->num_vertices);
}

void bind_model_group(mesh3d *mesh, u32 group_index) {
    gfx_bind_vertices(shader_mesh3d, 
        mesh->groups[group_index].offsetted_vertices, 
        mesh->groups[group_index].num_vertices, 
        mesh->groups[group_index].offsetted_indices,
        mesh->groups[group_index].num_indices);
    gfx_bind_texture(shader_mesh3d, 
        mesh->groups[group_index].offsetted_uvs, 
        mesh->groups[group_index].num_vertices);
}

void bind_model_from_group(mesh3d *mesh, u32 group_index) {
    gfx_bind_vertices(shader_mesh3d, 
        mesh->groups[group_index].offsetted_vertices, 
        mesh->num_vertices - mesh->groups[group_index].num_vertices, 
        mesh->groups[group_index].offsetted_indices,
        mesh->num_indices - mesh->groups[group_index].num_indices);
    gfx_bind_texture(shader_mesh3d, 
        mesh->groups[group_index].offsetted_uvs, 
        mesh->num_vertices - mesh->groups[group_index].num_vertices);
}

