#ifndef KATMER_PUBLIC_TRACE_H
#define KATMER_PUBLIC_TRACE_H

#include "katmer_string_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_trace_event {
    katmer_string_view layer;
    katmer_string_view code;
    katmer_string_view action;
    double time_ms;
    int ok;
} katmer_trace_event;

KATMER_API katmer_trace_event katmer_trace_event_make(const char* layer, const char* code, const char* action, double time_ms, int ok);
KATMER_API const char* katmer_trace_event_json(katmer_trace_event event);

#ifdef __cplusplus
}
#endif

#endif
