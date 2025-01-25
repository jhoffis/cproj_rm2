#include "file_util.h"

#include <string.h>
#include "allocator.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


char* path_name(const char* prefix, const char *name, const char* suffix) {
    size_t name_len = strlen(name);
    if (name_len > 32) return NULL;
    size_t suffix_len = strlen(suffix);
    if (suffix_len < 2) return NULL;

    for (int i = 0; i < name_len; i++) {
        if ((name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] >= 'a' && name[i] <= 'z') ||
            (name[i] >= '0' && name[i] <= '9') ||
            name[i] == '(' ||  
            name[i] == ')' ||  
            name[i] == '.' ||  
            name[i] == '\0' || 
            name[i] == ' '  || 
            name[i] == '_') {
            continue;
        }
        printf("Path-name \"%s\" is bad!\n", name);
        exit(1);
    }
    char *path = xmalloc(strlen(prefix) + name_len + strlen(suffix) + 1);
    if (!path) return NULL;

    // Construct the path
    snprintf(path, strlen(prefix) + name_len + strlen(suffix) + 1, "%s%s%s", prefix, name, suffix);

    return path;
}

image_data load_image(const char *name) {
    image_data img = {0};
    char *fixed_name = path_name("pics/", name, ".png");
    FILE* file = fopen(fixed_name, "rb"); // requires a persistent char* appearently
    xfree(fixed_name);
    if (!file) {
        printf("Could not find image \"%s\"\n", name);
        exit(1);
    }
    img.image = stbi_load_from_file(file, (int*) &img.w, (int*) &img.h, &img.comp, STBI_rgb_alpha);

    fclose(file);
    if (!img.image) {
        printf("Failed to load texture \"%s\"\n", name);
        exit(1);
    }
    img.pitch = 4*img.w; // 4 bytes
    // for (int i = 0; i < img.pitch * img.h; i += 4) {
        // Small endian flip from RGBA to ARGB (but backwards so memory-wise ABGR)
    //     u8 r = img.image[i];
    //     u8 b = img.image[i + 2];
    //     img.image[i] = b;
    //     img.image[i + 2] = r;
    // }

    // void *textureAddr = xMmAllocateContiguousMemoryEx(img.pitch * img.h, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // swizzle_rect(img.image, img.w, img.h, textureAddr, img.pitch, 4);
    //
    // img.addr26bits = (u32) textureAddr & 0x03ffffff; // Retain the lower 26 bits of the address
    // free(img.image);
    // img.image = textureAddr;

    return img;
}
