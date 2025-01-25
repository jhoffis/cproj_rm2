#pragma once
#include <stdlib.h>
#include "nums.h"

#define MEM_TRACK_DBG
#ifdef MEM_TRACK_DBG 
#include <stdio.h>
#include <string.h>

void mem_tracker_init(void);
void mem_tracker_cleanup(void);
void print_num_mem_allocated(void);

void *_priv_xmalloc(size_t size, const char *file, int line);
void *_priv_x_aligned_alloc(size_t size, size_t alignment, const char *file, int line);
void *_priv_xcalloc(size_t nmemb, size_t size, const char *file, int line);
void *_priv_xrealloc(void *ptr, size_t size, const char *file, int line);
#define xmalloc(size) \
    _priv_xmalloc((size), __FILE__, __LINE__)
#define x_aligned_alloc(size, alignment) \
    _priv_x_aligned_alloc((size), (alignment), __FILE__, __LINE__)
#define xcalloc(nmemb, size) \
    _priv_xcalloc((nmemb), (size), __FILE__, __LINE__)
#define xrealloc(ptr, size) \
    _priv_xrealloc((ptr), (size), __FILE__, __LINE__)

void _priv_xfree(void* ptr, const char *file, int line);
void _priv_xaligned_free(void* memblock, const char *file, int line);
#define xfree(ptr) \
    _priv_xfree((ptr), __FILE__, __LINE__)
#define xaligned_free(memblock) \
    _priv_xaligned_free((memblock), __FILE__, __LINE__)

#else
// Non-debug versions of malloc and free
static inline void mem_tracker_init(void) {}
static inline void mem_tracker_cleanup(void) {}
static inline void print_num_mem_allocated(void) {}
#define xmalloc(size) malloc(size)
#define x_aligned_alloc(size, alignment) aligned_alloc(size, alignment)
#define xcalloc(nmemb, size) calloc(nmemb, size)
#define xrealloc(ptr, size) realloc(ptr, size)
#define xfree(ptr) free(ptr)
#define xaligned_free(ptr) free(ptr)
#endif
