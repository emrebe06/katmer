#ifndef KATMER_INTERNAL_H
#define KATMER_INTERNAL_H

#include "katmer.h"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct KatmerSignal {
    std::string code;
    std::string message;
    double score = 0.0;
};

struct KatmerLayerTrace {
    std::string name;
    int ok = 1;
    std::string code;
    std::string message;
    std::string action;
    double time_ms = 0.0;
};

struct KatmerLayer {
    std::string name;
    katmer_layer_fn fn = nullptr;
};

struct KatmerArena {
    unsigned char* data = nullptr;
    size_t capacity = 0;
    size_t offset = 0;
    size_t high_watermark = 0;
    size_t allocations = 0;

    bool create(size_t bytes) {
        destroy();
        capacity = bytes;
        offset = 0;
        high_watermark = 0;
        allocations = 0;
        data = static_cast<unsigned char*>(std::malloc(bytes));
        return data != nullptr;
    }

    void destroy() {
        if (data) {
            std::free(data);
        }
        data = nullptr;
        capacity = 0;
        offset = 0;
        high_watermark = 0;
        allocations = 0;
    }

    void reset() {
        offset = 0;
        allocations = 0;
    }

    void* alloc(size_t size, size_t alignment) {
        if (!data || size == 0) return nullptr;
        if (alignment == 0) alignment = alignof(void*);
        size_t current = reinterpret_cast<size_t>(data + offset);
        size_t aligned = (current + alignment - 1) & ~(alignment - 1);
        size_t next_offset = (aligned - reinterpret_cast<size_t>(data)) + size;
        if (next_offset > capacity) {
            return nullptr;
        }
        offset = next_offset;
        if (offset > high_watermark) high_watermark = offset;
        allocations += 1;
        return reinterpret_cast<void*>(aligned);
    }

    size_t remaining() const {
        return capacity > offset ? capacity - offset : 0;
    }
};

struct katmer_runtime {
    size_t bake_count = 0;
    size_t default_arena_size = 1024 * 1024;
};

struct katmer_context {
    katmer_runtime* runtime = nullptr;
    std::string input_type;
    std::string input_text;
    std::string output_text;
    std::map<std::string, std::string> store;
    std::vector<KatmerSignal> signals;
    std::vector<KatmerLayerTrace> trace;
    KatmerArena arena;
    std::string trace_id;
    bool stopped = false;
    bool failed = false;

    ~katmer_context() {
        arena.destroy();
    }
};

struct katmer_stack {
    katmer_runtime* runtime = nullptr;
    std::vector<KatmerLayer> layers;
};

inline long long katmer_now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

inline std::string katmer_json_escape(const std::string& value) {
    std::ostringstream out;
    const char* hex = "0123456789abcdef";
    for (unsigned char ch : value) {
        switch (ch) {
        case '\\': out << "\\\\"; break;
        case '"': out << "\\\""; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        default:
            if (ch < 32) {
                out << "\\u00" << hex[(ch >> 4) & 0x0f] << hex[ch & 0x0f];
            } else {
                out << static_cast<char>(ch);
            }
            break;
        }
    }
    return out.str();
}

inline char* katmer_copy_cstr(const std::string& text) {
    char* out = static_cast<char*>(std::malloc(text.size() + 1));
    if (!out) return nullptr;
    std::memcpy(out, text.c_str(), text.size() + 1);
    return out;
}

inline std::string katmer_lower(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

inline std::string katmer_trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) ++start;
    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) --end;
    return value.substr(start, end - start);
}

std::string katmer_store_json(const std::map<std::string, std::string>& store);
std::string katmer_signals_json(const std::vector<KatmerSignal>& signals);
std::string katmer_trace_json(const std::vector<KatmerLayerTrace>& trace);
std::string katmer_context_json(const katmer_context& ctx, const katmer_result& result);
bool katmer_parse_dsl(const std::string& input, std::map<std::string, std::string>& store);
void katmer_shape_text(const std::string& input, std::map<std::string, std::string>& store);

#endif
