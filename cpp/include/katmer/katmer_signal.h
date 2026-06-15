#ifndef KATMER_PUBLIC_SIGNAL_H
#define KATMER_PUBLIC_SIGNAL_H

#include "katmer_string_view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katmer_signal {
    katmer_string_view code;
    katmer_string_view message;
    double score;
} katmer_signal;

KATMER_API katmer_signal katmer_signal_make(const char* code, const char* message, double score);
KATMER_API const char* katmer_signal_json(katmer_signal signal);

#ifdef __cplusplus
}
#endif

#endif
