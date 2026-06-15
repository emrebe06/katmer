#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    katmer_abi_info info = katmer_abi_get_info();
    assert(info.major == 0);
    const char* abi = katmer_abi_info_json();
    assert(std::strstr(abi, "\"abi\""));
    katmer_free_string(abi);
    const char* modules = katmer_modules_json();
    assert(std::strstr(modules, "\"http\""));
    assert(katmer_module_count() >= 5);
    katmer_free_string(modules);
    return 0;
}
