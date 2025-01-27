#include "file_util.h"

#include <string.h>
#include "allocator.h"
#include "stb_image_impl.h"

#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR "\\"
#define FIX_PATH(path) path
#define PATH(x) x

static char exe_path[MAX_PATH] = {0};

static const char* get_exe_path() {
    if (!exe_path[0]) {
        GetModuleFileNameA(NULL, exe_path, MAX_PATH);
        // Remove executable name to get the directory
        char* last_slash = strrchr(exe_path, '\\');
        if (last_slash) {
            *(last_slash + 1) = '\0';
        }
    }
    return exe_path;
}
#else
#define PATH_SEPARATOR "/"
#define FIX_PATH(path) path
#define PATH(x) x
#endif

char* path_name(const char* prefix, const char *name, const char* suffix) {
    #ifdef _WIN32
    const char* base_path = get_exe_path();
    size_t base_len = strlen(base_path);
    #else
    const char* base_path = "";
    size_t base_len = 0;
    #endif
    
    size_t prefix_len = strlen(FIX_PATH(prefix));
    size_t name_len = strlen(FIX_PATH(name));
    size_t suffix_len = strlen(FIX_PATH(suffix));
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
    char *path = xmalloc(base_len + prefix_len + name_len + suffix_len + 1);
    if (!path) return NULL;

    // Construct the path
    snprintf(path, base_len + prefix_len + name_len + suffix_len + 1, "%s%s%s%s", 
            base_path, FIX_PATH(prefix), FIX_PATH(name), FIX_PATH(suffix));

    return path;
}

static char *load_file_as_str(FILE *file) {
    // Move to the end of the file to determine its size
    if (fseek(file, 0L, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        return NULL;
    }

    // Get the size of the file
    long fileSize = ftell(file);
    if (fileSize == -1) {
        perror("Error getting file size");
        return NULL;
    }

    // Allocate memory for the buffer
    char *buffer = xmalloc(fileSize + 1);  // +1 for null terminator
    if (buffer == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Move back to the start of the file
    if (fseek(file, 0L, SEEK_SET) != 0) {
        perror("Error seeking to start of file");
        free(buffer);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        perror("Error reading file");
        free(buffer);
        return NULL;
    }

    // Null-terminate the string
    buffer[fileSize] = '\0';

    return buffer;
}

image_data load_image(const char *name) {
    // for (auto i = 0; i < imgs_actual_size; i++) {
    //     if (imgs[i].h
    // }

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
    xregister_aligned(img.image, img.pitch * img.h);
    // void *textureAddr = xMmAllocateContiguousMemoryEx(img.pitch * img.h, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // swizzle_rect(img.image, img.w, img.h, textureAddr, img.pitch, 4);
    //
    // img.addr26bits = (u32) textureAddr & 0x03ffffff; // Retain the lower 26 bits of the address
    // free(img.image);
    // img.image = textureAddr;

    return img;
}

FILE *load_model_file(const char *name) {
    char *fixed_name = path_name("models/", name, ".obj");
    FILE *file = fopen(fixed_name, "rb");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find model \"%s\"\n", name);
        exit(1);
    }
    return file;
}

char *load_vertex_shader(const char *name) {
    char *fixed_name = path_name("shaders/", name, "_v.glsl");
    FILE *file = fopen(fixed_name, "r");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find shader \"%s\"\n", name);
        exit(1);
    }
    return load_file_as_str(file);
}

char *load_fragment_shader(const char *name) {
    char *fixed_name = path_name("shaders/", name, "_f.glsl");
    FILE *file = fopen(fixed_name, "r");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find shader \"%s\"\n", name);
        exit(1);
    }
    return load_file_as_str(file);
}





