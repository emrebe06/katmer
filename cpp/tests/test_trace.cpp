#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_trace_event event = katmer_trace_event_make("guard", "OK", "continue", 0.12, 1);
    const char* json = katmer_trace_event_json(event);
    assert(std::strstr(json, "\"layer\":\"guard\""));
    assert(std::strstr(json, "\"ok\":true"));
    katmer_free_string(json);
    katmer_signal signal = katmer_signal_make("shape", "body detected", 0.1);
    json = katmer_signal_json(signal);
    assert(std::strstr(json, "\"code\":\"shape\""));
    katmer_free_string(json);
    return 0;
}
