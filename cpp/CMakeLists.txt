cmake_minimum_required(VERSION 3.16)
project(katmer_core VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(katmer_core SHARED
    src/katmer_result.cpp
    src/katmer_error.cpp
    src/katmer_allocator.cpp
    src/katmer_arena.cpp
    src/katmer_buffer.cpp
    src/katmer_context.cpp
    src/katmer_dsl.cpp
    src/katmer_json.cpp
    src/katmer_json_writer.cpp
    src/katmer_input.cpp
    src/katmer_output.cpp
    src/katmer_signal.cpp
    src/katmer_trace.cpp
    src/katmer_layer.cpp
    src/katmer_security.cpp
    src/katmer_stack.cpp
    src/katmer_runtime.cpp
    src/katmer_builtin_layers.cpp
    src/katmer_abi.cpp
)

target_include_directories(katmer_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(katmer_core PRIVATE KATMER_BUILD)

add_executable(katmer_smoke
    examples/katmer_smoke.cpp
)

target_link_libraries(katmer_smoke PRIVATE katmer_core)

add_executable(simple_bake
    examples/simple_bake.cpp
)

target_link_libraries(simple_bake PRIVATE katmer_core)

add_executable(custom_layer
    examples/custom_layer.cpp
)

target_link_libraries(custom_layer PRIVATE katmer_core)

add_executable(katmer_bake
    examples/katmer_bake.cpp
)

target_link_libraries(katmer_bake PRIVATE katmer_core)

add_executable(test_katmer_core
    tests/test_katmer_core.cpp
)

target_link_libraries(test_katmer_core PRIVATE katmer_core)

function(katmer_add_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE katmer_core)
endfunction()

katmer_add_test(test_arena tests/test_arena.cpp)
katmer_add_test(test_buffer tests/test_buffer.cpp)
katmer_add_test(test_json_writer tests/test_json_writer.cpp)
katmer_add_test(test_stack tests/test_stack.cpp)
katmer_add_test(test_guard tests/test_guard.cpp)
katmer_add_test(test_trace tests/test_trace.cpp)
katmer_add_test(test_abi tests/test_abi.cpp)
katmer_add_test(test_modules tests/test_modules.cpp)
