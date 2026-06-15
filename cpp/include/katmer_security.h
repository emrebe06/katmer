#ifndef KATMER_SECURITY_H
#define KATMER_SECURITY_H

#include <stddef.h>
#include "katmer_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum katmer_security_flag {
    KATMER_SECURITY_OK = 0,
    KATMER_SECURITY_SQL = 1u << 0,
    KATMER_SECURITY_COMMAND = 1u << 1,
    KATMER_SECURITY_SSRF = 1u << 2,
    KATMER_SECURITY_TRAVERSAL = 1u << 3,
    KATMER_SECURITY_SCRIPT = 1u << 4,
    KATMER_SECURITY_SECRET = 1u << 5,
    KATMER_SECURITY_OVERSIZED = 1u << 6,
    KATMER_SECURITY_CONTROL = 1u << 7,
    KATMER_SECURITY_MULTI_SIGNAL = 1u << 8
} katmer_security_flag;

KATMER_API unsigned int katmer_security_scan_flags(const char* input, size_t input_size, size_t max_size);
KATMER_API double katmer_security_score(unsigned int flags);
KATMER_API const char* katmer_security_flags_json(unsigned int flags);
KATMER_API const char* katmer_security_scan_json(const char* input, size_t input_size, size_t max_size);
KATMER_API const char* katmer_security_action(unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
