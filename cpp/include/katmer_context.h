#ifndef KATMER_CONTEXT_H
#define KATMER_CONTEXT_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_context katmer_context;

KATMER_API const char* katmer_context_get(katmer_context* ctx, const char* key);
KATMER_API void katmer_context_set(katmer_context* ctx, const char* key, const char* value);
KATMER_API void katmer_context_signal(katmer_context* ctx, const char* code, const char* message, double score);
KATMER_API void katmer_context_output(katmer_context* ctx, const char* value);
KATMER_API const char* katmer_context_input_text(katmer_context* ctx);
KATMER_API const char* katmer_context_input_type(katmer_context* ctx);
KATMER_API void* katmer_context_alloc(katmer_context* ctx, size_t size, size_t alignment);
KATMER_API size_t katmer_context_arena_used(katmer_context* ctx);
KATMER_API size_t katmer_context_arena_remaining(katmer_context* ctx);
KATMER_API size_t katmer_context_signal_count(katmer_context* ctx);

#ifdef __cplusplus
}
#endif

#endif
