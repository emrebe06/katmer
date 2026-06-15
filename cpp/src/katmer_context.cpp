#include "katmer_internal.h"

namespace {
std::string attr_value(const std::string& tag, const std::string& name) {
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

std::string number_to_string(size_t value) {
    return std::to_string(static_cast<unsigned long long>(value));
}
}

const char* katmer_context_get(katmer_context* ctx, const char* key) {
    if (!ctx || !key) return "";
    auto found = ctx->store.find(key);
    return found == ctx->store.end() ? "" : found->second.c_str();
}

void katmer_context_set(katmer_context* ctx, const char* key, const char* value) {
    if (!ctx || !key) return;
    ctx->store[key] = value ? value : "";
}

void katmer_context_signal(katmer_context* ctx, const char* code, const char* message, double score) {
    if (!ctx) return;
    ctx->signals.push_back(KatmerSignal{code ? code : "SIGNAL", message ? message : "", score});
}

void katmer_context_output(katmer_context* ctx, const char* value) {
    if (!ctx) return;
    ctx->output_text = value ? value : "{}";
}

const char* katmer_context_input_text(katmer_context* ctx) {
    return ctx ? ctx->input_text.c_str() : "";
}

const char* katmer_context_input_type(katmer_context* ctx) {
    return ctx ? ctx->input_type.c_str() : "";
}

void* katmer_context_alloc(katmer_context* ctx, size_t size, size_t alignment) {
    if (!ctx) return nullptr;
    return ctx->arena.alloc(size, alignment);
}

size_t katmer_context_arena_used(katmer_context* ctx) {
    return ctx ? ctx->arena.offset : 0;
}

size_t katmer_context_arena_remaining(katmer_context* ctx) {
    return ctx ? ctx->arena.remaining() : 0;
}

size_t katmer_context_signal_count(katmer_context* ctx) {
    return ctx ? ctx->signals.size() : 0;
}

std::string katmer_store_json(const std::map<std::string, std::string>& store) {
    std::ostringstream out;
    out << "{";
    bool first = true;
    for (const auto& item : store) {
        if (!first) out << ",";
        out << "\"" << katmer_json_escape(item.first) << "\":\"" << katmer_json_escape(item.second) << "\"";
        first = false;
    }
    out << "}";
    return out.str();
}

std::string katmer_signals_json(const std::vector<KatmerSignal>& signals) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < signals.size(); ++i) {
        if (i) out << ",";
        out << "{";
        out << "\"code\":\"" << katmer_json_escape(signals[i].code) << "\",";
        out << "\"message\":\"" << katmer_json_escape(signals[i].message) << "\",";
        out << "\"score\":" << signals[i].score;
        out << "}";
    }
    out << "]";
    return out.str();
}

std::string katmer_trace_json(const std::vector<KatmerLayerTrace>& trace) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < trace.size(); ++i) {
        if (i) out << ",";
        out << "{";
        out << "\"name\":\"" << katmer_json_escape(trace[i].name) << "\",";
        out << "\"ok\":" << (trace[i].ok ? "true" : "false") << ",";
        out << "\"action\":\"" << katmer_json_escape(trace[i].action) << "\",";
        out << "\"code\":\"" << katmer_json_escape(trace[i].code) << "\",";
        out << "\"message\":\"" << katmer_json_escape(trace[i].message) << "\",";
        out << "\"time_ms\":" << trace[i].time_ms;
        out << "}";
    }
    out << "]";
    return out.str();
}

std::string katmer_context_json(const katmer_context& ctx, const katmer_result& result) {
    std::ostringstream out;
    auto module = ctx.store.find("dsl.module");
    auto type = ctx.store.find("dsl.type");
    out << "{";
    out << "\"ok\":" << (result.ok ? "true" : "false") << ",";
    out << "\"engine\":\"katmer\",";
    out << "\"trace_id\":\"" << katmer_json_escape(ctx.trace_id) << "\",";
    out << "\"input_type\":\"" << katmer_json_escape(ctx.input_type) << "\",";
    out << "\"module\":\"" << katmer_json_escape(module == ctx.store.end() ? "core" : module->second) << "\",";
    out << "\"type\":\"" << katmer_json_escape(type == ctx.store.end() ? ctx.input_type : type->second) << "\",";
    out << "\"action\":\"" << katmer_action_name(result.action) << "\",";
    out << "\"status\":\"" << katmer_status_name(result.status) << "\",";
    out << "\"code\":\"" << katmer_json_escape(result.code ? result.code : "") << "\",";
    out << "\"message\":\"" << katmer_json_escape(result.message ? result.message : "") << "\",";
    out << "\"arena\":{";
    out << "\"used\":" << ctx.arena.offset << ",";
    out << "\"remaining\":" << ctx.arena.remaining() << ",";
    out << "\"high_watermark\":" << ctx.arena.high_watermark << ",";
    out << "\"allocations\":" << ctx.arena.allocations;
    out << "},";
    out << "\"store\":" << katmer_store_json(ctx.store) << ",";
    out << "\"signals\":" << katmer_signals_json(ctx.signals) << ",";
    out << "\"layers\":" << katmer_trace_json(ctx.trace) << ",";
    out << "\"result\":" << (ctx.output_text.empty() ? "{}" : ctx.output_text);
    out << "}";
    return out.str();
}

bool katmer_parse_dsl(const std::string& input, std::map<std::string, std::string>& store) {
    std::string lowered = katmer_lower(input);
    size_t open = lowered.find("<katmer");
    if (open == std::string::npos) return false;
    size_t open_end = lowered.find(">", open);
    if (open_end == std::string::npos) return false;
    size_t close = lowered.rfind("</katmer>");
    if (close == std::string::npos || close < open_end) return false;
    std::string tag = input.substr(open, open_end - open + 1);
    store["dsl.module"] = attr_value(tag, "module");
    store["dsl.type"] = attr_value(tag, "type");
    store["dsl.body"] = katmer_trim(input.substr(open_end + 1, close - open_end - 1));
    store["dsl.raw_open"] = tag;
    if (store["dsl.module"].empty()) store["dsl.module"] = "core";
    if (store["dsl.type"].empty()) store["dsl.type"] = "str";
    return true;
}

void katmer_shape_text(const std::string& input, std::map<std::string, std::string>& store) {
    size_t lines = input.empty() ? 0 : 1;
    size_t braces = 0;
    size_t brackets = 0;
    size_t digits = 0;
    size_t symbols = 0;
    size_t quotes = 0;
    size_t control = 0;
    size_t max_line = 0;
    size_t current_line = 0;
    for (unsigned char ch : input) {
        if (ch == '\n') {
            ++lines;
            if (current_line > max_line) max_line = current_line;
            current_line = 0;
        } else {
            ++current_line;
        }
        if (ch == '{' || ch == '}') ++braces;
        if (ch == '[' || ch == ']') ++brackets;
        if (ch == '"') ++quotes;
        if (std::isdigit(ch)) ++digits;
        if (!std::isalnum(ch) && !std::isspace(ch)) ++symbols;
        if (ch < 32 && ch != '\n' && ch != '\r' && ch != '\t') ++control;
    }
    if (current_line > max_line) max_line = current_line;
    store["shape.bytes"] = number_to_string(input.size());
    store["shape.lines"] = number_to_string(lines);
    store["shape.braces"] = number_to_string(braces);
    store["shape.brackets"] = number_to_string(brackets);
    store["shape.quotes"] = number_to_string(quotes);
    store["shape.digits"] = number_to_string(digits);
    store["shape.symbols"] = number_to_string(symbols);
    store["shape.control"] = number_to_string(control);
    store["shape.max_line"] = number_to_string(max_line);
}
