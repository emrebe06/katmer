#include "katmer_internal.h"
#include "katmer_dsl.h"

namespace {
std::string parse_attr(const std::string& tag, const std::string& name) {
    std::string needle = name + "=";
    size_t pos = tag.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    if (pos >= tag.size()) return "";
    char quote = tag[pos];
    if (quote != '"' && quote != '\'') return "";
    size_t end = tag.find(quote, pos + 1);
    if (end == std::string::npos) return "";
    return tag.substr(pos + 1, end - pos - 1);
}

std::map<std::string, std::string> parse_attrs(const std::string& tag) {
    std::map<std::string, std::string> attrs;
    size_t pos = 0;
    while (pos < tag.size()) {
        while (pos < tag.size() && !std::isalpha(static_cast<unsigned char>(tag[pos]))) ++pos;
        size_t name_start = pos;
        while (pos < tag.size() && (std::isalnum(static_cast<unsigned char>(tag[pos])) || tag[pos] == '_' || tag[pos] == '-' || tag[pos] == ':')) ++pos;
        if (name_start == pos) break;
        std::string name = tag.substr(name_start, pos - name_start);
        while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos]))) ++pos;
        if (pos >= tag.size() || tag[pos] != '=') continue;
        ++pos;
        while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos]))) ++pos;
        if (pos >= tag.size() || (tag[pos] != '"' && tag[pos] != '\'')) continue;
        char quote = tag[pos++];
        size_t value_start = pos;
        size_t value_end = tag.find(quote, pos);
        if (value_end == std::string::npos) break;
        attrs[name] = tag.substr(value_start, value_end - value_start);
        pos = value_end + 1;
    }
    return attrs;
}

bool parse_doc(const char* input, size_t input_size, std::string& tag, std::string& body, std::string& error) {
    if (!input) {
        error = "input is null";
        return false;
    }
    std::string text(input, input_size);
    std::string lowered = katmer_lower(text);
    size_t open = lowered.find("<katmer");
    if (open == std::string::npos) {
        error = "opening <katmer> tag not found";
        return false;
    }
    size_t open_end = lowered.find(">", open);
    if (open_end == std::string::npos) {
        error = "opening <katmer> tag is not closed";
        return false;
    }
    size_t close = lowered.rfind("</katmer>");
    if (close == std::string::npos || close < open_end) {
        error = "closing </katmer> tag not found";
        return false;
    }
    tag = text.substr(open, open_end - open + 1);
    body = katmer_trim(text.substr(open_end + 1, close - open_end - 1));
    return true;
}
}

int katmer_dsl_has_document(const char* input, size_t input_size) {
    std::string tag;
    std::string body;
    std::string error;
    return parse_doc(input, input_size, tag, body, error) ? 1 : 0;
}

const char* katmer_dsl_attr_json(const char* input, size_t input_size) {
    std::string tag;
    std::string body;
    std::string error;
    if (!parse_doc(input, input_size, tag, body, error)) {
        return katmer_copy_cstr("{\"ok\":false,\"attrs\":{},\"error\":\"" + katmer_json_escape(error) + "\"}");
    }
    std::map<std::string, std::string> attrs = parse_attrs(tag);
    std::ostringstream out;
    out << "{\"ok\":true,\"attrs\":{";
    bool first = true;
    for (const auto& item : attrs) {
        if (!first) out << ",";
        out << "\"" << katmer_json_escape(item.first) << "\":\"" << katmer_json_escape(item.second) << "\"";
        first = false;
    }
    out << "}}";
    return katmer_copy_cstr(out.str());
}

const char* katmer_dsl_parse_json(const char* input, size_t input_size) {
    std::string tag;
    std::string body;
    std::string error;
    if (!parse_doc(input, input_size, tag, body, error)) {
        std::ostringstream fail;
        fail << "{\"ok\":false,\"engine\":\"katmer-dsl\",\"code\":\"DSL_PARSE_ERROR\",";
        fail << "\"message\":\"" << katmer_json_escape(error) << "\"}";
        return katmer_copy_cstr(fail.str());
    }
    std::string module = parse_attr(tag, "module");
    std::string type = parse_attr(tag, "type");
    if (module.empty()) module = "core";
    if (type.empty()) type = "str";
    std::map<std::string, std::string> shape;
    katmer_shape_text(body, shape);
    std::ostringstream out;
    out << "{";
    out << "\"ok\":true,";
    out << "\"engine\":\"katmer-dsl\",";
    out << "\"module\":\"" << katmer_json_escape(module) << "\",";
    out << "\"type\":\"" << katmer_json_escape(type) << "\",";
    out << "\"body\":\"" << katmer_json_escape(body) << "\",";
    out << "\"shape\":" << katmer_store_json(shape);
    out << "}";
    return katmer_copy_cstr(out.str());
}
