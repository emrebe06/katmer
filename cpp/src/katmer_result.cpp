#include "katmer_internal.h"

const char* katmer_action_name(katmer_action action) {
    switch (action) {
    case KATMER_CONTINUE: return "continue";
    case KATMER_STOP: return "stop";
    case KATMER_FAIL: return "fail";
    default: return "unknown";
    }
}

const char* katmer_status_name(katmer_status status) {
    switch (status) {
    case KATMER_STATUS_OK: return "OK";
    case KATMER_STATUS_INVALID_ARGUMENT: return "INVALID_ARGUMENT";
    case KATMER_STATUS_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
    case KATMER_STATUS_LAYER_FAILED: return "LAYER_FAILED";
    case KATMER_STATUS_GUARD_BLOCKED: return "GUARD_BLOCKED";
    case KATMER_STATUS_PARSE_ERROR: return "PARSE_ERROR";
    case KATMER_STATUS_INTERNAL: return "INTERNAL";
    default: return "UNKNOWN";
    }
}

katmer_result katmer_continue(const char* code, const char* message) {
    return katmer_result{1, KATMER_CONTINUE, KATMER_STATUS_OK, code ? code : "OK", message ? message : ""};
}

katmer_result katmer_stop(const char* code, const char* message) {
    return katmer_result{1, KATMER_STOP, KATMER_STATUS_OK, code ? code : "STOP", message ? message : ""};
}

katmer_result katmer_fail(const char* code, const char* message) {
    return katmer_fail_status(KATMER_STATUS_LAYER_FAILED, code ? code : "FAIL", message ? message : "");
}

katmer_result katmer_fail_status(katmer_status status, const char* code, const char* message) {
    return katmer_result{0, KATMER_FAIL, status, code ? code : katmer_status_name(status), message ? message : ""};
}

const char* katmer_result_json(katmer_result result) {
    std::ostringstream out;
    out << "{";
    out << "\"ok\":" << (result.ok ? "true" : "false") << ",";
    out << "\"action\":\"" << katmer_action_name(result.action) << "\",";
    out << "\"status\":\"" << katmer_status_name(result.status) << "\",";
    out << "\"code\":\"" << katmer_json_escape(result.code ? result.code : "") << "\",";
    out << "\"message\":\"" << katmer_json_escape(result.message ? result.message : "") << "\"";
    out << "}";
    return katmer_copy_cstr(out.str());
}

void katmer_free_string(const char* value) {
    std::free(const_cast<char*>(value));
}
