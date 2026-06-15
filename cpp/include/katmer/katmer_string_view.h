#ifndef KATMER_PUBLIC_STRING_VIEW_H
#define KATMER_PUBLIC_STRING_VIEW_H

#include "../katmer_result.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_string_view {
    const char* data;
    size_t size;
} katmer_string_view;

KATMER_API katmer_string_view katmer_sv_from_parts(const char* data, size_t size);
KATMER_API katmer_string_view katmer_sv_from_cstr(const char* value);
KATMER_API katmer_string_view katmer_sv_trim(katmer_string_view value);
KATMER_API int katmer_sv_equals(katmer_string_view left, katmer_string_view right);
KATMER_API int katmer_sv_contains(katmer_string_view value, katmer_string_view needle);
KATMER_API katmer_string_view katmer_sv_slice(katmer_string_view value, size_t start, size_t size);
KATMER_API const char* katmer_sv_json(katmer_string_view value);

#ifdef __cplusplus
}
#endif

#endif
