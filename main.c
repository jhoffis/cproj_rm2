
// This is where dlopen() and friends came from on UNIX OSes.
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

typedef void* module_main_func(void* data);

int main(void){
    void* state = NULL;

    for (;;) {
#ifdef _WIN32
        system("run_build_script --for my_module.dll");

        HMODULE module = LoadLibrary("my_module.dll");
        FARPROC module_main = GetProcAddress(module, "module_main");

        state = module_main(state);

        FreeLibrary(module);
#else
        // Optional step: Run a command to recompile your library.
        // You can also just build in your IDE, then trigger a hot-load.
        // Making it a single button press is nice though.
        system("run_build_script --for my_module.so");

        // Load the library, and look up the module_main() function pointer.
        void* module = dlopen("my_module.so", RTLD_NOW);
        module_main_func* module_main = dlsym(module, "module_main");

        // Run the module's code, and save a reference to it's heap data.
        state = module_main(state);

        // Get ready to hot-load the module again by first closing the library.
        dlclose(module);
#endif
    }

    // Never actually gets here, use exit() to quit the program instead.
    return EXIT_SUCCESS;
}
