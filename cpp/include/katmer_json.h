#ifndef KATMER_JSON_H
#define KATMER_JSON_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

KATMER_API const char* katmer_json_escape_string(const char* value);
KATMER_API const char* katmer_json_quote(const char* value);
KATMER_API const char* katmer_json_pair(const char* key, const char* value);
KATMER_API int katmer_json_like_balanced(const char* value, size_t size);
KATMER_API size_t katmer_json_estimated_depth(const char* value, size_t size);

#ifdef __cplusplus
}
#endif

#endif
