#include "katmer_internal.h"
#include "katmer/katmer_abi.h"
#include "katmer/katmer_module.h"

katmer_abi_info katmer_abi_get_info(void) {
#if defined(_MSC_VER)
    const char* compiler = "msvc";
#elif defined(__clang__)
    const char* compiler = "clang";
#elif defined(__GNUC__)
    const char* compiler = "gcc";
#else
    const char* compiler = "unknown";
#endif
#if defined(NDEBUG)
    const char* build = "release";
#else
    const char* build = "debug";
#endif
    return katmer_abi_info{KATMER_ABI_VERSION_MAJOR, KATMER_ABI_VERSION_MINOR, KATMER_ABI_VERSION_PATCH, sizeof(void*), compiler, build};
}

const char* katmer_abi_info_json(void) {
    katmer_abi_info info = katmer_abi_get_info();
    std::ostringstream out;
    out << "{\"abi\":\"" << info.major << "." << info.minor << "." << info.patch << "\",";
    out << "\"pointer_size\":" << info.pointer_size << ",";
    out << "\"compiler\":\"" << info.compiler << "\",\"build\":\"" << info.build << "\"}";
    return katmer_copy_cstr(out.str());
}

const char* katmer_abi_bake_json(const char* input_type, const void* input_data, size_t input_size) {
    katmer_runtime* runtime = katmer_runtime_create();
    if (!runtime) return katmer_result_json(katmer_fail_status(KATMER_STATUS_OUT_OF_MEMORY, "RUNTIME_ALLOC", "runtime allocation failed"));
    katmer_stack* stack = katmer_stack_create_default(runtime);
    if (!stack) {
        katmer_runtime_destroy(runtime);
        return katmer_result_json(katmer_fail_status(KATMER_STATUS_OUT_OF_MEMORY, "STACK_ALLOC", "stack allocation failed"));
    }
    const char* json = katmer_bake_json(stack, input_type ? input_type : "raw", input_data, input_size);
    char* copy = katmer_copy_cstr(json ? json : "{}");
    katmer_free_string(json);
    katmer_stack_destroy(stack);
    katmer_runtime_destroy(runtime);
    return copy;
}

namespace {
const katmer_module_info kModules[] = {
    {"http", "HTTP request/response shaping and guard hints", 1},
    {"llm", "Prompt, tool, and agent payload shaping", 1},
    {"audio", "Audio conversion, DSP pipelines, and metadata payload shaping", 1},
    {"dsp", "Digital signal processing payload shaping for audio and sensor streams", 1},
    {"video", "Video conversion and stream job payload shaping", 1},
    {"android", "Mobile automation and device bridge payload shaping", 1}
};
}

size_t katmer_module_count(void) {
    return sizeof(kModules) / sizeof(kModules[0]);
}

katmer_module_info katmer_module_get(size_t index) {
    if (index >= katmer_module_count()) return katmer_module_info{"", "", 0};
    return kModules[index];
}

const char* katmer_modules_json(void) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < katmer_module_count(); ++i) {
        if (i) out << ",";
        katmer_module_info mod = katmer_module_get(i);
        out << "{\"name\":\"" << mod.name << "\",\"purpose\":\"" << katmer_json_escape(mod.purpose) << "\",\"builtin\":" << (mod.builtin ? "true" : "false") << "}";
    }
    out << "]";
    return katmer_copy_cstr(out.str());
}
