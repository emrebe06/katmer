#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_runtime* runtime = katmer_runtime_create();
    katmer_stack* stack = katmer_stack_create_default(runtime);
    assert(stack);
    assert(katmer_stack_layer_count(stack) >= 7);
    const char* input = "<katmer module=\"http\">GET /</katmer>";
    const char* json = katmer_bake_json(stack, "katmer", input, std::strlen(input));
    assert(std::strstr(json, "\"ok\":true"));
    katmer_free_string(json);
    katmer_stack_destroy(stack);
    katmer_runtime_destroy(runtime);
    return 0;
}
