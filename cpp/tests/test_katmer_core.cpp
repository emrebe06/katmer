#include "katmer.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

int main() {
    katmer_runtime* runtime = katmer_runtime_create();
    assert(runtime != nullptr);
    katmer_runtime_set_arena_size(runtime, 1024 * 256);
    katmer_stack* stack = katmer_stack_create_default(runtime);
    assert(stack != nullptr);
    assert(katmer_stack_layer_count(stack) >= 7);

    const char* clean =
        "<katmer module=\"http\" type=\"str\">\n"
        "GET /ping\n"
        "</katmer>";
    const char* clean_json = katmer_bake_json(stack, "katmer", clean, std::strlen(clean));
    assert(clean_json != nullptr);
    std::string clean_text = clean_json;
    assert(clean_text.find("\"ok\":true") != std::string::npos);
    assert(clean_text.find("\"module\":\"http\"") != std::string::npos);
    katmer_free_string(clean_json);

    const char* bad =
        "<katmer module=\"http\" type=\"str\">\n"
        "POST /x\n"
        "{\"q\":\"drop table users\"}\n"
        "</katmer>";
    const char* bad_json = katmer_bake_json(stack, "katmer", bad, std::strlen(bad));
    assert(bad_json != nullptr);
    std::string bad_text = bad_json;
    assert(bad_text.find("GUARD_BLOCK") != std::string::npos);
    assert(bad_text.find("sql_injection") != std::string::npos);
    katmer_free_string(bad_json);

    katmer_stack_destroy(stack);
    katmer_runtime_destroy(runtime);
    std::cout << "katmer core tests passed\n";
    return 0;
}
