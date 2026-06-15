#include "katmer_internal.h"
#include "katmer/katmer_error.h"

katmer_error katmer_error_make(katmer_status status, const char* code, const char* message, const char* detail) {
    return katmer_error{status, code ? code : "", message ? message : "", detail ? detail : ""};
}

const char* katmer_error_json(katmer_error error) {
    std::ostringstream out;
    out << "{";
    out << "\"status\":\"" << katmer_status_name(error.status) << "\",";
    out << "\"code\":\"" << katmer_json_escape(error.code ? error.code : "") << "\",";
    out << "\"message\":\"" << katmer_json_escape(error.message ? error.message : "") << "\",";
    out << "\"detail\":\"" << katmer_json_escape(error.detail ? error.detail : "") << "\"";
    out << "}";
    return katmer_copy_cstr(out.str());
}
