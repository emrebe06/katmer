#ifndef KATMER_LAYER_H
#define KATMER_LAYER_H

#include "katmer_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef katmer_result (*katmer_layer_fn)(katmer_context* ctx);

KATMER_API katmer_result katmer_layer_parse(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_shape(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_validate(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_guard(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_select(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_output(katmer_context* ctx);
KATMER_API katmer_result katmer_layer_trace(katmer_context* ctx);

#ifdef __cplusplus
}
#endif

#endif
