#ifndef KATMER_RESULT_H
#define KATMER_RESULT_H

#if defined(_WIN32)
#  if defined(KATMER_BUILD_STATIC)
#    define KATMER_API
#  elif defined(KATMER_BUILD)
#    define KATMER_API __declspec(dllexport)
#  else
#    define KATMER_API __declspec(dllimport)
#  endif
#else
#  define KATMER_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum katmer_action {
    KATMER_CONTINUE = 0,
    KATMER_STOP = 1,
    KATMER_FAIL = 2
} katmer_action;

typedef enum katmer_status {
    KATMER_STATUS_OK = 0,
    KATMER_STATUS_INVALID_ARGUMENT = 1,
    KATMER_STATUS_OUT_OF_MEMORY = 2,
    KATMER_STATUS_LAYER_FAILED = 3,
    KATMER_STATUS_GUARD_BLOCKED = 4,
    KATMER_STATUS_PARSE_ERROR = 5,
    KATMER_STATUS_INTERNAL = 99
} katmer_status;

typedef struct katmer_result {
    int ok;
    katmer_action action;
    katmer_status status;
    const char* code;
    const char* message;
} katmer_result;

KATMER_API katmer_result katmer_continue(const char* code, const char* message);
KATMER_API katmer_result katmer_stop(const char* code, const char* message);
KATMER_API katmer_result katmer_fail(const char* code, const char* message);
KATMER_API katmer_result katmer_fail_status(katmer_status status, const char* code, const char* message);
KATMER_API const char* katmer_action_name(katmer_action action);
KATMER_API const char* katmer_status_name(katmer_status status);
KATMER_API const char* katmer_result_json(katmer_result result);
KATMER_API void katmer_free_string(const char* value);

#ifdef __cplusplus
}
#endif

#endif
