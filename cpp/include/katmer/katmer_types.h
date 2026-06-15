#ifndef KATMER_PUBLIC_TYPES_H
#define KATMER_PUBLIC_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t katmer_u8;
typedef uint32_t katmer_u32;
typedef uint64_t katmer_u64;
typedef int32_t katmer_i32;
typedef int64_t katmer_i64;

typedef struct katmer_byte_view {
    const void* data;
    size_t size;
} katmer_byte_view;

#endif
