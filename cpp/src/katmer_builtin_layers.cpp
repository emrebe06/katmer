#include "katmer_internal.h"

#include <vector>

namespace {
bool text_contains(const std::string& haystack, const std::string& needle) {
    return katmer_lower(haystack).find(needle) != std::string::npos;
}

bool balanced(const std::string& text, char open_ch, char close_ch) {
    int depth = 0;
    bool in_string = false;
    bool escaped = false;
    for (char ch : text) {
        if (in_string) {
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == '"') {
                in_string = false;
            }
            continue;
        }
        if (ch == '"') {
            in_string = true;
            continue;
        }
        if (ch == open_ch) ++depth;
        if (ch == close_ch) --depth;
        if (depth < 0) return false;
    }
    return depth == 0 && !in_string;
}

void emit_if_contains(katmer_context* ctx, const std::string& body, const char* token, const char* code, double score) {
    if (text_contains(body, token)) {
        std::string message = std::string("matched token: ") + token;
        katmer_context_signal(ctx, code, message.c_str(), score);
    }
}
}

katmer_result katmer_layer_parse(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "PARSE_NO_CONTEXT", "Missing context");
    ctx->store["input.type"] = ctx->input_type;
    ctx->store["input.bytes"] = std::to_string(ctx->input_text.size());
    bool is_dsl = katmer_parse_dsl(ctx->input_text, ctx->store);
    ctx->store["parse.dsl"] = is_dsl ? "true" : "false";
    if (!is_dsl) {
        ctx->store["dsl.module"] = "core";
        ctx->store["dsl.type"] = ctx->input_type;
        ctx->store["dsl.body"] = ctx->input_text;
    }
    return katmer_continue("PARSE_OK", is_dsl ? "Katmer DSL parsed" : "Raw input accepted");
}

katmer_result katmer_layer_shape(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "SHAPE_NO_CONTEXT", "Missing context");
    katmer_shape_text(ctx->store["dsl.body"], ctx->store);
    ctx->store["arena.used_after_shape"] = std::to_string(katmer_context_arena_used(ctx));
    return katmer_continue("SHAPE_OK", "Input shape measured");
}

katmer_result katmer_layer_validate(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "VALIDATE_NO_CONTEXT", "Missing context");
    const std::string& body = ctx->store["dsl.body"];
    const std::string& type = ctx->store["dsl.type"];
    if (body.empty()) {
        katmer_context_signal(ctx, "empty_input", "Katmer body is empty", 0.1);
        return katmer_stop("VALIDATE_EMPTY", "Empty input stopped");
    }
    if (body.size() > 1024 * 1024 * 8) {
        katmer_context_signal(ctx, "oversized_input", "Katmer body exceeds 8MB default limit", 0.7);
        return katmer_stop("VALIDATE_TOO_LARGE", "Input too large");
    }
    if ((type == "json" || body.find('{') != std::string::npos || body.find('[') != std::string::npos)
        && (!balanced(body, '{', '}') || !balanced(body, '[', ']'))) {
        katmer_context_signal(ctx, "json_unbalanced", "JSON-like body has unbalanced braces or brackets", 0.55);
        return katmer_stop("VALIDATE_JSON_UNBALANCED", "JSON-like input is not balanced");
    }
    return katmer_continue("VALIDATE_OK", "Input validation passed");
}

katmer_result katmer_layer_guard(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "GUARD_NO_CONTEXT", "Missing context");
    const std::string& body = ctx->store["dsl.body"];
    emit_if_contains(ctx, body, "drop table", "sql_injection", 0.45);
    emit_if_contains(ctx, body, "union select", "sql_injection", 0.45);
    emit_if_contains(ctx, body, " or 1=1", "sql_injection", 0.45);
    emit_if_contains(ctx, body, "../", "path_traversal", 0.35);
    emit_if_contains(ctx, body, "..\\", "path_traversal", 0.35);
    emit_if_contains(ctx, body, "169.254.169.254", "ssrf", 0.5);
    emit_if_contains(ctx, body, "metadata.google.internal", "ssrf", 0.5);
    emit_if_contains(ctx, body, "powershell", "command_injection", 0.5);
    emit_if_contains(ctx, body, "cmd.exe", "command_injection", 0.5);
    emit_if_contains(ctx, body, "/bin/sh", "command_injection", 0.5);
    emit_if_contains(ctx, body, "api_key", "secret_like", 0.2);
    emit_if_contains(ctx, body, "access_token", "secret_like", 0.2);

    double total = 0.0;
    for (const KatmerSignal& signal : ctx->signals) {
        total += signal.score;
    }
    ctx->store["guard.risk"] = std::to_string(total > 0.99 ? 0.99 : total);
    if (total >= 0.65) {
        ctx->store["guard.action"] = "block";
        return katmer_stop("GUARD_BLOCK", "Guard blocked risky input");
    }
    if (total > 0.0) {
        ctx->store["guard.action"] = "observe";
        return katmer_continue("GUARD_OBSERVE", "Guard observed risk signals");
    }
    ctx->store["guard.action"] = "allow";
    return katmer_continue("GUARD_ALLOW", "Guard allowed input");
}

katmer_result katmer_layer_select(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "SELECT_NO_CONTEXT", "Missing context");
    const std::string& module = ctx->store["dsl.module"];
    if (module == "http") {
        std::string body = ctx->store["dsl.body"];
        size_t line_end = body.find('\n');
        std::string first = line_end == std::string::npos ? body : body.substr(0, line_end);
        ctx->store["select.kind"] = "http";
        ctx->store["select.target"] = first;
    } else if (module == "llm") {
        ctx->store["select.kind"] = "llm";
        ctx->store["select.target"] = "provider";
    } else if (module == "audio" || module == "dsp") {
        const std::string& type = ctx->store["dsl.type"];
        ctx->store["select.kind"] = "dsp";
        ctx->store["select.target"] = type.empty() || type == "str" ? "signal.pipeline" : type;
        ctx->store["dsp.ready"] = "true";
        ctx->store["dsp.mode"] = type.empty() ? "pipeline" : type;
    } else if (module == "video") {
        ctx->store["select.kind"] = "video";
        ctx->store["select.target"] = "media.job";
    } else if (module == "android") {
        ctx->store["select.kind"] = "android";
        ctx->store["select.target"] = "mobile.bridge";
    } else {
        ctx->store["select.kind"] = module.empty() ? "core" : module;
        ctx->store["select.target"] = "default";
    }
    return katmer_continue("SELECT_OK", "Execution target selected");
}

katmer_result katmer_layer_output(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "OUTPUT_NO_CONTEXT", "Missing context");
    std::ostringstream out;
    out << "{";
    out << "\"module\":\"" << katmer_json_escape(ctx->store["dsl.module"]) << "\",";
    out << "\"type\":\"" << katmer_json_escape(ctx->store["dsl.type"]) << "\",";
    out << "\"body\":\"" << katmer_json_escape(ctx->store["dsl.body"]) << "\",";
    out << "\"select_kind\":\"" << katmer_json_escape(ctx->store["select.kind"]) << "\",";
    out << "\"select\":\"" << katmer_json_escape(ctx->store["select.target"]) << "\",";
    out << "\"guard\":\"" << katmer_json_escape(ctx->store["guard.action"]) << "\"";
    out << "}";
    ctx->output_text = out.str();
    return katmer_continue("OUTPUT_OK", "Output generated");
}

katmer_result katmer_layer_trace(katmer_context* ctx) {
    if (!ctx) return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "TRACE_NO_CONTEXT", "Missing context");
    ctx->store["trace.layers"] = std::to_string(ctx->trace.size() + 1);
    ctx->store["trace.signals"] = std::to_string(ctx->signals.size());
    return katmer_continue("TRACE_OK", "Trace recorded");
}
