#include "katmer/katmer.h"

#include <jni.h>
#include <string>

namespace {
jstring copy_json(JNIEnv* env, const char* json) {
    if (!json) return env->NewStringUTF("{}");
    jstring out = env->NewStringUTF(json);
    katmer_free_string(json);
    return out;
}

std::string jstring_to_utf8(JNIEnv* env, jstring value) {
    if (!value) return "";
    const char* chars = env->GetStringUTFChars(value, nullptr);
    if (!chars) return "";
    std::string out(chars);
    env->ReleaseStringUTFChars(value, chars);
    return out;
}
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_katmer_KatmerNative_nativeBake(JNIEnv* env, jobject, jstring inputType, jstring input) {
    std::string type = jstring_to_utf8(env, inputType);
    std::string body = jstring_to_utf8(env, input);
    return copy_json(env, katmer_abi_bake_json(type.c_str(), body.data(), body.size()));
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_katmer_KatmerNative_nativeAbi(JNIEnv* env, jobject) {
    return copy_json(env, katmer_abi_info_json());
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_katmer_KatmerNative_nativeModules(JNIEnv* env, jobject) {
    return copy_json(env, katmer_modules_json());
}
