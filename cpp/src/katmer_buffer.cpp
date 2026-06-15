#include "katmer_internal.h"
#include "katmer/katmer_buffer.h"

struct katmer_buffer {
    char* data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
    katmer_allocator allocator = katmer_allocator_heap();
};

namespace {
size_t next_capacity(size_t current, size_t wanted) {
    size_t cap = current ? current : 64;
    while (cap < wanted) cap *= 2;
    return cap;
}
}

katmer_buffer* katmer_buffer_create(size_t initial_capacity) {
    return katmer_buffer_create_with_allocator(initial_capacity, katmer_allocator_heap());
}

katmer_buffer* katmer_buffer_create_with_allocator(size_t initial_capacity, katmer_allocator allocator) {
    katmer_buffer* buffer = new katmer_buffer();
    buffer->allocator = allocator.alloc ? allocator : katmer_allocator_heap();
    if (initial_capacity && !katmer_buffer_reserve(buffer, initial_capacity)) {
        delete buffer;
        return nullptr;
    }
    return buffer;
}

void katmer_buffer_destroy(katmer_buffer* buffer) {
    if (!buffer) return;
    katmer_allocator_free(buffer->allocator, buffer->data, buffer->capacity);
    delete buffer;
}

void katmer_buffer_clear(katmer_buffer* buffer) {
    if (!buffer) return;
    buffer->size = 0;
    if (buffer->data) buffer->data[0] = '\0';
}

int katmer_buffer_reserve(katmer_buffer* buffer, size_t capacity) {
    if (!buffer) return 0;
    if (capacity + 1 <= buffer->capacity) return 1;
    size_t next = next_capacity(buffer->capacity, capacity + 1);
    char* data = static_cast<char*>(katmer_allocator_realloc(buffer->allocator, buffer->data, buffer->capacity, next, alignof(char)));
    if (!data) return 0;
    buffer->data = data;
    buffer->capacity = next;
    if (buffer->size == 0) buffer->data[0] = '\0';
    return 1;
}

int katmer_buffer_write(katmer_buffer* buffer, const void* data, size_t size) {
    if (!buffer || (!data && size)) return 0;
    if (!katmer_buffer_reserve(buffer, buffer->size + size)) return 0;
    if (size) std::memcpy(buffer->data + buffer->size, data, size);
    buffer->size += size;
    buffer->data[buffer->size] = '\0';
    return 1;
}

int katmer_buffer_write_cstr(katmer_buffer* buffer, const char* value) {
    return katmer_buffer_write(buffer, value ? value : "", value ? std::strlen(value) : 0);
}

int katmer_buffer_write_byte(katmer_buffer* buffer, unsigned char value) {
    return katmer_buffer_write(buffer, &value, 1);
}

size_t katmer_buffer_size(katmer_buffer* buffer) {
    return buffer ? buffer->size : 0;
}

size_t katmer_buffer_capacity(katmer_buffer* buffer) {
    return buffer ? buffer->capacity : 0;
}

const char* katmer_buffer_data(katmer_buffer* buffer) {
    return buffer && buffer->data ? buffer->data : "";
}

katmer_string_view katmer_buffer_view(katmer_buffer* buffer) {
    return katmer_sv_from_parts(katmer_buffer_data(buffer), katmer_buffer_size(buffer));
}

char* katmer_buffer_to_cstr(katmer_buffer* buffer) {
    return katmer_copy_cstr(katmer_buffer_data(buffer));
}

const char* katmer_buffer_stats_json(katmer_buffer* buffer) {
    std::ostringstream out;
    out << "{\"size\":" << katmer_buffer_size(buffer) << ",\"capacity\":" << katmer_buffer_capacity(buffer) << "}";
    return katmer_copy_cstr(out.str());
}

katmer_string_view katmer_sv_from_parts(const char* data, size_t size) {
    return katmer_string_view{data ? data : "", data ? size : 0};
}

katmer_string_view katmer_sv_from_cstr(const char* value) {
    return katmer_sv_from_parts(value ? value : "", value ? std::strlen(value) : 0);
}

katmer_string_view katmer_sv_trim(katmer_string_view value) {
    size_t start = 0;
    while (start < value.size && std::isspace(static_cast<unsigned char>(value.data[start]))) start++;
    size_t end = value.size;
    while (end > start && std::isspace(static_cast<unsigned char>(value.data[end - 1]))) end--;
    return katmer_sv_from_parts(value.data + start, end - start);
}

int katmer_sv_equals(katmer_string_view left, katmer_string_view right) {
    if (left.size != right.size) return 0;
    if (left.size == 0) return 1;
    return std::memcmp(left.data, right.data, left.size) == 0;
}

int katmer_sv_contains(katmer_string_view value, katmer_string_view needle) {
    if (needle.size == 0) return 1;
    if (needle.size > value.size) return 0;
    for (size_t i = 0; i + needle.size <= value.size; ++i) {
        if (std::memcmp(value.data + i, needle.data, needle.size) == 0) return 1;
    }
    return 0;
}

katmer_string_view katmer_sv_slice(katmer_string_view value, size_t start, size_t size) {
    if (start >= value.size) return katmer_sv_from_parts("", 0);
    size_t available = value.size - start;
    return katmer_sv_from_parts(value.data + start, size < available ? size : available);
}

const char* katmer_sv_json(katmer_string_view value) {
    std::string text(value.data ? value.data : "", value.size);
    std::ostringstream out;
    out << "{\"size\":" << value.size << ",\"value\":\"" << katmer_json_escape(text) << "\"}";
    return katmer_copy_cstr(out.str());
}
