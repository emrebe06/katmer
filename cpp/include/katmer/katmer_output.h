#ifndef KATMER_PUBLIC_OUTPUT_H
#define KATMER_PUBLIC_OUTPUT_H

#include "katmer_string_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_output {
    int ok;
    katmer_string_view kind;
    katmer_string_view body;
} katmer_output;

KATMER_API katmer_output katmer_output_make(int ok, const char* kind, const char* body);
KATMER_API const char* katmer_output_json(katmer_output output);

#ifdef __cplusplus
}
#endif

#endif
