#include "model_loader.h"
#include "file_util.h"
#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

mesh_data* load_obj(const char* filename) {
    FILE* file = load_model_file(filename);
    if (!file) return NULL;

    mesh_data* mesh = xcalloc(1, sizeof(mesh_data));
    if (!mesh) {
        fclose(file);
        return NULL;
    }

    // Temporary storage for growing arrays
    f32_v3* tmp_pos = NULL;
    f32_v2* tmp_tex = NULL;
    f32_v3* tmp_norm = NULL;
    struct face_index* tmp_faces = NULL;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            if (line[1] == ' ') { // Vertex position
                f32_v3 v;
                if (sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
                    tmp_pos = xrealloc(tmp_pos, (mesh->position_count + 1) * sizeof(f32_v3));
                    tmp_pos[mesh->position_count++] = v;
                }
            }
            else if (line[1] == 't') { // Texture coordinate
                f32_v2 vt;
                if (sscanf(line, "vt %f %f", &vt.x, &vt.y) == 2) {
                    tmp_tex = xrealloc(tmp_tex, (mesh->texcoord_count + 1) * sizeof(f32_v2));
                    tmp_tex[mesh->texcoord_count++] = vt;
                }
            }
            else if (line[1] == 'n') { // Vertex normal
                f32_v3 vn;
                if (sscanf(line, "vn %f %f %f", &vn.x, &vn.y, &vn.z) == 3) {
                    tmp_norm = xrealloc(tmp_norm, (mesh->normal_count + 1) * sizeof(f32_v3));
                    tmp_norm[mesh->normal_count++] = vn;
                }
            }
        }
        else if (line[0] == 'f') { // Face
            struct face_index f[4];
            int count = 0;
            char* ptr = line + 2;
            
            while (*ptr && count < 4) {
                f[count].pos_idx = f[count].tex_idx = f[count].norm_idx = -1;
                sscanf(ptr, "%d/%d/%d", 
                    &f[count].pos_idx,
                    &f[count].tex_idx,
                    &f[count].norm_idx);
                
                // Handle negative indices (relative addressing)
                if (f[count].pos_idx < 0) f[count].pos_idx += mesh->position_count;
                if (f[count].tex_idx < 0) f[count].tex_idx += mesh->texcoord_count;
                if (f[count].norm_idx < 0) f[count].norm_idx += mesh->normal_count;
                
                // Convert to 0-based indices
                f[count].pos_idx--;
                f[count].tex_idx--;
                f[count].norm_idx--;

                while (*ptr && !isspace(*ptr)) ptr++;
                while (*ptr && isspace(*ptr)) ptr++;
                count++;
            }

            // Simple triangulation (quad -> 2 triangles)
            for (int i = 0; i < count - 2; i++) {
                tmp_faces = xrealloc(tmp_faces, (mesh->face_count + 3) * sizeof(struct face_index));
                tmp_faces[mesh->face_count++] = f[0];
                tmp_faces[mesh->face_count++] = f[i+1];
                tmp_faces[mesh->face_count++] = f[i+2];
            }
        }
    }

    fclose(file);

    // Transfer temporary arrays to mesh
    mesh->positions = tmp_pos;
    mesh->texcoords = tmp_tex;
    mesh->normals = tmp_norm;
    mesh->faces = tmp_faces;

    return mesh;
}

void free_mesh(mesh_data* mesh) {
    if (mesh) {
        xfree(mesh->positions);
        xfree(mesh->texcoords);
        xfree(mesh->normals);
        xfree(mesh->faces);
        xfree(mesh);
    }
} 