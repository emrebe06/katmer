# Katmer Kotlin Binding

This folder contains the JVM-facing Kotlin class and JNI bridge skeleton for Android/JVM use.

Build the native JNI library against `katmer_core` and package it as `katmer_jni`.

```kotlin
KatmerNative().use { katmer ->
    println(katmer.bakeHttp("GET /health"))
    println(katmer.bakeDsp("lowpass cutoff=1200 gain=-3"))
}
```
