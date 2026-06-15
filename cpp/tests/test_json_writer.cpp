#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_json_writer* writer = katmer_json_writer_create();
    assert(writer);
    assert(katmer_json_begin_object(writer));
    assert(katmer_json_key(writer, "ok"));
    assert(katmer_json_bool(writer, 1));
    assert(katmer_json_key(writer, "name"));
    assert(katmer_json_string(writer, "katmer"));
    assert(katmer_json_key(writer, "layers"));
    assert(katmer_json_begin_array(writer));
    assert(katmer_json_string(writer, "parse"));
    assert(katmer_json_string(writer, "guard"));
    assert(katmer_json_end_array(writer));
    assert(katmer_json_end_object(writer));
    const char* json = katmer_json_writer_cstr(writer);
    assert(std::strstr(json, "\"ok\":true"));
    assert(std::strstr(json, "\"layers\""));
    katmer_free_string(json);
    katmer_json_writer_destroy(writer);
    return 0;
}
