#include "shader.h"
#include "allocator.h"
#include "file_util.h"
#include "nums.h"
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h> // hide opengl behind in here. In case we want to switch to vulkan later.
#include "game_state.h"

#define MAX_UNIFORMS 64
#define MAX_ATTRIBUTES 16

typedef struct {
    const char *name;
    u32 program;

    u32 vertex_buffer; // vbo
    u32 index_buffer; // ebo
    u32 vertex_attr_buffer; // vao
    u32 texture_coord_buffer;

    f32_v3 *vertices;
    u32 vertices_amount;
    u32 *indices;
    u32 indices_amount;
    f32_v2 *texture_coordinates;
    u32 texture_coordinates_amount;
} shader_data;

shader_data shaders[shader_len];
shader_types shader_current;

static void compile_shader(shader_types type) {
    int  success;
    char error_info[512];

    shader_data shader = shaders[type];
    switch (type) {
        case shader_sprite2D:
            shader.name = "sprite2D";
            break;
        case shader_mesh3d:
        case shader_tire_mesh3d:
            shader.name = "mesh3D";
            break;
        default:
            printf("Error: Unknown shader type %d\n", type);
            exit(1);
    }

    const char *vertex_file = load_vertex_shader(shader.name);
    const char *fragment_file = load_fragment_shader(shader.name);

    // Vertex shader
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_file, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, error_info);
        printf("Error! Could not compile vertex shader %s: %s\n", shader.name, error_info);
        goto fail;
    }

    // Fragment shader
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_file, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, error_info);
        printf("Error! Could not compile fragment shader %s: %s\n", shader.name, error_info);
        goto fail;
    }

    // Shader program
    shader.program = glCreateProgram();
    glAttachShader(shader.program, vertex_shader);
    glAttachShader(shader.program, fragment_shader);
    glLinkProgram(shader.program);

    glGetProgramiv(shader.program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader.program, 512, NULL, error_info);
        printf("Error! Could not link shader program %s: %s\n", shader.name, error_info);
        goto fail;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    // Buffer objects
    glGenBuffers(1, &shader.vertex_buffer);
    glGenVertexArrays(1, &shader.vertex_attr_buffer);
    glGenBuffers(1, &shader.index_buffer);
    glGenBuffers(1, &shader.texture_coord_buffer);

    shaders[type] = shader;

fail:
    xfree((void*)vertex_file);
    xfree((void*)fragment_file);
}

void gfx_init_shaders(void) {
    for (auto i = 0; i < shader_len; i++) {
        shaders[i] = (shader_data){0};
        compile_shader(i);
    }
}

void gfx_reload_shaders(void) {
    for (auto i = 0; i < shader_len; i++) {
        compile_shader(i);
        if (shaders[i].vertices_amount != 0) {
            gfx_bind_vertices(i, shaders[i].vertices, shaders[i].vertices_amount, shaders[i].indices, shaders[i].indices_amount);
        }
        if (shaders[i].texture_coordinates_amount != 0) {
            gfx_bind_texture(i, shaders[i].texture_coordinates, shaders[i].texture_coordinates_amount);
        }
    }
}

void gfx_cleanup_shaders(void) {
}

void gfx_set_shader(shader_types type) {
    shader_current = type;
    glUseProgram(shaders[type].program);
    glBindVertexArray(shaders[type].vertex_attr_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaders[type].index_buffer);
}

void gfx_bind_vertices(shader_types type, 
                       f32_v3 *vertices, 
                       u32 vertices_amount,
                       u32 *indices,
                       u32 indices_amount) {
    shaders[type].vertices = vertices;
    shaders[type].vertices_amount = vertices_amount;
    shaders[type].indices = indices;
    shaders[type].indices_amount = indices_amount;

    glBindVertexArray(shaders[type].vertex_attr_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, shaders[type].vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices_amount * sizeof(f32_v3), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaders[type].index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_amount * sizeof(f32), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32_v3), (void*)0);
    glEnableVertexAttribArray(0);  

    glBindVertexArray(0);
}

void gfx_bind_texture(shader_types type, f32_v2 *texture_coordinates, u32 amount) {
    shaders[type].texture_coordinates = texture_coordinates;
    shaders[type].texture_coordinates_amount = amount;
    glBindVertexArray(shaders[type].vertex_attr_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, shaders[type].texture_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(f32_v2), texture_coordinates, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(f32_v2), (void*)0);
    glEnableVertexAttribArray(1);

    // Unbind for safety
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void gfx_finalize_image(image_data* img) {
    glGenTextures(1, &img->texture);
    glBindTexture(GL_TEXTURE_2D, img->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    const float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->image);
    glGenerateMipmap(GL_TEXTURE_2D);

    xaligned_free(img->image);

    // Unbind texture for safety
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unbind the vertex array (if applicable in your context)
    glBindVertexArray(0);
}

void gfx_activate_texture(u32 texture_pipe, u32 texture) {
    glActiveTexture(GL_TEXTURE0 + texture_pipe);
    glBindTexture(GL_TEXTURE_2D, texture); // TODO support more textures
}

void gfx_uniform_f32_mat4x4(u32 location, f32_m4x4 mat) {
    glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

void gfx_uniform_f32_v4(u32 location, f32_v4 vec4) {
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void gfx_uniform_f32_v3(u32 location, f32_v3 vec4) {
    glUniform3f(location, vec4.x, vec4.y, vec4.z);
}

void gfx_uniform_f32_v2(u32 location, f32_v2 vec4) {
    glUniform2f(location, vec4.x, vec4.y);
}

void gfx_uniform_f32(u32 location, f32 num) {
    glUniform1f(location, num);
}

void gfx_uniform_i8(u32 location, i8 num) {
    glUniform1i(location, num);
}

void gfx_draw(void) {
    glDrawElements(game_state.render_method, shaders[shader_current].indices_amount, GL_UNSIGNED_INT, 0);
}
