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
    mesh->num_indices = attrib.num_face_num_verts * 3;
    mesh->indices = xmalloc(sizeof(u32) * mesh->num_indices);
    mesh->vertices = xmalloc(sizeof(f32_v3) * mesh->num_indices);
    mesh->normals = xmalloc(sizeof(f32_v3) * mesh->num_indices);
    mesh->uvs = xmalloc(sizeof(f32_v2) * mesh->num_indices);

    // First pass: count unique vertices per group to allocate properly
    size_t total_vertices = 0;
    for (size_t i = 0; i < num_shapes; i++) {
        size_t group_start = shapes[i].face_offset * 3;
        size_t group_end = group_start + (shapes[i].length * 3);
        size_t unique_verts = 0;
        size_t vertex_offset = total_vertices;  // Track where this group's vertices start
        
        // Set up group info
        if (i == 0) {
            mesh->groups[i].start_group_index = 0;
            mesh->groups[i].start_vertex_index = 0;
            mesh->groups[i].num_indices = mesh->num_indices;
            mesh->groups[i].offsetted_indices = mesh->indices;
        } else {
            mesh->groups[i].start_group_index = group_start;
            mesh->groups[i].start_vertex_index = vertex_offset;
            mesh->groups[i].num_indices = mesh->num_indices - group_start;
            mesh->groups[i].offsetted_indices = xmalloc(sizeof(u32) * mesh->groups[i].num_indices);
        }
        mesh->groups[i].num_indices_actual_size = shapes[i].length * 3;
        
        // Process vertices for this group
        for (size_t f = group_start; f < group_end; f++) {
            tinyobj_vertex_index_t idx = attrib.faces[f];
            
            // Check if vertex exists in this group
            bool found = false;
            for (size_t prev = group_start; prev < f; prev++) {
                tinyobj_vertex_index_t prev_idx = attrib.faces[prev];
                if (idx.v_idx == prev_idx.v_idx && 
                    idx.vn_idx == prev_idx.vn_idx && 
                    idx.vt_idx == prev_idx.vt_idx) {
                    mesh->indices[f] = mesh->indices[prev];
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                assert(idx.v_idx >= 0);
                assert(idx.vn_idx >= 0);
                assert(idx.vt_idx >= 0);
                
                // Add new vertex at the correct offset
                size_t vert_idx = vertex_offset + unique_verts;
                mesh->vertices[vert_idx] = (f32_v3){
                    attrib.vertices[3 * idx.v_idx + 0],
                    attrib.vertices[3 * idx.v_idx + 1],
                    attrib.vertices[3 * idx.v_idx + 2]
                };
                mesh->normals[vert_idx] = (f32_v3){
                    attrib.normals[3 * idx.vn_idx + 0],
                    attrib.normals[3 * idx.vn_idx + 1],
                    attrib.normals[3 * idx.vn_idx + 2]
                };
                mesh->uvs[vert_idx] = (f32_v2){
                    attrib.texcoords[2 * idx.vt_idx + 0],
                    1.0f - attrib.texcoords[2 * idx.vt_idx + 1]
                };
                mesh->indices[f] = vert_idx;
                unique_verts++;
            }

            for (size_t g = 1; g <= i; g++) {
                size_t local_index = f - mesh->groups[g].start_group_index;
                mesh->groups[g].offsetted_indices[local_index] = 
                    mesh->indices[f] - mesh->groups[g].start_vertex_index;
            }
        }

        mesh->groups[i].num_vertices = unique_verts;
        total_vertices += unique_verts;
    }

    // Resize vertex arrays to actual size
    mesh->num_vertices = total_vertices;
    mesh->vertices = xrealloc(mesh->vertices, sizeof(f32_v3) * mesh->num_vertices);
    mesh->normals = xrealloc(mesh->normals, sizeof(f32_v3) * mesh->num_vertices);
    mesh->uvs = xrealloc(mesh->uvs, sizeof(f32_v2) * mesh->num_vertices);

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
    gfx_bind_vertices(shader_current, 
        mesh->vertices, 
        mesh->num_vertices, 
        mesh->indices,
        mesh->num_indices);
    gfx_bind_texture(shader_current, 
        mesh->uvs, 
        mesh->num_vertices);
}

void bind_model_group(mesh3d *mesh, u32 group_index) {
    gfx_bind_vertices(shader_current, 
        mesh->vertices + mesh->groups[group_index].start_vertex_index, 
        mesh->groups[group_index].num_vertices, 
        mesh->groups[group_index].offsetted_indices,
        mesh->groups[group_index].num_indices_actual_size);
    gfx_bind_texture(shader_current, 
        mesh->uvs + mesh->groups[group_index].start_vertex_index, 
        mesh->groups[group_index].num_vertices);
}

void bind_model_from_group(mesh3d *mesh, u32 group_index) {
    gfx_bind_vertices(shader_current, 
        mesh->vertices + mesh->groups[group_index].start_vertex_index, 
        mesh->num_vertices - mesh->groups[group_index].start_vertex_index, 
        mesh->groups[group_index].offsetted_indices,
        mesh->groups[group_index].num_indices);
    gfx_bind_texture(shader_current, 
        mesh->uvs + mesh->groups[group_index].start_vertex_index, 
        mesh->num_vertices - mesh->groups[group_index].start_vertex_index);
}

