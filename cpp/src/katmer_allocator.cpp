#include "katmer_internal.h"
#include "katmer/katmer_allocator.h"

#if defined(_WIN32)
#include <malloc.h>
#elif defined(__APPLE__) || defined(__ANDROID__) || defined(__linux__) || defined(__unix__)
#include <errno.h>
#endif

namespace {
bool is_power_of_two(size_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

size_t checked_alignment(size_t alignment) {
    alignment = katmer_allocator_normalize_alignment(alignment);
    if (alignment < sizeof(void*)) alignment = sizeof(void*);
    return alignment;
}

void* heap_alloc(void*, size_t size, size_t alignment) {
    if (size == 0) return nullptr;
    alignment = checked_alignment(alignment);
#if defined(_WIN32)
    return _aligned_malloc(size, alignment);
#elif defined(__APPLE__) || defined(__ANDROID__) || defined(__linux__) || defined(__unix__)
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) return nullptr;
    return ptr;
#else
    void* raw = std::malloc(size + alignment + sizeof(void*));
    if (!raw) return nullptr;
    size_t start = reinterpret_cast<size_t>(raw) + sizeof(void*);
    size_t aligned = (start + alignment - 1) & ~(alignment - 1);
    reinterpret_cast<void**>(aligned)[-1] = raw;
    return reinterpret_cast<void*>(aligned);
#endif
}

void heap_free(void*, void* ptr, size_t) {
    if (!ptr) return;
#if defined(_WIN32)
    _aligned_free(ptr);
#elif defined(__APPLE__) || defined(__ANDROID__) || defined(__linux__) || defined(__unix__)
    std::free(ptr);
#else
    std::free(reinterpret_cast<void**>(ptr)[-1]);
#endif
}

void* heap_realloc(void* user_data, void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    if (!ptr) return heap_alloc(user_data, new_size, alignment);
    if (new_size == 0) {
        heap_free(user_data, ptr, old_size);
        return nullptr;
    }
    void* next = heap_alloc(user_data, new_size, alignment);
    if (!next) return nullptr;
    std::memcpy(next, ptr, old_size < new_size ? old_size : new_size);
    heap_free(user_data, ptr, old_size);
    return next;
}
}

katmer_allocator_platform katmer_allocator_current_platform(void) {
#if defined(_WIN32)
    return KATMER_ALLOCATOR_PLATFORM_WINDOWS;
#elif defined(__APPLE__)
    return KATMER_ALLOCATOR_PLATFORM_APPLE;
#elif defined(__ANDROID__)
    return KATMER_ALLOCATOR_PLATFORM_ANDROID;
#elif defined(__linux__)
    return KATMER_ALLOCATOR_PLATFORM_LINUX;
#elif defined(__unix__)
    return KATMER_ALLOCATOR_PLATFORM_POSIX;
#else
    return KATMER_ALLOCATOR_PLATFORM_UNKNOWN;
#endif
}

const char* katmer_allocator_platform_name(katmer_allocator_platform platform) {
    switch (platform) {
    case KATMER_ALLOCATOR_PLATFORM_WINDOWS: return "windows";
    case KATMER_ALLOCATOR_PLATFORM_APPLE: return "apple";
    case KATMER_ALLOCATOR_PLATFORM_ANDROID: return "android";
    case KATMER_ALLOCATOR_PLATFORM_LINUX: return "linux";
    case KATMER_ALLOCATOR_PLATFORM_POSIX: return "posix";
    default: return "unknown";
    }
}

size_t katmer_allocator_normalize_alignment(size_t alignment) {
    if (alignment == 0) return sizeof(void*);
    if (alignment < sizeof(void*)) alignment = sizeof(void*);
    if (is_power_of_two(alignment)) return alignment;
    size_t power = sizeof(void*);
    while (power < alignment) {
        if (power > (static_cast<size_t>(-1) / 2)) return sizeof(void*);
        power <<= 1;
    }
    return power;
}

int katmer_allocator_alignment_is_valid(size_t alignment) {
    if (alignment == 0) return 1;
    return alignment >= sizeof(void*) && is_power_of_two(alignment);
}

katmer_allocator katmer_allocator_heap(void) {
    return katmer_allocator{nullptr, heap_alloc, heap_realloc, heap_free};
}

void* katmer_allocator_alloc(katmer_allocator allocator, size_t size, size_t alignment) {
    if (!allocator.alloc) allocator = katmer_allocator_heap();
    return allocator.alloc(allocator.user_data, size, alignment);
}

void* katmer_allocator_realloc(katmer_allocator allocator, void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    if (!allocator.realloc_fn) allocator = katmer_allocator_heap();
    return allocator.realloc_fn(allocator.user_data, ptr, old_size, new_size, alignment);
}

void katmer_allocator_free(katmer_allocator allocator, void* ptr, size_t size) {
    if (!ptr) return;
    if (!allocator.free_fn) allocator = katmer_allocator_heap();
    allocator.free_fn(allocator.user_data, ptr, size);
}
