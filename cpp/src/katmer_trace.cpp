#include "katmer_internal.h"
#include "katmer/katmer_trace.h"

katmer_trace_event katmer_trace_event_make(const char* layer, const char* code, const char* action, double time_ms, int ok) {
    return katmer_trace_event{katmer_sv_from_cstr(layer ? layer : ""), katmer_sv_from_cstr(code ? code : ""), katmer_sv_from_cstr(action ? action : ""), time_ms, ok};
}

const char* katmer_trace_event_json(katmer_trace_event event) {
    std::string layer(event.layer.data ? event.layer.data : "", event.layer.size);
    std::string code(event.code.data ? event.code.data : "", event.code.size);
    std::string action(event.action.data ? event.action.data : "", event.action.size);
    std::ostringstream out;
    out << "{\"layer\":\"" << katmer_json_escape(layer) << "\",\"code\":\"" << katmer_json_escape(code) << "\",";
    out << "\"action\":\"" << katmer_json_escape(action) << "\",\"time_ms\":" << event.time_ms << ",\"ok\":" << (event.ok ? "true" : "false") << "}";
    return katmer_copy_cstr(out.str());
}
