#pragma once
#include "nums.h"
#include <stdio.h>

typedef struct {
    u16 w;
    u16 h;
    u16 pitch;
    i32 comp;
    u8 *image;
} image_data;

char* path_name(const char* prefix, const char *name, const char* suffix);
image_data load_image(const char *name);
FILE *load_model_file(const char *name);
