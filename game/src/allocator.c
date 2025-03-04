#include "allocator.h"
// #include <threads.h>

#ifdef MEM_TRACK_DBG 
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>
#include "str_util.h"

#ifdef _WIN32
#include <malloc.h>
#endif

typedef enum {
    none, standard, aligned,
} free_method;

static size_t num_allocations = 0;
static size_t num_real_mem = 0;
static size_t num_dbg_tracking_mem = 0;

static size_t num_record_real_mem = 0;
static char* record_name = NULL;
static size_t record_name_size = 0;

static size_t num_max_size;
static void** alloc_ptrs = NULL;
static char** alloc_names = NULL;
static size_t* alloc_sizes = NULL;
static free_method* alloc_method = NULL;

static bool realloc_allocation_tracker(size_t size) {
    num_max_size = size;
    const size_t old_ptr_size = num_allocations * sizeof(void*);
    const size_t old_names_size = num_allocations * sizeof(char*);
    const size_t old_size_size = num_allocations * sizeof(size_t);
    const size_t old_method_size = num_allocations * sizeof(free_method);
    const size_t new_ptr_size = num_max_size * sizeof(void*);
    const size_t new_names_size = num_max_size * sizeof(char*);
    const size_t new_size_size = num_max_size * sizeof(size_t);
    const size_t new_method_size = num_max_size * sizeof(free_method);

    alloc_ptrs = realloc(alloc_ptrs, new_ptr_size);
    alloc_names = realloc(alloc_names, new_names_size);
    alloc_sizes = realloc(alloc_sizes, new_size_size);
    alloc_method = realloc(alloc_method, new_method_size);

    if (alloc_ptrs == NULL || alloc_names == NULL || alloc_sizes == NULL || alloc_method == NULL) {
        return false;
    }

    num_dbg_tracking_mem += (new_ptr_size     - old_ptr_size)   + 
                            (new_names_size   - old_names_size) + 
                            (new_size_size    - old_size_size)  + 
                            (new_method_size  - old_method_size);
    return true;
}

void mem_tracker_init(void) {
    if (num_max_size > 0) return;
    if (!realloc_allocation_tracker(16)) goto fail;

    record_name_size = 256;  // reasonable initial size
    record_name = malloc(record_name_size);
    if (!record_name) goto fail;
    
    return;
fail:
    printf("Failed to initialize memory tracker\n");
    exit(1);
}

// Function to print how much memory is currently allocated
void print_num_mem_allocated(void) {
    if (num_real_mem > 100000) {
        printf("Mem: %zu kb (n=%zu, r=%zu kb)\n", num_real_mem / 1000, num_allocations, num_record_real_mem / 1000);
    } else {
        printf("Mem: %zu b (n=%zu, r=%zu b)\n", num_real_mem, num_allocations, num_record_real_mem);
    }
    // for (int i = 0; i < num_allocations; i++) {
    //     pb_print("%zu b: %s\n", alloc_sizes[i], alloc_names[i]);
    // }
    // pb_print("Mem record at %s\n", record_name);
    // pb_print("Total tracking memory used: %zu bytes\n", num_dbg_tracking_mem);
    // pb_print("Total memory used (user + tracking): %zu bytes\n", num_real_mem + num_dbg_tracking_mem);
}

static void debug_print_all_allocated(size_t size, const char *file, int line) {
    printf("Attempted to allocate %zu b, which more than the max!\nFile: %s, Line: %d\n", size, file, line);
    for (int i = 0; i < num_allocations; i++) {
        printf("%zu b: %s\n", alloc_sizes[i], alloc_names[i]);
    }
    exit(1);
}

/*
 * ==================================
 * ============ TRACKING ============  
 * ==================================
 */

static int upsize_allocation_tracker(void) {
    if (num_allocations < num_max_size) return 1;
    return realloc_allocation_tracker(num_max_size * 2);
}

static bool track_allocation(void* ptr, size_t size, free_method method, const char *file, int line) {
    if (!ptr) {
        printf("Failed to allocate pointer!\nFile: %s, Line: %d\n", file, line);
        exit(1);
    }

    num_allocations++;
    if (!upsize_allocation_tracker()) {
        printf("Failed to upsize allocation tracker!\nFile: %s, Line: %d\n", file, line);
        exit(1);
    }

    // Find the last '/' in the file path
    const char* last_slash = strrchr(file, '/');
    const char* filename = last_slash ? last_slash + 1 : file; // If no slash, use entire string

    size_t name_size = strlen(filename) + 50;
    char *name = malloc(name_size);
    if (!name) {
        printf("Failed to allocate name!\nFile: %s, Line: %d\n", file, line);
        exit(1);
    }

    // Use only the filename in the string format
    snprintf(name, name_size, "File: %s, Line: %d", filename, line);

    alloc_ptrs[num_allocations - 1] = ptr;
    alloc_names[num_allocations - 1] = name;
    alloc_sizes[num_allocations - 1] = size;
    alloc_method[num_allocations - 1] = method;

    num_real_mem += size;
    if (num_record_real_mem < num_real_mem) {
        num_record_real_mem = num_real_mem;

        // Ensure record_name buffer is large enough
        if (name_size > record_name_size) {
            char* new_record = realloc(record_name, name_size);
            if (new_record) {
                record_name = new_record;
                record_name_size = name_size;
            } else {
                // If realloc fails, keep old buffer and truncate
                snprintf(record_name, record_name_size, "%s", name);
                return true;  // Still return success as main allocation worked
            }
        }

        plt_strncpy_s(record_name, record_name_size, name, record_name_size - 1);
        record_name[record_name_size - 1] = '\0';
    }

    return true;
}

static void untrack_and_free(void* ptr, free_method method, const char *file, int line) {
    if (ptr == NULL) return;

    bool found = false;
    size_t i = 0;
    for (; i < num_allocations; i++) {
        if (alloc_ptrs[i] == ptr) {
            found = true;
            break;
        }
    }

    if (!found) {
        printf("Attempted to free unmanaged memory or free more than once!\nFile: %s, Line: %d\n", file, line);
        exit(1);
    }

    if (method != none && method != alloc_method[i] ) {
        printf("Attempted to free memory with the wrong method!\nFile: %s, Line: %d\n", file, line);
        exit(1);
    }

    switch (method) {
        case none:
            break;
        case standard:
            free(ptr);
            break;
        case aligned:
            #ifdef _WIN32
                _aligned_free(ptr);
            #else
                free(ptr);
            #endif
            break;
    }

    // Remove the allocation from the tracker
    free(alloc_names[i]);
    num_real_mem -= alloc_sizes[i];

    // Shift remaining elements in the tracker to fill the gap
    for (size_t j = i; j < num_allocations - 1; j++) {
        alloc_ptrs[j] = alloc_ptrs[j + 1];
        alloc_names[j] = alloc_names[j + 1];
        alloc_sizes[j] = alloc_sizes[j + 1];
        alloc_method[j] = alloc_method[j + 1];
    }

    num_allocations--;
}


/*
 * =====================================
 * ============ ALLOCATIONS ============  
 * =====================================
 */

void _priv_xregister_standard(void *ptr, size_t size, const char *file, int line) {
    track_allocation(ptr, size, standard, file, line);
}

void _priv_xregister_aligned(void *ptr, size_t size, const char *file, int line) {
    track_allocation(ptr, size, aligned, file, line);
}

void *_priv_xmalloc(size_t size, const char *file, int line) {
    if (size + num_real_mem >= MAX_MEM_64) {
        debug_print_all_allocated(size, file, line);
    }
    void* ptr = malloc(size);
    if (!track_allocation(ptr, size, standard, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_xaligned_alloc(size_t size, size_t alignment, const char *file, int line) {
    if (size + num_real_mem >= MAX_MEM_64) {
        debug_print_all_allocated(size, file, line);
    }
    #ifdef _WIN32
        void* ptr = _aligned_malloc(size, alignment);
    #else
        void* ptr = aligned_alloc(alignment, size);
    #endif
    if (!track_allocation(ptr, size, aligned, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_xcalloc(size_t nmemb, size_t size, const char *file, int line) {
    if (nmemb*size + num_real_mem >= MAX_MEM_64) {
        debug_print_all_allocated(nmemb*size, file, line);
    }
    void* ptr = calloc(nmemb, size);
    size_t total_size = nmemb * size;
    if (!track_allocation(ptr, total_size, standard, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_xrealloc(void *old_ptr, size_t size, const char *file, int line) {
    size_t old_size = 0;
    for (int i = 0; i < num_allocations; i++) {
        if (alloc_ptrs[i] == old_ptr) {
            old_size = alloc_sizes[i];
            break;
        }
    }
    if (size - old_size + num_real_mem >= MAX_MEM_64) {
        debug_print_all_allocated(size - old_size, file, line);
    }

    // Attempt reallocation first
    void *new_ptr = realloc(old_ptr, size);
    if (!new_ptr) {
        // Realloc failed; retain the old pointer
        return NULL;
    }

    // Reallocation successful; track new allocation
    if (!track_allocation(new_ptr, size, standard, file, line)) {
        free(new_ptr);  // Free newly allocated memory if tracking fails
        return NULL;
    }

    // Untrack old allocation only after new allocation is successful
    if (old_ptr) {
        untrack_and_free(old_ptr, none, file, line);
    }

    return new_ptr;
}

void *_priv_xrealloc_aligned(void *old_ptr, size_t size, size_t alignment, const char *file, int line) {
    // Find old size for tracking
    size_t old_size = 0;
    for (int i = 0; i < num_allocations; i++) {
        if (alloc_ptrs[i] == old_ptr) {
            old_size = alloc_sizes[i];
            break;
        }
    }

    if (size - old_size + num_real_mem >= MAX_MEM_64) {
        debug_print_all_allocated(size - old_size, file, line);
    }

#ifdef _WIN32
    void *new_ptr = _aligned_realloc(old_ptr, size, alignment);
#else
    void *new_ptr = realloc(old_ptr, size);
#endif
    if (!new_ptr) {
        // Realloc failed; retain the old pointer
        return NULL;
    }

    // Reallocation successful; track new allocation
    if (!track_allocation(new_ptr, size, standard, file, line)) {
        free(new_ptr);  // Free newly allocated memory if tracking fails
        return NULL;
    }

    // Untrack old allocation only after new allocation is successful
    if (old_ptr) {
        untrack_and_free(old_ptr, none, file, line);
    }
    return new_ptr;
}

/*
 * =====================================
 * ============ DESTRUCTION ============  
 * =====================================
 */
void _priv_xfree(void* ptr, const char *file, int line) {
    untrack_and_free(ptr, standard, file, line);
}

void _priv_xaligned_free(void* ptr, const char *file, int line) {
    untrack_and_free(ptr, aligned, file, line);
}

void mem_tracker_cleanup(void) {
    if (num_allocations > 0) {
        printf("Not all memory is freed!\n");
        for (int i = 0; i < num_allocations; i++) {
            printf("%s\n", alloc_names[i]);
        }

        // add sleep and then delete so we can test faster!
        // thrd_sleep(&(struct timespec){.tv_sec=1}, NULL);
        for (int i = 0; i < num_allocations; i++) {
            untrack_and_free(alloc_ptrs[i], alloc_method[i], "cleanup", 0);
        }
        return;
    }
    free(alloc_method);
    free(alloc_sizes);
    for (int i = 0; i < num_allocations; i++) {
        free(alloc_names[i]);
    }
    free(alloc_names);
    free(alloc_ptrs);
    free(record_name);

    num_max_size = 0;
    num_allocations = 0;
    num_real_mem = 0;
    num_dbg_tracking_mem = 0;
    num_record_real_mem = 0;
    record_name = NULL;
    record_name_size = 0;
    alloc_ptrs = NULL;
    alloc_names = NULL;
    alloc_sizes = NULL;
    alloc_method = NULL;
}

TEST(test_allocator) {
    mem_tracker_init();
    void *m = xmalloc(1);
    void *c = xcalloc(1, 1);
    void *r1 = xrealloc(m, 2);
    void *r2 = xrealloc(c, 2);
    void *a = xaligned_alloc(1, 16);
    void *ra1 = xrealloc_aligned(a, 2, 16);
    ASSERT(num_allocations == 3, "should be 3 allocated");

    xfree(m);
    xfree(c);
    xfree(a);
    ASSERT(num_allocations == 0, "should be all freed");
}

#endif
