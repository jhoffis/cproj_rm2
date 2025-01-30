#pragma once
#include <stdlib.h>
#include "nums.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef DEBUG
#define MEM_TRACK_DBG
#endif

#ifdef MEM_TRACK_DBG 

void mem_tracker_init(void);
void mem_tracker_cleanup(void);
void print_num_mem_allocated(void);

void _priv_xregister_standard(void *ptr, size_t size, const char *file, int line);
void _priv_xregister_aligned(void *ptr, size_t size, const char *file, int line);
void *_priv_xmalloc(size_t size, const char *file, int line);
void *_priv_xaligned_alloc(size_t size, size_t alignment, const char *file, int line);
void *_priv_xcalloc(size_t nmemb, size_t size, const char *file, int line);
void *_priv_xrealloc(void *ptr, size_t size, const char *file, int line);
void *_priv_xrealloc_aligned(void *ptr, size_t size, size_t alignment, const char *file, int line);

#define xregister_standard(ptr, size) \
    _priv_xregister_standard((ptr), (size), __FILE__, __LINE__)
#define xregister_aligned(ptr, size) \
    _priv_xregister_aligned((ptr), (size), __FILE__, __LINE__)
#define xmalloc(size) \
    _priv_xmalloc((size), __FILE__, __LINE__)
#define xaligned_alloc(size, alignment) \
    _priv_xaligned_alloc((size), (alignment), __FILE__, __LINE__)
#define xcalloc(nmemb, size) \
    _priv_xcalloc((nmemb), (size), __FILE__, __LINE__)
#define xrealloc(ptr, size) \
    _priv_xrealloc((ptr), (size), __FILE__, __LINE__)
#define xrealloc_aligned(ptr, size, alignment) \
    _priv_xrealloc_aligned((ptr), (size), (alignment), __FILE__, __LINE__)

void _priv_xfree(void* ptr, const char *file, int line);
void _priv_xaligned_free(void* ptr, const char *file, int line);
#define xfree(ptr) \
    _priv_xfree((ptr), __FILE__, __LINE__)
#define xaligned_free(ptr) \
    _priv_xaligned_free((ptr), __FILE__, __LINE__)

#else
// Non-debug versions of malloc and free
static inline void mem_tracker_init(void) {}
static inline void mem_tracker_cleanup(void) {}
static inline void print_num_mem_allocated(void) {}
static inline void xregister_standard(void *ptr, size_t size) {}
static inline void xregister_aligned(void *ptr, size_t size) {}
#define xmalloc(size) malloc(size)
#define xcalloc(nmemb, size) calloc(nmemb, size)
#define xrealloc(ptr, size) realloc(ptr, size)
#ifdef _WIN32
#define xaligned_alloc(size, alignment) _aligned_malloc(size, alignment)
#define xrealloc_aligned(ptr, size, alignment) _aligned_realloc(ptr, size, alignment)
#define xaligned_free(ptr) _aligned_free(ptr)
#else
#define xaligned_alloc(size, alignment) aligned_alloc(alignment, size)
#define xrealloc_aligned(ptr, size, alignment) realloc(ptr, size)
#define xaligned_free(ptr) free(ptr)
#endif
#define xfree(ptr) free(ptr)
#endif


