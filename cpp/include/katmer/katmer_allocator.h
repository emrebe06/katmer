#ifndef KATMER_PUBLIC_ALLOCATOR_H
#define KATMER_PUBLIC_ALLOCATOR_H

#include "../katmer_result.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*katmer_alloc_fn)(void* user_data, size_t size, size_t alignment);
typedef void* (*katmer_realloc_fn)(void* user_data, void* ptr, size_t old_size, size_t new_size, size_t alignment);
typedef void (*katmer_free_fn)(void* user_data, void* ptr, size_t size);

typedef struct katmer_allocator {
    void* user_data;
    katmer_alloc_fn alloc;
    katmer_realloc_fn realloc_fn;
    katmer_free_fn free_fn;
} katmer_allocator;

typedef enum katmer_allocator_platform {
    KATMER_ALLOCATOR_PLATFORM_WINDOWS = 1,
    KATMER_ALLOCATOR_PLATFORM_APPLE = 2,
    KATMER_ALLOCATOR_PLATFORM_ANDROID = 3,
    KATMER_ALLOCATOR_PLATFORM_LINUX = 4,
    KATMER_ALLOCATOR_PLATFORM_POSIX = 5,
    KATMER_ALLOCATOR_PLATFORM_UNKNOWN = 99
} katmer_allocator_platform;

KATMER_API katmer_allocator_platform katmer_allocator_current_platform(void);
KATMER_API const char* katmer_allocator_platform_name(katmer_allocator_platform platform);
KATMER_API size_t katmer_allocator_normalize_alignment(size_t alignment);
KATMER_API int katmer_allocator_alignment_is_valid(size_t alignment);
KATMER_API katmer_allocator katmer_allocator_heap(void);
KATMER_API void* katmer_allocator_alloc(katmer_allocator allocator, size_t size, size_t alignment);
KATMER_API void* katmer_allocator_realloc(katmer_allocator allocator, void* ptr, size_t old_size, size_t new_size, size_t alignment);
KATMER_API void katmer_allocator_free(katmer_allocator allocator, void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
