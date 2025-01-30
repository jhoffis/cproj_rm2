#include <stdio.h>   // For stderr
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "game/src/str_util.h"
#include "game/src/allocator.h"
typedef void* module_main_func(void* data);

// This is where dlopen() and friends came from on UNIX OSes.
#ifdef _WIN32
#include <windows.h>
#include <direct.h>  // For _mkdir
#else
#include <dlfcn.h>
#include <sys/stat.h> // For mkdir
#endif
// Add this utility function before main()
static int create_directory(const char* path) {
#ifdef _WIN32
    auto path_copy = path_name("", path, "");
    int success = _mkdir(path_copy);
    xfree(path_copy);
    return success;
#else
    return mkdir(path, 0755);
#endif
}

int main(void){
    void* state = NULL;
    const char* build_dir = "module_build";
    const char* module_name = "my_module";

    for (;;) {
    mem_tracker_init();

    // Create build directory if it doesn't exist
    if (create_directory(build_dir) != 0 && errno != EEXIST) {
        perror("Failed to create build directory");
        return EXIT_FAILURE;
    }
#ifdef _WIN32
        char* build_path = path_name("", build_dir, "");
        char build_cmd[512];
        snprintf(build_cmd, sizeof(build_cmd),
            "cd \"%s\" && cmake -G \"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=%s ../.. && make",
            build_path, module_name);

        if (system(build_cmd) != 0) {
            fprintf(stderr, "Build failed\n");
            return EXIT_FAILURE;
        }

        size_t path_len = strlen(build_path);
        size_t name_len = strlen(module_name);
        size_t total_len = path_len + name_len + 10; // For "\", ".dll", and null terminator
        char *module_path = xmalloc(total_len);  // Ensure xmalloc properly allocates memory
        strcpy_s(module_path, total_len, build_path);
        strcat_s(module_path, total_len, "\\");
        strcat_s(module_path, total_len, module_name);
        strcat_s(module_path, total_len, ".dll");
        HMODULE module = LoadLibrary(module_path);
        xfree(module_path);
        xfree(build_path);
        if (!module) {
            fprintf(stderr, "Failed to load library: %lu\n", GetLastError());
            return EXIT_FAILURE;
        }
        
        module_main_func* module_main = (module_main_func*)(void*)GetProcAddress(module, "module_main");
        if (!module_main) {
            fprintf(stderr, "Failed to find module_main: %lu\n", GetLastError());
            FreeLibrary(module);
            return EXIT_FAILURE;
        }
        mem_tracker_cleanup();
        state = module_main(state);
        FreeLibrary(module);
#else
        // Modified Linux build commands to mirror Windows structure
        char build_cmd[512];
        snprintf(build_cmd, sizeof(build_cmd), 
            "cd %s && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOL=FALSE -DGAME_TITLE:STRING=%s ../.. && make --no-print-directory",
            build_dir, module_name);
        
        if (system(build_cmd) != 0) {
            fprintf(stderr, "Build failed\n");
            return EXIT_FAILURE;
        }

        char lib_path[512];
        snprintf(lib_path, sizeof(lib_path), "./%s/lib%s.so", build_dir, module_name);
        
        void* module = dlopen(lib_path, RTLD_NOW);
        if (!module) {
            fprintf(stderr, "Failed to load library: %s\n", dlerror());
            return EXIT_FAILURE;
        }
        
        module_main_func* module_main = (module_main_func*)dlsym(module, "module_main");
        if (!module_main) {
            fprintf(stderr, "Failed to find module_main: %s\n", dlerror());
            dlclose(module);
            return EXIT_FAILURE;
        }

        state = module_main(state);
        dlclose(module);
#endif
    }

    // Never actually gets here, use exit() to quit the program instead.
    return EXIT_SUCCESS;
}
