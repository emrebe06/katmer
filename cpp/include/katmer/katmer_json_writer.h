#ifndef KATMER_PUBLIC_JSON_WRITER_H
#define KATMER_PUBLIC_JSON_WRITER_H

#include "katmer_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_json_writer katmer_json_writer;

KATMER_API katmer_json_writer* katmer_json_writer_create(void);
KATMER_API void katmer_json_writer_destroy(katmer_json_writer* writer);
KATMER_API void katmer_json_writer_reset(katmer_json_writer* writer);
KATMER_API int katmer_json_begin_object(katmer_json_writer* writer);
KATMER_API int katmer_json_end_object(katmer_json_writer* writer);
KATMER_API int katmer_json_begin_array(katmer_json_writer* writer);
KATMER_API int katmer_json_end_array(katmer_json_writer* writer);
KATMER_API int katmer_json_key(katmer_json_writer* writer, const char* key);
KATMER_API int katmer_json_string(katmer_json_writer* writer, const char* value);
KATMER_API int katmer_json_number(katmer_json_writer* writer, double value);
KATMER_API int katmer_json_bool(katmer_json_writer* writer, int value);
KATMER_API int katmer_json_null(katmer_json_writer* writer);
KATMER_API const char* katmer_json_writer_cstr(katmer_json_writer* writer);

#ifdef __cplusplus
}
#endif

#endif
