package io.katmer

class KatmerNative : AutoCloseable {
    fun bake(input: String, inputType: String = "katmer"): String = nativeBake(inputType, input)

    fun bakeHttp(payload: String, type: String = "request"): String {
        return bake("""<katmer module="http" type="$type">$payload</katmer>""")
    }

    fun bakeDsp(payload: String, type: String = "dsp.pipeline"): String {
        return bake("""<katmer module="audio" type="$type">$payload</katmer>""")
    }

    fun abi(): String = nativeAbi()

    fun modules(): String = nativeModules()

    override fun close() = Unit

    private external fun nativeBake(inputType: String, input: String): String
    private external fun nativeAbi(): String
    private external fun nativeModules(): String

    companion object {
        init {
            System.loadLibrary("katmer_jni")
        }
    }
}
