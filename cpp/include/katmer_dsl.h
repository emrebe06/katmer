#ifndef KATMER_DSL_H
#define KATMER_DSL_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_dsl_document {
    int ok;
    const char* module;
    const char* type;
    const char* body;
    const char* error;
} katmer_dsl_document;

KATMER_API const char* katmer_dsl_parse_json(const char* input, size_t input_size);
KATMER_API int katmer_dsl_has_document(const char* input, size_t input_size);
KATMER_API const char* katmer_dsl_attr_json(const char* input, size_t input_size);

#ifdef __cplusplus
}
#endif

#endif
