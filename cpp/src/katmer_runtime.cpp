#include "katmer_internal.h"

katmer_runtime* katmer_runtime_create(void) {
    return new katmer_runtime();
}

void katmer_runtime_destroy(katmer_runtime* runtime) {
    delete runtime;
}

void katmer_runtime_set_arena_size(katmer_runtime* runtime, size_t bytes) {
    if (!runtime || bytes == 0) return;
    runtime->default_arena_size = bytes;
}

size_t katmer_runtime_bake_count(katmer_runtime* runtime) {
    return runtime ? runtime->bake_count : 0;
}

const char* katmer_runtime_version(void) {
    return "0.1.0";
}
