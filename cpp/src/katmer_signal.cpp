#include "katmer_internal.h"
#include "katmer/katmer_signal.h"

katmer_signal katmer_signal_make(const char* code, const char* message, double score) {
    return katmer_signal{katmer_sv_from_cstr(code ? code : ""), katmer_sv_from_cstr(message ? message : ""), score};
}

const char* katmer_signal_json(katmer_signal signal) {
    std::string code(signal.code.data ? signal.code.data : "", signal.code.size);
    std::string message(signal.message.data ? signal.message.data : "", signal.message.size);
    std::ostringstream out;
    out << "{\"code\":\"" << katmer_json_escape(code) << "\",\"message\":\"" << katmer_json_escape(message) << "\",\"score\":" << signal.score << "}";
    return katmer_copy_cstr(out.str());
}
