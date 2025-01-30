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
        fixed_name = path_name("../models/", filename, "");
    } else {
        fixed_name = path_name("../models/", obj_filename, ".obj");
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

    // Process vertices
    mesh->num_vertices = attrib.num_vertices;
    mesh->vertices = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    for (u32 i = 0; i < mesh->num_vertices; i++) {
        mesh->vertices[i] = (f32_v3){
            attrib.vertices[3*i],
            attrib.vertices[3*i+1],
            attrib.vertices[3*i+2]
        };
    }

    // Process normals
    if (attrib.num_normals > 0) {
        mesh->normals = xmalloc(sizeof(f32_v3) * attrib.num_normals);
        for (u32 i = 0; i < attrib.num_normals; i++) {
            mesh->normals[i] = (f32_v3){
                attrib.normals[3*i],
                attrib.normals[3*i+1],
                attrib.normals[3*i+2]
            };
        }
    }

    // Process texture coordinates
    if (attrib.num_texcoords > 0) {
        mesh->uvs = xmalloc(sizeof(f32_v2) * attrib.num_texcoords);
        for (u32 i = 0; i < attrib.num_texcoords; i++) {
            mesh->uvs[i] = (f32_v2){
                attrib.texcoords[2*i],
                attrib.texcoords[2*i+1]
            };
        }
    }

    // Process indices
    mesh->num_indices = attrib.num_faces;
    mesh->indices = xmalloc(sizeof(u32) * mesh->num_indices);
    for (u32 i = 0; i < mesh->num_indices; i++) {
        mesh->indices[i] = attrib.faces[i].v_idx;
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return mesh;
}

void bind_model(mesh3d *mesh) {
    gfx_bind_vertices(shader_mesh3d, mesh->vertices, mesh->num_vertices, mesh->indices, mesh->num_indices);
    gfx_bind_texture(shader_mesh3d, mesh->uvs, mesh->num_vertices);
}
