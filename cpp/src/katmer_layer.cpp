#include "katmer_internal.h"
#include "katmer/katmer_layer.h"

katmer_layer_fn katmer_builtin_layer_by_name(const char* name) {
    std::string key = katmer_lower(name ? name : "");
    if (key == "parse") return katmer_layer_parse;
    if (key == "shape") return katmer_layer_shape;
    if (key == "validate") return katmer_layer_validate;
    if (key == "guard") return katmer_layer_guard;
    if (key == "select") return katmer_layer_select;
    if (key == "output") return katmer_layer_output;
    if (key == "trace") return katmer_layer_trace;
    return nullptr;
}

const char* katmer_builtin_layers_json(void) {
    return katmer_copy_cstr("[\"parse\",\"shape\",\"validate\",\"guard\",\"select\",\"output\",\"trace\"]");
}
