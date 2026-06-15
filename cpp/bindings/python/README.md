# Katmer Python Binding

This binding calls the Katmer C ABI with `ctypes`.

```python
from katmer import Katmer

k = Katmer(r"..\..\..\build\Release\katmer_core.dll")
print(k.bake_http("GET /health"))
print(k.bake_dsp("lowpass cutoff=1200 gain=-3"))
```

Returned strings allocated by the native core are released with `katmer_free_string`.
