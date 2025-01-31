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

    mesh->num_vertices = attrib.num_face_num_verts * 3;
    mesh->num_indices = attrib.num_face_num_verts * 3;

    mesh->vertices = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    mesh->indices = xmalloc(sizeof(u32) * mesh->num_indices);
    mesh->normals = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    mesh->uvs = xmalloc(sizeof(f32_v2) * mesh->num_vertices);

    mesh->groups = xmalloc(sizeof(mesh_group_offset) * num_shapes);
    mesh->num_groups = num_shapes;
    size_t vertex_index = 0;

    // Process all faces
    for (size_t face = 0; face < attrib.num_face_num_verts; face++) {
        // Get the three vertices for this face (triangulated)
        for (size_t v = 0; v < 3; v++) {
            tinyobj_vertex_index_t idx = attrib.faces[face * 3 + v];
            
            // Vertex positions
            if (idx.v_idx >= 0) {
                mesh->vertices[vertex_index] = (f32_v3){
                    attrib.vertices[3 * idx.v_idx + 0],
                    attrib.vertices[3 * idx.v_idx + 1],
                    attrib.vertices[3 * idx.v_idx + 2]
                };
            }

            // Normals
            if (idx.vn_idx >= 0) {
                mesh->normals[vertex_index] = (f32_v3){
                    attrib.normals[3 * idx.vn_idx + 0],
                    attrib.normals[3 * idx.vn_idx + 1],
                    attrib.normals[3 * idx.vn_idx + 2]
                };
            }

            // Texture coordinates
            if (idx.vt_idx >= 0) {
                // Flip the Y coordinate since OBJ and OpenGL use different coordinate systems
                mesh->uvs[vertex_index] = (f32_v2){
                    attrib.texcoords[2 * idx.vt_idx + 0],
                    1.0f - attrib.texcoords[2 * idx.vt_idx + 1]  // Flip Y coordinate
                };
            } else {
                // Provide default UV coordinates if none exist
                mesh->uvs[vertex_index] = (f32_v2){0.0f, 0.0f};
            }

            // Set index
            mesh->indices[vertex_index] = vertex_index;
            vertex_index++;
        }
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return mesh;
}

void destroy_model(mesh3d *mesh) {
    xfree(mesh->vertices);
    xfree(mesh->normals);
    xfree(mesh->uvs);
    xfree(mesh->indices);
    xfree(mesh);
}

void bind_model(mesh3d *mesh) {
    gfx_bind_vertices(shader_mesh3d, mesh->vertices, mesh->num_vertices, mesh->indices, mesh->num_indices);
    gfx_bind_texture(shader_mesh3d, mesh->uvs, mesh->num_vertices);
}
