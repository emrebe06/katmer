# Katmer HTTP Module

HTTP is a built-in Katmer module.

It is selected with:

```xml
<katmer module="http" type="request">GET /health</katmer>
```

The core returns:

- `result.module = "http"`
- `result.select_kind = "http"`
- `result.select = first request line`
- `result.guard = allow | observe | block`

This module is ready for Python, Node, and Kotlin bindings through `katmer_abi_bake_json`.
