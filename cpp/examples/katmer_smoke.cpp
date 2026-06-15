#include "katmer.h"

#include <cstring>
#include <iostream>

int main() {
    const char* input =
        "<katmer module=\"http\" type=\"str\">\n"
        "POST /checkout\n"
        "Content-Type: application/json\n\n"
        "{\"product_id\":42}\n"
        "</katmer>";

    katmer_runtime* runtime = katmer_runtime_create();
    katmer_stack* stack = katmer_stack_create_default(runtime);

    const char* json = katmer_bake_json(stack, "katmer", input, std::strlen(input));
    std::cout << json << std::endl;
    katmer_free_string(json);

    katmer_stack_destroy(stack);
    katmer_runtime_destroy(runtime);
    return 0;
}
