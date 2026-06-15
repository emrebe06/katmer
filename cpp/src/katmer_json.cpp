#include "katmer_internal.h"
#include "katmer_json.h"

const char* katmer_json_escape_string(const char* value) {
    return katmer_copy_cstr(katmer_json_escape(value ? value : ""));
}

const char* katmer_json_quote(const char* value) {
    std::string out = "\"" + katmer_json_escape(value ? value : "") + "\"";
    return katmer_copy_cstr(out);
}

const char* katmer_json_pair(const char* key, const char* value) {
    std::ostringstream out;
    out << "\"" << katmer_json_escape(key ? key : "") << "\":\"" << katmer_json_escape(value ? value : "") << "\"";
    return katmer_copy_cstr(out.str());
}

int katmer_json_like_balanced(const char* value, size_t size) {
    if (!value) return 1;
    int curly = 0;
    int square = 0;
    bool in_string = false;
    bool escaped = false;
    for (size_t i = 0; i < size; ++i) {
        char ch = value[i];
        if (in_string) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') in_string = false;
            continue;
        }
        if (ch == '"') {
            in_string = true;
        } else if (ch == '{') {
            ++curly;
        } else if (ch == '}') {
            --curly;
        } else if (ch == '[') {
            ++square;
        } else if (ch == ']') {
            --square;
        }
        if (curly < 0 || square < 0) return 0;
    }
    return !in_string && curly == 0 && square == 0;
}

size_t katmer_json_estimated_depth(const char* value, size_t size) {
    if (!value) return 0;
    size_t depth = 0;
    size_t max_depth = 0;
    bool in_string = false;
    bool escaped = false;
    for (size_t i = 0; i < size; ++i) {
        char ch = value[i];
        if (in_string) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') in_string = false;
            continue;
        }
        if (ch == '"') {
            in_string = true;
        } else if (ch == '{' || ch == '[') {
            ++depth;
            if (depth > max_depth) max_depth = depth;
        } else if ((ch == '}' || ch == ']') && depth > 0) {
            --depth;
        }
    }
    return max_depth;
}
