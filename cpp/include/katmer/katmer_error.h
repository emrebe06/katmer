#ifndef KATMER_PUBLIC_ERROR_H
#define KATMER_PUBLIC_ERROR_H

#include "../katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_error {
    katmer_status status;
    const char* code;
    const char* message;
    const char* detail;
} katmer_error;

KATMER_API katmer_error katmer_error_make(katmer_status status, const char* code, const char* message, const char* detail);
KATMER_API const char* katmer_error_json(katmer_error error);

#ifdef __cplusplus
}
#endif

#endif
