#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC  // Force static implementation visibility
#define STBI_NO_FLIP      // Disable vertical flip functionality
#define STBI_NO_IPHONE    // Disable iPhone orientation macros

#include "allocator.h"

#if defined(_WIN32)
    #define STBI_MALLOC(size)       _aligned_malloc(size, 16)
    #define STBI_REALLOC(ptr,size)  _aligned_realloc(ptr, size, 16)
    #define STBI_FREE(ptr)          _aligned_free(ptr)
#else
    #define STBI_MALLOC(size)       aligned_alloc(16, size)
    #define STBI_REALLOC(ptr,size)  realloc(ptr, size)
    #define STBI_FREE(ptr)          free(ptr)
#endif

#include "stb_image.h" 