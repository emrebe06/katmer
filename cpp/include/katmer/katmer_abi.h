#ifndef KATMER_PUBLIC_ABI_H
#define KATMER_PUBLIC_ABI_H

#include "../katmer_result.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KATMER_ABI_VERSION_MAJOR 0
#define KATMER_ABI_VERSION_MINOR 1
#define KATMER_ABI_VERSION_PATCH 0

typedef struct katmer_abi_info {
    int major;
    int minor;
    int patch;
    size_t pointer_size;
    const char* compiler;
    const char* build;
} katmer_abi_info;

KATMER_API katmer_abi_info katmer_abi_get_info(void);
KATMER_API const char* katmer_abi_info_json(void);
KATMER_API const char* katmer_abi_bake_json(const char* input_type, const void* input_data, size_t input_size);

#ifdef __cplusplus
}
#endif

#endif
