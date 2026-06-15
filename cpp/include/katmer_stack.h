#ifndef KATMER_STACK_H
#define KATMER_STACK_H

#include <stddef.h>
#include "katmer_layer.h"
#include "katmer_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_stack katmer_stack;

KATMER_API katmer_stack* katmer_stack_create(katmer_runtime* runtime);
KATMER_API katmer_stack* katmer_stack_create_default(katmer_runtime* runtime);
KATMER_API void katmer_stack_destroy(katmer_stack* stack);
KATMER_API int katmer_stack_use(katmer_stack* stack, const char* layer_name, katmer_layer_fn fn);
KATMER_API size_t katmer_stack_layer_count(katmer_stack* stack);

KATMER_API katmer_result katmer_bake(
    katmer_stack* stack,
    const char* input_type,
    const void* input_data,
    size_t input_size
);

KATMER_API const char* katmer_bake_json(
    katmer_stack* stack,
    const char* input_type,
    const void* input_data,
    size_t input_size
);

#ifdef __cplusplus
}
#endif

#endif
