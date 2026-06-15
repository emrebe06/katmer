#include "katmer_internal.h"
#include "katmer_security.h"

namespace {
bool contains_ci(const std::string& text, const std::string& token) {
    return katmer_lower(text).find(token) != std::string::npos;
}

unsigned int popcount_flags(unsigned int flags) {
    unsigned int count = 0;
    while (flags) {
        count += flags & 1u;
        flags >>= 1u;
    }
    return count;
}

void add_signal(std::vector<std::string>& signals, const char* signal) {
    for (const std::string& item : signals) {
        if (item == signal) return;
    }
    signals.emplace_back(signal);
}

std::vector<std::string> signal_names(unsigned int flags) {
    std::vector<std::string> signals;
    if (flags & KATMER_SECURITY_SQL) add_signal(signals, "sql_injection");
    if (flags & KATMER_SECURITY_COMMAND) add_signal(signals, "command_injection");
    if (flags & KATMER_SECURITY_SSRF) add_signal(signals, "ssrf");
    if (flags & KATMER_SECURITY_TRAVERSAL) add_signal(signals, "path_traversal");
    if (flags & KATMER_SECURITY_SCRIPT) add_signal(signals, "script_injection");
    if (flags & KATMER_SECURITY_SECRET) add_signal(signals, "secret_like");
    if (flags & KATMER_SECURITY_OVERSIZED) add_signal(signals, "oversized");
    if (flags & KATMER_SECURITY_CONTROL) add_signal(signals, "control_character");
    if (flags & KATMER_SECURITY_MULTI_SIGNAL) add_signal(signals, "multi_signal");
    return signals;
}

std::string signals_json(const std::vector<std::string>& signals) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < signals.size(); ++i) {
        if (i) out << ",";
        out << "\"" << katmer_json_escape(signals[i]) << "\"";
    }
    out << "]";
    return out.str();
}
}

unsigned int katmer_security_scan_flags(const char* input, size_t input_size, size_t max_size) {
    if (!input) return KATMER_SECURITY_OK;
    std::string text(input, input_size);
    unsigned int flags = KATMER_SECURITY_OK;
    if (max_size > 0 && input_size > max_size) flags |= KATMER_SECURITY_OVERSIZED;
    for (unsigned char ch : text) {
        if (ch < 32 && ch != '\n' && ch != '\r' && ch != '\t') {
            flags |= KATMER_SECURITY_CONTROL;
            break;
        }
    }
    if (contains_ci(text, "drop table") || contains_ci(text, "union select") || contains_ci(text, " or 1=1") || contains_ci(text, "'--")) {
        flags |= KATMER_SECURITY_SQL;
    }
    if (contains_ci(text, "powershell") || contains_ci(text, "cmd.exe") || contains_ci(text, "/bin/sh") || contains_ci(text, "&&")) {
        flags |= KATMER_SECURITY_COMMAND;
    }
    if (contains_ci(text, "169.254.169.254") || contains_ci(text, "metadata.google.internal") || contains_ci(text, "localhost:") || contains_ci(text, "file://")) {
        flags |= KATMER_SECURITY_SSRF;
    }
    if (contains_ci(text, "../") || contains_ci(text, "..\\") || contains_ci(text, "%2e") || contains_ci(text, "%2f")) {
        flags |= KATMER_SECURITY_TRAVERSAL;
    }
    if (contains_ci(text, "<script") || contains_ci(text, "javascript:") || contains_ci(text, "onerror=")) {
        flags |= KATMER_SECURITY_SCRIPT;
    }
    if (contains_ci(text, "api_key") || contains_ci(text, "access_token") || contains_ci(text, "private_key") || contains_ci(text, "password")) {
        flags |= KATMER_SECURITY_SECRET;
    }
    unsigned int count = popcount_flags(flags);
    if (count >= 3) flags |= KATMER_SECURITY_MULTI_SIGNAL;
    return flags;
}

double katmer_security_score(unsigned int flags) {
    double score = 0.0;
    if (flags & KATMER_SECURITY_SQL) score += 0.32;
    if (flags & KATMER_SECURITY_COMMAND) score += 0.36;
    if (flags & KATMER_SECURITY_SSRF) score += 0.34;
    if (flags & KATMER_SECURITY_TRAVERSAL) score += 0.24;
    if (flags & KATMER_SECURITY_SCRIPT) score += 0.30;
    if (flags & KATMER_SECURITY_SECRET) score += 0.12;
    if (flags & KATMER_SECURITY_OVERSIZED) score += 0.18;
    if (flags & KATMER_SECURITY_CONTROL) score += 0.25;
    if (flags & KATMER_SECURITY_MULTI_SIGNAL) score += 0.18;
    return score > 0.99 ? 0.99 : score;
}

const char* katmer_security_action(unsigned int flags) {
    double score = katmer_security_score(flags);
    if (score >= 0.65) return "block";
    if (score >= 0.30) return "observe";
    return "allow";
}

const char* katmer_security_flags_json(unsigned int flags) {
    std::vector<std::string> signals = signal_names(flags);
    std::ostringstream out;
    out << "{";
    out << "\"flags\":" << flags << ",";
    out << "\"score\":" << katmer_security_score(flags) << ",";
    out << "\"action\":\"" << katmer_security_action(flags) << "\",";
    out << "\"signals\":" << signals_json(signals);
    out << "}";
    return katmer_copy_cstr(out.str());
}

const char* katmer_security_scan_json(const char* input, size_t input_size, size_t max_size) {
    unsigned int flags = katmer_security_scan_flags(input, input_size, max_size);
    std::vector<std::string> signals = signal_names(flags);
    std::ostringstream out;
    out << "{";
    out << "\"ok\":" << (flags == KATMER_SECURITY_OK ? "true" : "false") << ",";
    out << "\"engine\":\"katmer-security\",";
    out << "\"flags\":" << flags << ",";
    out << "\"score\":" << katmer_security_score(flags) << ",";
    out << "\"action\":\"" << katmer_security_action(flags) << "\",";
    out << "\"signals\":" << signals_json(signals);
    out << "}";
    return katmer_copy_cstr(out.str());
}
