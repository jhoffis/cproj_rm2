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

    mesh->num_vertices = attrib.num_face_num_verts;
    mesh->num_indices =attrib.num_face_num_verts * 3; 

    u32 index = 0;
    mesh->vertices = xmalloc(sizeof(f32_v3) * mesh->num_vertices);
    mesh->indices = xmalloc(sizeof(u32) * mesh->num_indices);
    mesh->normals = xmalloc(sizeof(f32_v3) * attrib.num_normals);
    mesh->uvs = xmalloc(sizeof(f32_v2) * attrib.num_texcoords);
    for (auto i = 0; i < num_shapes; i++) {
        auto shape = shapes[i];
        for (int n = 0; n < shape.length; n++) {
            auto f0 = attrib.faces[shape.face_offset + 3*n + 0];
            auto f1 = attrib.faces[shape.face_offset + 3*n + 1];
            auto f2 = attrib.faces[shape.face_offset + 3*n + 2];

            f32_v3 vertex0 = {
                attrib.vertices[3 * (size_t)f0.v_idx + 0],
                attrib.vertices[3 * (size_t)f0.v_idx + 1],
                attrib.vertices[3 * (size_t)f0.v_idx + 2]
            };
            f32_v3 vertex1 = {
                attrib.vertices[3 * (size_t)f1.v_idx + 0],
                attrib.vertices[3 * (size_t)f1.v_idx + 1],
                attrib.vertices[3 * (size_t)f1.v_idx + 2]
            };
            f32_v3 vertex2 = {
                attrib.vertices[3 * (size_t)f2.v_idx + 0],
                attrib.vertices[3 * (size_t)f2.v_idx + 1],
                attrib.vertices[3 * (size_t)f2.v_idx + 2]
            };

            // f32_v3 normal0 = {
            //     attrib.normals[3 * (size_t)f0.vn_idx + 0],
            //     attrib.normals[3 * (size_t)f0.vn_idx + 1],
            //     attrib.normals[3 * (size_t)f0.vn_idx + 2]
            // };
            // f32_v3 normal1 = {
            //     attrib.normals[3 * (size_t)f1.vn_idx + 0],
            //     attrib.normals[3 * (size_t)f1.vn_idx + 1],
            //     attrib.normals[3 * (size_t)f1.vn_idx + 2]
            // };
            // f32_v3 normal2 = {
            //     attrib.normals[3 * (size_t)f2.vn_idx + 0],
            //     attrib.normals[3 * (size_t)f2.vn_idx + 1],
            //     attrib.normals[3 * (size_t)f2.vn_idx + 2]
            // };

            f32_v2 tex0 = {
                attrib.texcoords[2 * (size_t)f0.vt_idx + 0],
                attrib.texcoords[2 * (size_t)f0.vt_idx + 1],
            };
            f32_v2 tex1 = {
                attrib.texcoords[2 * (size_t)f1.vt_idx + 0],
                attrib.texcoords[2 * (size_t)f1.vt_idx + 1],
            };
            f32_v2 tex2 = {
                attrib.texcoords[2 * (size_t)f2.vt_idx + 0],
                attrib.texcoords[2 * (size_t)f2.vt_idx + 1],
            };

            mesh->vertices[index]     = vertex0;
            mesh->vertices[index + 1] = vertex1;
            mesh->vertices[index + 2] = vertex2;
            mesh->uvs[index]     = tex0;
            mesh->uvs[index + 1] = tex1;
            mesh->uvs[index + 2] = tex2;
            mesh->indices[index]     = index;
            mesh->indices[index + 1] = index + 1;
            mesh->indices[index + 2] = index + 2;
            index += 3;
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
