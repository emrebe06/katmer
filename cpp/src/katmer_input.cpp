#include "katmer_internal.h"
#include "katmer/katmer_input.h"

katmer_input katmer_input_make(const char* kind, const void* body, size_t bytes) {
    return katmer_input{katmer_sv_from_cstr(kind ? kind : "raw"), katmer_sv_from_parts(static_cast<const char*>(body), bytes), bytes};
}

const char* katmer_input_json(katmer_input input) {
    std::string kind(input.kind.data ? input.kind.data : "", input.kind.size);
    std::string body(input.body.data ? input.body.data : "", input.body.size);
    std::ostringstream out;
    out << "{\"kind\":\"" << katmer_json_escape(kind) << "\",\"bytes\":" << input.bytes;
    out << ",\"preview\":\"" << katmer_json_escape(body.substr(0, 160)) << "\"}";
    return katmer_copy_cstr(out.str());
}
