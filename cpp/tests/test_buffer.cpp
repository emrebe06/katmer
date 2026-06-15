#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_buffer* buffer = katmer_buffer_create(4);
    assert(buffer);
    assert(katmer_buffer_write_cstr(buffer, "kat"));
    assert(katmer_buffer_write_cstr(buffer, "mer"));
    assert(katmer_buffer_size(buffer) == 6);
    assert(std::strcmp(katmer_buffer_data(buffer), "katmer") == 0);
    katmer_string_view view = katmer_buffer_view(buffer);
    assert(katmer_sv_contains(view, katmer_sv_from_cstr("tm")));
    char* copy = katmer_buffer_to_cstr(buffer);
    assert(std::strcmp(copy, "katmer") == 0);
    katmer_free_string(copy);
    katmer_buffer_destroy(buffer);
    return 0;
}
