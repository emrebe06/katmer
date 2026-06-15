#ifndef KATMER_PUBLIC_MODULE_H
#define KATMER_PUBLIC_MODULE_H

#include "../katmer_result.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_module_info {
    const char* name;
    const char* purpose;
    int builtin;
} katmer_module_info;

KATMER_API size_t katmer_module_count(void);
KATMER_API katmer_module_info katmer_module_get(size_t index);
KATMER_API const char* katmer_modules_json(void);

#ifdef __cplusplus
}
#endif

#endif
