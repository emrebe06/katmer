#include "katmer/katmer.h"
#include <cstring>
#include <iostream>

static katmer_result stamp_layer(katmer_context* ctx) {
    katmer_context_set(ctx, "custom.layer", "stamp");
    return katmer_continue("STAMP", "custom layer executed");
}

int main() {
    katmer_runtime* runtime = katmer_runtime_create();
    katmer_stack* stack = katmer_stack_create(runtime);
    katmer_stack_use(stack, "parse", katmer_layer_parse);
    katmer_stack_use(stack, "stamp", stamp_layer);
    katmer_stack_use(stack, "output", katmer_layer_output);
    const char* input = "<katmer module=\"llm\">hello</katmer>";
    const char* json = katmer_bake_json(stack, "katmer", input, std::strlen(input));
    std::cout << json << "\n";
    katmer_free_string(json);
    katmer_stack_destroy(stack);
    katmer_runtime_destroy(runtime);
    return 0;
}
