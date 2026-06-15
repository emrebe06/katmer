#include "katmer_internal.h"
#include "katmer_arena.h"
#include "katmer/katmer_allocator.h"

struct katmer_arena {
    unsigned char* data = nullptr;
    size_t capacity = 0;
    size_t reserved = 0;
    size_t offset = 0;
    size_t high_watermark = 0;
    size_t allocations = 0;
    size_t alignment = alignof(void*);
    katmer_allocator allocator = katmer_allocator_heap();
};

namespace {
size_t normalize_alignment(size_t alignment) {
    if (alignment == 0) return alignof(void*);
    if ((alignment & (alignment - 1)) != 0) {
        size_t power = 1;
        while (power < alignment) power <<= 1;
        return power;
    }
    return alignment;
}

void* arena_alloc_internal(katmer_arena* arena, size_t size, size_t alignment, bool zeroed) {
    if (!arena || !arena->data || size == 0) return nullptr;
    alignment = katmer_allocator_normalize_alignment(alignment);
    size_t base = reinterpret_cast<size_t>(arena->data);
    size_t current = base + arena->offset;
    size_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t next = (aligned - base) + size;
    if (next < size || next > arena->capacity) return nullptr;
    arena->offset = next;
    arena->high_watermark = arena->offset > arena->high_watermark ? arena->offset : arena->high_watermark;
    arena->allocations += 1;
    void* ptr = reinterpret_cast<void*>(aligned);
    if (zeroed) std::memset(ptr, 0, size);
    return ptr;
}
}

katmer_arena* katmer_arena_create(size_t capacity) {
    return katmer_arena_create_aligned(capacity, alignof(void*));
}

katmer_arena* katmer_arena_create_aligned(size_t capacity, size_t alignment) {
    if (capacity == 0) return nullptr;
    katmer_arena* arena = new katmer_arena();
    arena->alignment = katmer_allocator_normalize_alignment(alignment);
    arena->allocator = katmer_allocator_heap();
    size_t padding = arena->alignment > alignof(void*) ? arena->alignment : alignof(void*);
    if (capacity > static_cast<size_t>(-1) - padding) {
        delete arena;
        return nullptr;
    }
    arena->reserved = capacity + padding;
    arena->data = static_cast<unsigned char*>(katmer_allocator_alloc(arena->allocator, arena->reserved, arena->alignment));
    if (!arena->data) {
        delete arena;
        return nullptr;
    }
    arena->capacity = capacity;
    return arena;
}

void katmer_arena_destroy(katmer_arena* arena) {
    if (!arena) return;
    if (arena->data) katmer_allocator_free(arena->allocator, arena->data, arena->reserved);
    delete arena;
}

void katmer_arena_reset(katmer_arena* arena) {
    if (!arena) return;
    arena->offset = 0;
    arena->allocations = 0;
}

void* katmer_arena_alloc(katmer_arena* arena, size_t size, size_t alignment) {
    return arena_alloc_internal(arena, size, alignment, false);
}

void* katmer_arena_alloc_zeroed(katmer_arena* arena, size_t size, size_t alignment) {
    return arena_alloc_internal(arena, size, alignment, true);
}

char* katmer_arena_strdup(katmer_arena* arena, const char* value) {
    if (!value) return nullptr;
    size_t length = std::strlen(value);
    char* out = static_cast<char*>(katmer_arena_alloc(arena, length + 1, alignof(char)));
    if (!out) return nullptr;
    std::memcpy(out, value, length + 1);
    return out;
}

katmer_arena_stats katmer_arena_get_stats(katmer_arena* arena) {
    if (!arena) return katmer_arena_stats{0, 0, 0, 0, 0};
    return katmer_arena_stats{
        arena->capacity,
        arena->offset,
        arena->capacity > arena->offset ? arena->capacity - arena->offset : 0,
        arena->high_watermark,
        arena->allocations
    };
}

const char* katmer_arena_stats_json(katmer_arena_stats stats) {
    std::ostringstream out;
    out << "{";
    out << "\"capacity\":" << stats.capacity << ",";
    out << "\"used\":" << stats.used << ",";
    out << "\"remaining\":" << stats.remaining << ",";
    out << "\"high_watermark\":" << stats.high_watermark << ",";
    out << "\"allocations\":" << stats.allocations;
    out << "}";
    return katmer_copy_cstr(out.str());
}
