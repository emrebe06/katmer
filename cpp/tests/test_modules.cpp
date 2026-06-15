#include "katmer/katmer.h"

#include <cassert>
#include <cstring>

int main() {
    const char* http = "<katmer module=\"http\" type=\"request\">POST /orders\n{\"id\":1}</katmer>";
    const char* http_json = katmer_abi_bake_json("katmer", http, std::strlen(http));
    assert(std::strstr(http_json, "\"select_kind\":\"http\""));
    assert(std::strstr(http_json, "POST /orders"));
    katmer_free_string(http_json);

    const char* dsp = "<katmer module=\"audio\" type=\"dsp.filter\">lowpass cutoff=1200 gain=-3</katmer>";
    const char* dsp_json = katmer_abi_bake_json("katmer", dsp, std::strlen(dsp));
    assert(std::strstr(dsp_json, "\"select_kind\":\"dsp\""));
    assert(std::strstr(dsp_json, "\"dsp.ready\":\"true\""));
    katmer_free_string(dsp_json);

    const char* modules = katmer_modules_json();
    assert(std::strstr(modules, "\"dsp\""));
    katmer_free_string(modules);
    return 0;
}
