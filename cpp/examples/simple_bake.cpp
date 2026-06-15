#include "katmer/katmer.h"
#include <cstring>
#include <iostream>

int main() {
    const char* doc = "<katmer module=\"http\" type=\"request\">GET /health</katmer>";
    const char* json = katmer_abi_bake_json("katmer", doc, std::strlen(doc));
    std::cout << json << "\n";
    katmer_free_string(json);
    return 0;
}
