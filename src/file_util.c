#include "file_util.h"

#include <string.h>
#include "allocator.h"
#include "stb_image_impl.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
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
#endif

FILE* platform_fopen(const char* filename, const char* mode) {
    FILE* file = NULL;
    #ifdef _WIN32
        errno_t err = fopen_s(&file, filename, mode);
        if (err != 0) return NULL;
    #else
        file = fopen(filename, mode);
    #endif
    return file;
}

int platform_sscanf(const char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result;
    #ifdef _WIN32
        result = vsscanf_s(str, format, args);
    #else
        result = vsscanf(str, format, args);
    #endif
    va_end(args);
    return result;
}


char* path_name(const char* prefix, const char *name, const char* suffix) {
    #ifdef _WIN32
    const char* base_path = get_exe_path();
    size_t base_len = strlen(base_path);
    #else
    const char* base_path = "";
    size_t base_len = 0;
    #endif
    
    size_t prefix_len = strlen(prefix);
    size_t name_len = strlen(name);
    size_t suffix_len = strlen(suffix);

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
            base_path, prefix, name, suffix);

    #ifdef _WIN32
    // Replace forward slashes with backslashes on Windows
    for (size_t i = 0; i < base_len + prefix_len + name_len + suffix_len; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }
    #endif

    return path;
}

static char *load_file_as_str(FILE *file, size_t *out_size) {
    // Move to the end of the file to determine its size
    if (fseek(file, 0L, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        return NULL;
    }

    // Get the size of the file
    size_t file_size = ftell(file);
    if (file_size == -1) {
        perror("Error getting file size");
        return NULL;
    }

    // Allocate memory for the buffer
    char *buffer = xmalloc(file_size + 1);  // +1 for null terminator
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
    size_t bytesRead = fread(buffer, 1, file_size, file);
    if (bytesRead != file_size) {
        perror("Error reading file for loading string");
        free(buffer);
        return NULL;
    }

    // Null-terminate the string
    buffer[file_size] = '\0';
    if (out_size != NULL) {
        *out_size = file_size;
    }
    return buffer;
}

image_data load_image(const char *name) {
    // for (auto i = 0; i < imgs_actual_size; i++) {
    //     if (imgs[i].h
    // }

    image_data img = {0};
    char *fixed_name = path_name("pics/", name, ".png");
    FILE* file = platform_fopen(fixed_name, "rb");
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

static void tobj_file_reader_cb(void *ctx, const char *filename, int is_mtl, const char *obj_filename, char **buf, size_t *len) {
    (void)ctx;
    char *fixed_name;
    if (obj_filename != filename) {
        fixed_name = path_name("models/", filename, "");
    } else {
        fixed_name = path_name("models/", obj_filename, ".obj");
    }
    FILE *file = platform_fopen(fixed_name, "rb");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find model \"%s\"\n", filename);
        exit(1);
    }

    *buf = load_file_as_str(file, len);
}


void load_model_file(const char *name) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    {
        u32 flags = TINYOBJ_FLAG_TRIANGULATE;
        i32 ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                    &num_materials, name, tobj_file_reader_cb, NULL, flags);
        if (ret != TINYOBJ_SUCCESS) {
            printf("Failed to triangulate OBJ file: %s\n", name);
            return;
        }
    }
}

char *load_vertex_shader(const char *name) {
    char *fixed_name = path_name("shaders/", name, "_v.glsl");
    FILE *file = platform_fopen(fixed_name, "rb");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find shader \"%s\"\n", name);
        exit(1);
    }
    return load_file_as_str(file, NULL);
}

char *load_fragment_shader(const char *name) {
    char *fixed_name = path_name("shaders/", name, "_f.glsl");
    FILE *file = platform_fopen(fixed_name, "rb");
    xfree(fixed_name);
    if (!file) {
        printf("Could not find shader \"%s\"\n", name);
        exit(1);
    }
    return load_file_as_str(file, NULL);
}

