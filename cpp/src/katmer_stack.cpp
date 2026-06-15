#include "katmer_internal.h"

namespace {
thread_local std::string tls_last_context_json;
}

katmer_stack* katmer_stack_create(katmer_runtime* runtime) {
    if (!runtime) return nullptr;
    katmer_stack* stack = new katmer_stack();
    stack->runtime = runtime;
    return stack;
}

katmer_stack* katmer_stack_create_default(katmer_runtime* runtime) {
    katmer_stack* stack = katmer_stack_create(runtime);
    if (!stack) return nullptr;
    katmer_stack_use(stack, "parse", katmer_layer_parse);
    katmer_stack_use(stack, "shape", katmer_layer_shape);
    katmer_stack_use(stack, "validate", katmer_layer_validate);
    katmer_stack_use(stack, "guard", katmer_layer_guard);
    katmer_stack_use(stack, "select", katmer_layer_select);
    katmer_stack_use(stack, "output", katmer_layer_output);
    katmer_stack_use(stack, "trace", katmer_layer_trace);
    return stack;
}

void katmer_stack_destroy(katmer_stack* stack) {
    delete stack;
}

int katmer_stack_use(katmer_stack* stack, const char* layer_name, katmer_layer_fn fn) {
    if (!stack || !layer_name || !fn) return 0;
    stack->layers.push_back(KatmerLayer{layer_name, fn});
    return 1;
}

size_t katmer_stack_layer_count(katmer_stack* stack) {
    return stack ? stack->layers.size() : 0;
}

katmer_result katmer_bake(katmer_stack* stack, const char* input_type, const void* input_data, size_t input_size) {
    if (!stack || !stack->runtime || !input_data) {
        tls_last_context_json = "{\"ok\":false,\"engine\":\"katmer\",\"code\":\"KATMER_INVALID_ARGUMENT\"}";
        return katmer_fail_status(KATMER_STATUS_INVALID_ARGUMENT, "KATMER_INVALID_ARGUMENT", "Invalid stack or input");
    }
    stack->runtime->bake_count += 1;

    katmer_context ctx;
    ctx.runtime = stack->runtime;
    ctx.input_type = input_type ? input_type : "bytes";
    ctx.input_text.assign(static_cast<const char*>(input_data), input_size);
    ctx.trace_id = "ktr_" + std::to_string(stack->runtime->bake_count) + "_" + std::to_string(katmer_now_ns());
    if (!ctx.arena.create(stack->runtime->default_arena_size)) {
        katmer_result oom = katmer_fail_status(KATMER_STATUS_OUT_OF_MEMORY, "ARENA_CREATE_FAILED", "Could not allocate Katmer arena");
        tls_last_context_json = katmer_context_json(ctx, oom);
        return oom;
    }

    katmer_result last = katmer_continue("OK", "Baked");
    for (const KatmerLayer& layer : stack->layers) {
        auto started = std::chrono::high_resolution_clock::now();
        katmer_result result = layer.fn(&ctx);
        auto ended = std::chrono::high_resolution_clock::now();
        double time_ms = std::chrono::duration<double, std::milli>(ended - started).count();
        ctx.trace.push_back(KatmerLayerTrace{
            layer.name,
            result.ok,
            result.code ? result.code : "",
            result.message ? result.message : "",
            katmer_action_name(result.action),
            time_ms
        });
        last = result;
        if (!result.ok || result.action == KATMER_FAIL) {
            ctx.failed = true;
            break;
        }
        if (result.action == KATMER_STOP) {
            ctx.stopped = true;
            break;
        }
    }

    tls_last_context_json = katmer_context_json(ctx, last);
    return last;
}

const char* katmer_bake_json(katmer_stack* stack, const char* input_type, const void* input_data, size_t input_size) {
    katmer_result result = katmer_bake(stack, input_type, input_data, input_size);
    (void) result;
    return katmer_copy_cstr(tls_last_context_json);
}
