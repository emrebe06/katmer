#include "katmer/katmer.h"

#include <iostream>
#include <iterator>
#include <string>

int main(int argc, char** argv) {
    const char* input_type = argc > 1 ? argv[1] : "katmer";
    std::string input;
    if (argc > 2) {
        input = argv[2];
    } else {
        input.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
    }

    const char* json = katmer_abi_bake_json(input_type, input.data(), input.size());
    if (!json) {
        std::cerr << "{\"ok\":false,\"engine\":\"katmer\",\"code\":\"NO_JSON\"}" << std::endl;
        return 2;
    }
    std::cout << json << std::endl;
    katmer_free_string(json);
    return 0;
}
