#include "katmer_internal.h"
#include "katmer/katmer_output.h"

katmer_output katmer_output_make(int ok, const char* kind, const char* body) {
    return katmer_output{ok, katmer_sv_from_cstr(kind ? kind : "text"), katmer_sv_from_cstr(body ? body : "")};
}

const char* katmer_output_json(katmer_output output) {
    std::string kind(output.kind.data ? output.kind.data : "", output.kind.size);
    std::string body(output.body.data ? output.body.data : "", output.body.size);
    std::ostringstream out;
    out << "{\"ok\":" << (output.ok ? "true" : "false") << ",\"kind\":\"" << katmer_json_escape(kind) << "\",";
    out << "\"body\":\"" << katmer_json_escape(body) << "\"}";
    return katmer_copy_cstr(out.str());
}
