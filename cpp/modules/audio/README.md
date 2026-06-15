# Katmer Audio/DSP Module

Audio is the DSP-facing built-in Katmer module.

It is selected with either `module="audio"` or `module="dsp"`:

```xml
<katmer module="audio" type="dsp.pipeline">lowpass cutoff=1200 gain=-3</katmer>
```

The core returns:

- `result.module = "audio"` or `"dsp"`
- `result.select_kind = "dsp"`
- `result.select = requested DSP type`
- `store["dsp.ready"] = "true"`

This is the first native contract for future real DSP workers.
