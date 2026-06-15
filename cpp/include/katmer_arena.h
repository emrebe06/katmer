#ifndef KATMER_ARENA_H
#define KATMER_ARENA_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_arena katmer_arena;

typedef struct katmer_arena_stats {
    size_t capacity;
    size_t used;
    size_t remaining;
    size_t high_watermark;
    size_t allocations;
} katmer_arena_stats;

KATMER_API katmer_arena* katmer_arena_create(size_t capacity);
KATMER_API katmer_arena* katmer_arena_create_aligned(size_t capacity, size_t alignment);
KATMER_API void katmer_arena_destroy(katmer_arena* arena);
KATMER_API void katmer_arena_reset(katmer_arena* arena);
KATMER_API void* katmer_arena_alloc(katmer_arena* arena, size_t size, size_t alignment);
KATMER_API void* katmer_arena_alloc_zeroed(katmer_arena* arena, size_t size, size_t alignment);
KATMER_API char* katmer_arena_strdup(katmer_arena* arena, const char* value);
KATMER_API katmer_arena_stats katmer_arena_get_stats(katmer_arena* arena);
KATMER_API const char* katmer_arena_stats_json(katmer_arena_stats stats);

#ifdef __cplusplus
}
#endif

#endif
