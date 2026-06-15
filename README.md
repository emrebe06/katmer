# Katmer

Katmer is a native C/C++ layered execution engine with a stable C ABI.

It receives input, bakes it through thin layers, collects signals, and returns a structured JSON result. Katmer is designed for language bindings, plugin backends, HTTP/DSP payload shaping, native worker layers, and future Python/JavaScript/Kotlin integration.

## Ecosystem

Katmer is the native layer in a small Python-first runtime ecosystem:

- [QuickAPI](https://github.com/emrebe06/QuickAPI): JSON-first Python API runtime.
- [SQLNocturne](https://github.com/emrebe06/sqlnocturne): safety-first SQL data runtime.

Katmer can run alone through its C ABI. In the ecosystem, it acts as the worker floor under higher-level Python APIs.

## Keywords

C ABI runtime, native execution engine, C++ layered runtime, Python native bridge, JavaScript native binding, Kotlin JNI runtime, plugin backend engine, DSP payload runtime, HTTP payload parser, arena allocator, JSON trace engine.

First MVP:

- C++ core
- stable C ABI
- built-in parse, shape, validate, guard, select, output, trace layers
- platform-aware heap allocator for Windows, Apple/macOS/iOS, Android, Linux, and POSIX
- per-bake arena allocator plus standalone arena API
- native buffer, string view, input, output, signal, trace, and JSON writer APIs
- structured JSON trace output through the C ABI
- `<katmer ...>...</katmer>` DSL parser
- Python ctypes binding
- browser/web JavaScript parser
- first binding stubs for JS native, Go, .NET, and Kotlin/JNI

## Build Native Core

Clone and build Katmer:

```bash
git clone https://github.com/emrebe06/katmer.git
cd katmer
cmake -S cpp -B build
cmake --build build --config Release
```

Windows output:

```text
build/Release/katmer_bake.exe
build/Release/katmer_core.dll
```

Linux/macOS output:

```text
build/katmer_bake
libkatmer_core.so or libkatmer_core.dylib
```

See [docs/INSTALL.md](docs/INSTALL.md) for platform-specific commands.

## Build From Existing Checkout

```bash
cmake -S cpp -B build
cmake --build build --config Release
```

## Quick Bake

```bash
build/Release/katmer_bake.exe katmer < examples/http_payload.katmer
build/Release/katmer_bake.exe katmer < examples/dsp_payload.katmer
```

Direct Windows example:

```bat
build\Release\katmer_bake.exe katmer "<katmer module=\"http\" type=\"request\">GET /health</katmer>"
```

Linux/macOS example:

```bash
./build/katmer_bake katmer '<katmer module="http" type="request">GET /health</katmer>'
```

## C API Shape

```c
#include <katmer/katmer.h>

katmer_runtime* runtime = katmer_runtime_create();
katmer_stack* stack = katmer_stack_create(runtime);

katmer_stack_use(stack, "parse", katmer_layer_parse);
katmer_stack_use(stack, "shape", katmer_layer_shape);
katmer_stack_use(stack, "validate", katmer_layer_validate);
katmer_stack_use(stack, "guard", katmer_layer_guard);
katmer_stack_use(stack, "select", katmer_layer_select);
katmer_stack_use(stack, "output", katmer_layer_output);
katmer_stack_use(stack, "trace", katmer_layer_trace);

const char* json = katmer_bake_json(stack, "katmer", input, input_size);
katmer_free_string(json);
katmer_stack_destroy(stack);
katmer_runtime_destroy(runtime);
```

Or use the default layer stack:

```c
katmer_stack* stack = katmer_stack_create_default(runtime);
```

For one-shot binding calls:

```c
const char* json = katmer_abi_bake_json("katmer", input, input_size);
katmer_free_string(json);
```

## Allocator Contract

Katmer's public heap allocator normalizes alignment and pairs allocation/free correctly per platform:

- Windows: `_aligned_malloc` / `_aligned_free`
- macOS and iOS: `posix_memalign` / `free`
- Android: `posix_memalign` / `free`
- Linux/POSIX: `posix_memalign` / `free`
- Unknown fallback: aligned wrapper over `malloc` / `free`

Bindings should release any string returned by Katmer with `katmer_free_string`.

## Source Layout

```text
cpp/
  CMakeLists.txt
  include/
    katmer/
      katmer.h
      katmer_abi.h
      katmer_types.h
      katmer_result.h
      katmer_error.h
      katmer_allocator.h
      katmer_arena.h
      katmer_buffer.h
      katmer_string_view.h
      katmer_json_writer.h
      katmer_input.h
      katmer_output.h
      katmer_signal.h
      katmer_trace.h
      katmer_context.h
      katmer_layer.h
      katmer_stack.h
      katmer_runtime.h
      katmer_module.h
  src/
    katmer_result.cpp
    katmer_error.cpp
    katmer_allocator.cpp
    katmer_arena.cpp
    katmer_buffer.cpp
    katmer_json_writer.cpp
    katmer_input.cpp
    katmer_output.cpp
    katmer_signal.cpp
    katmer_trace.cpp
    katmer_context.cpp
    katmer_layer.cpp
    katmer_stack.cpp
    katmer_runtime.cpp
    katmer_builtin_layers.cpp
    katmer_abi.cpp
  modules/
    http/
    llm/
    audio/
    video/
    android/
  tests/
    test_arena.cpp
    test_buffer.cpp
    test_json_writer.cpp
    test_stack.cpp
    test_guard.cpp
    test_trace.cpp
    test_abi.cpp
    test_katmer_core.cpp
bindings/
  python/
  web/
  js/
  go/
  dotnet/
  kotlin/
```

## DSL

```xml
<katmer module="http" type="str">
POST /checkout
Content-Type: application/json

{"product_id": 42}
</katmer>
```

## Python

```python
from katmer import Katmer

k = Katmer("build/Release/katmer_core.dll")
print(k.bake_http("GET /health"))
print(k.bake_dsp("lowpass cutoff=1200 gain=-3"))
```

## Node.js

```js
import { Katmer } from "./cpp/bindings/node/index.mjs";

const katmer = new Katmer({ executable: "build/Release/katmer_bake.exe" });
console.log(katmer.bakeHttp("GET /health"));
console.log(katmer.bakeDsp("lowpass cutoff=1200 gain=-3"));
```

## Kotlin

```kotlin
KatmerNative().use { katmer ->
    println(katmer.bakeHttp("GET /health"))
    println(katmer.bakeDsp("lowpass cutoff=1200 gain=-3"))
}
```

The Kotlin folder contains the JVM class and JNI bridge source; Android packaging comes next.
