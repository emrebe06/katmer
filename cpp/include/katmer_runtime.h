#ifndef KATMER_RUNTIME_H
#define KATMER_RUNTIME_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_runtime katmer_runtime;

KATMER_API katmer_runtime* katmer_runtime_create(void);
KATMER_API void katmer_runtime_destroy(katmer_runtime* runtime);
KATMER_API void katmer_runtime_set_arena_size(katmer_runtime* runtime, size_t bytes);
KATMER_API size_t katmer_runtime_bake_count(katmer_runtime* runtime);
KATMER_API const char* katmer_runtime_version(void);

#ifdef __cplusplus
}
#endif

#endif
