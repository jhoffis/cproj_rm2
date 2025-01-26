#include "shader.h"
#include "allocator.h"
#include "file_util.h"
#include "nums.h"
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h> // hide opengl behind in here. In case we want to switch to vulkan later.

#define MAX_UNIFORMS 64
#define MAX_ATTRIBUTES 16

typedef struct {
    const char *name;
    u32 program;

    u32 vertex_buffer; // vbo
    u32 index_buffer; // ebo
    u32 vertex_attr_buffer; // vao

    u32 uniform_locations[MAX_UNIFORMS];    // Uniform locations
    u32 attribute_locations[MAX_ATTRIBUTES]; // Attribute locations
} shader_data;

shader_data shaders[shader_len];
shader_types curr;

static void compile_shader(shader_types type) {
    int  success;
    char error_info[512];

    shader_data shader = {0};
    switch (type) {
        case shader_main:
            shader.name = "main";
            break;
        default:
            printf("Error could not initialize shader type %d", type);
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

    shaders[type] = shader;

fail:
    xfree((void*)vertex_file);
    xfree((void*)fragment_file);
}

void gfx_init_shaders(void) {
    for (auto i = 0; i < shader_len; i++) {
        compile_shader(i);
    }
}

void gfx_cleanup_shaders(void) {
}

void gfx_set_shader(shader_types type) {
    curr = type;
    glUseProgram(shaders[type].program);
    glBindVertexArray(shaders[type].vertex_attr_buffer);
}

void gfx_bind_vertices(shader_types type, f32 *vertices, u32 amount) {

    glBindVertexArray(shaders[type].vertex_attr_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, shaders[type].vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(f32), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);  
}

void gfx_draw(void) {
    glDrawArrays(GL_TRIANGLES, 0, 3);
}



