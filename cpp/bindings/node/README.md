# Katmer Node Binding

The Node binding uses `katmer_bake` when available and falls back to a lightweight JS parser.

```js
import { Katmer } from "./index.mjs";

const katmer = new Katmer({ executable: "../../../build/Release/katmer_bake.exe" });
console.log(katmer.bakeHttp("GET /health"));
console.log(katmer.bakeDsp("lowpass cutoff=1200 gain=-3"));
```
