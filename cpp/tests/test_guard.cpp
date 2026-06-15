#include "katmer/katmer.h"
#include <cassert>
#include <cstring>

int main() {
    const char* bad = "<katmer module=\"http\">drop table users</katmer>";
    const char* json = katmer_abi_bake_json("katmer", bad, std::strlen(bad));
    assert(std::strstr(json, "\"ok\":false"));
    assert(std::strstr(json, "sql_injection"));
    katmer_free_string(json);
    unsigned int flags = katmer_security_scan_flags("cmd.exe && whoami", 17, 1024);
    assert(flags != KATMER_SECURITY_OK);
    return 0;
}
