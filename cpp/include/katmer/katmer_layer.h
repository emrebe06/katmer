#ifndef KATMER_PUBLIC_LAYER_H
#define KATMER_PUBLIC_LAYER_H
#include "../katmer_layer.h"
#ifdef __cplusplus
extern "C" {
#endif
KATMER_API katmer_layer_fn katmer_builtin_layer_by_name(const char* name);
KATMER_API const char* katmer_builtin_layers_json(void);
#ifdef __cplusplus
}
#endif
#endif
