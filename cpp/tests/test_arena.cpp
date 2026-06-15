#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_arena* arena = katmer_arena_create(256);
    assert(arena);
    void* a = katmer_arena_alloc(arena, 32, 8);
    char* b = katmer_arena_strdup(arena, "katmer");
    assert(a);
    assert(b);
    assert(std::strcmp(b, "katmer") == 0);
    katmer_arena_stats stats = katmer_arena_get_stats(arena);
    assert(stats.used > 0);
    katmer_arena_reset(arena);
    stats = katmer_arena_get_stats(arena);
    assert(stats.used == 0);
    katmer_arena_destroy(arena);
    return 0;
}
