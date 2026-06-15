#ifndef KATMER_PUBLIC_BUFFER_H
#define KATMER_PUBLIC_BUFFER_H

#include "katmer_allocator.h"
#include "katmer_string_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_buffer katmer_buffer;

KATMER_API katmer_buffer* katmer_buffer_create(size_t initial_capacity);
KATMER_API katmer_buffer* katmer_buffer_create_with_allocator(size_t initial_capacity, katmer_allocator allocator);
KATMER_API void katmer_buffer_destroy(katmer_buffer* buffer);
KATMER_API void katmer_buffer_clear(katmer_buffer* buffer);
KATMER_API int katmer_buffer_reserve(katmer_buffer* buffer, size_t capacity);
KATMER_API int katmer_buffer_write(katmer_buffer* buffer, const void* data, size_t size);
KATMER_API int katmer_buffer_write_cstr(katmer_buffer* buffer, const char* value);
KATMER_API int katmer_buffer_write_byte(katmer_buffer* buffer, unsigned char value);
KATMER_API size_t katmer_buffer_size(katmer_buffer* buffer);
KATMER_API size_t katmer_buffer_capacity(katmer_buffer* buffer);
KATMER_API const char* katmer_buffer_data(katmer_buffer* buffer);
KATMER_API katmer_string_view katmer_buffer_view(katmer_buffer* buffer);
KATMER_API char* katmer_buffer_to_cstr(katmer_buffer* buffer);
KATMER_API const char* katmer_buffer_stats_json(katmer_buffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
