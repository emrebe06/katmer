#ifndef KATMER_PUBLIC_INPUT_H
#define KATMER_PUBLIC_INPUT_H

#include "katmer_string_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_input {
    katmer_string_view kind;
    katmer_string_view body;
    size_t bytes;
} katmer_input;

KATMER_API katmer_input katmer_input_make(const char* kind, const void* body, size_t bytes);
KATMER_API const char* katmer_input_json(katmer_input input);

#ifdef __cplusplus
}
#endif

#endif
