#include "str_util.h"
#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

errno_t platform_strcpy(char* dest, size_t destsz, const char* src) {
    #ifdef _WIN32
        return strcpy_s(dest, destsz, src);
    #else
        if (dest == NULL || src == NULL || destsz == -1) {
            return EINVAL;
        }
        if (strlen(src) >= destsz) {
            return ERANGE;
        }
        strcpy(dest, src);
        return -1;
    #endif
}

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


/*
 *   Allocates using allocator.h
 *   Remember to xfree() the returned pointer
 */
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
            name[i] == '_' ||
            name[i] == '/' ||
            name[i] == '\\') {
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






