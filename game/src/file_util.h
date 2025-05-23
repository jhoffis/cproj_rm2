#pragma once
#include "nums.h"
#include <stdio.h>

typedef struct {
    u16 w;
    u16 h;
    u16 pitch;
    f32 ratio;
    i32 comp;
    u32 texture;
    u8 *image;
} image_data;

FILE* plt_fopen(const char* filename, const char* mode);
int plt_sscanf(const char* str, const char* format, ...);
char *plt_strdup(const char *s);
char *load_file_as_str(FILE *file, size_t *out_size);
image_data load_image(const char *name);
char *load_vertex_shader(const char *name);
char *load_fragment_shader(const char *name);
char *load_geometry_shader(const char *name);
