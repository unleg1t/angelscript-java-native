// JNI bindings for org.angelscript.NativeMemory.
#include "asjava.h"

#include <cstdlib>
#include <cstring>

using namespace asjava;

extern "C" {

JNIEXPORT jlong JNICALL Java_org_angelscript_NativeMemory_allocate(JNIEnv *env, jclass,
                                                                    jint size) {
    (void)env;
    if (size <= 0) {
        return 0;
    }
    void *memory = std::calloc(1, static_cast<size_t>(size));
    return reinterpret_cast<jlong>(memory);
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_free(JNIEnv *env, jclass,
                                                               jlong address) {
    (void)env;
    if (address != 0) {
        std::free(reinterpret_cast<void *>(static_cast<intptr_t>(address)));
    }
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setByte(JNIEnv *env, jclass,
                                                                  jlong address, jbyte value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<signed char *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jbyte JNICALL Java_org_angelscript_NativeMemory_getByte(JNIEnv *env, jclass,
                                                                    jlong address) {
    (void)env;
    if (address == 0) {
        return 0;
    }
    return *reinterpret_cast<signed char *>(static_cast<intptr_t>(address));
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setShort(JNIEnv *env, jclass,
                                                                   jlong address, jshort value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<short *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jshort JNICALL Java_org_angelscript_NativeMemory_getShort(JNIEnv *env, jclass,
                                                                     jlong address) {
    (void)env;
    if (address == 0) {
        return 0;
    }
    return *reinterpret_cast<short *>(static_cast<intptr_t>(address));
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setInt(JNIEnv *env, jclass,
                                                                 jlong address, jint value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<int *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jint JNICALL Java_org_angelscript_NativeMemory_getInt(JNIEnv *env, jclass,
                                                                 jlong address) {
    (void)env;
    if (address == 0) {
        return 0;
    }
    return *reinterpret_cast<int *>(static_cast<intptr_t>(address));
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setLong(JNIEnv *env, jclass,
                                                                  jlong address, jlong value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<long long *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jlong JNICALL Java_org_angelscript_NativeMemory_getLong(JNIEnv *env, jclass,
                                                                   jlong address) {
    (void)env;
    if (address == 0) {
        return 0;
    }
    return *reinterpret_cast<long long *>(static_cast<intptr_t>(address));
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setFloat(JNIEnv *env, jclass,
                                                                   jlong address, jfloat value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<float *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jfloat JNICALL Java_org_angelscript_NativeMemory_getFloat(JNIEnv *env, jclass,
                                                                    jlong address) {
    (void)env;
    if (address == 0) {
        return 0.0f;
    }
    return *reinterpret_cast<float *>(static_cast<intptr_t>(address));
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeMemory_setDouble(JNIEnv *env, jclass,
                                                                    jlong address,
                                                                    jdouble value) {
    (void)env;
    if (address != 0) {
        *reinterpret_cast<double *>(static_cast<intptr_t>(address)) = value;
    }
}

JNIEXPORT jdouble JNICALL Java_org_angelscript_NativeMemory_getDouble(JNIEnv *env, jclass,
                                                                        jlong address) {
    (void)env;
    if (address == 0) {
        return 0.0;
    }
    return *reinterpret_cast<double *>(static_cast<intptr_t>(address));
}

}  // extern "C"
