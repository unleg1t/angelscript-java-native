// Shared declarations for the AngelScript JNI bridge.
#ifndef ASJAVA_H
#define ASJAVA_H

#include <jni.h>

#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "angelscript.h"

namespace asjava {

// ---------------------------------------------------------------------------
// UTF-8 <-> UTF-16 conversion
// ---------------------------------------------------------------------------

std::string ToUtf8(JNIEnv *env, jstring str);
jstring FromUtf8(JNIEnv *env, const char *str);
jstring FromUtf8(JNIEnv *env, const std::string &str);

// ---------------------------------------------------------------------------
// Java exceptions
// ---------------------------------------------------------------------------

void ThrowJava(JNIEnv *env, const char *className, const std::string &message);

// Describes and clears a pending Java exception, then raises it as a script exception.
void TranslateJavaException(JNIEnv *env, asIScriptEngine *engine);

// ---------------------------------------------------------------------------
// Callbacks into Java
// ---------------------------------------------------------------------------

// A Java object with a single method that the script engine can call back into.
struct JavaCallback {
    JavaVM *vm = nullptr;
    jobject target = nullptr;  // global reference
    jmethodID method = nullptr;
    void *aux = nullptr;       // opaque payload handed back to the callback
};

JavaCallback *CreateCallback(JNIEnv *env, jobject target, const char *methodName,
                              const char *methodSig);
void DestroyCallback(JNIEnv *env, JavaCallback *cb);

// The trampolines registered with the engine.
void GenericTrampoline(asIScriptGeneric *gen);
void MessageTrampoline(asSMessageInfo *msg, void *param);
void LineTrampoline(asIScriptContext *ctx, void *param);
void ExceptionTrampoline(asIScriptContext *ctx, void *param);

// Creates an asSFuncPtr for an arbitrary C function pointer.
asSFuncPtr MakeFuncPtr(void *fn);

// Creates an asSFuncPtr for a function using the generic calling convention.
asSFuncPtr MakeGenericFuncPtr(void *fn);

// ---------------------------------------------------------------------------
// Per engine state
// ---------------------------------------------------------------------------

// A pointer owned by the engine state, released when the engine is shut down.
struct OwnedResource {
    void *ptr = nullptr;
    void (*deleter)(void *) = nullptr;
};

struct EngineState {
    asIScriptEngine *engine = nullptr;
    JavaVM *vm = nullptr;
    std::vector<JavaCallback *> callbacks;
    JavaCallback *messageCallback = nullptr;
    std::vector<OwnedResource> owned;
    // The Java string factories that were registered with the engine.
    std::unordered_map<void *, jobject> stringFactories;
};

EngineState *GetEngineState(asIScriptEngine *engine);
EngineState *CreateEngineState(asIScriptEngine *engine, JavaVM *vm);
void DestroyEngineState(asIScriptEngine *engine);

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Returns the JNIEnv for the current thread, attaching it if necessary.
JNIEnv *AttachEnv(JavaVM *vm);
void DetachEnv(JavaVM *vm);

// Looks up a Java class by its JNI name and returns a local reference.
jclass FindClass(JNIEnv *env, const char *name);

// Builds the org.angelscript.MessageType enum instance for the given native value.
jobject MakeMessageType(JNIEnv *env, int value);

// Reads an AngelScript string value into a Java string. The value must be the address of a
// string value, as returned by asIScriptGeneric::GetArgAddress.
jstring ReadStringValue(JNIEnv *env, asIScriptEngine *engine, void *value);

// Creates an AngelScript string constant from a Java string.
const void *CreateStringConstant(asIScriptEngine *engine, const std::string &text);

// Releases the string constants that were created for the arguments of a context.
void ReleasePendingStrings(asIScriptContext *ctx);

// Keeps a string constant alive until the context executes.
void TrackPendingString(asIScriptContext *ctx, const void *constant);

// Reads the "ptr" field of a Java wrapper object.
jlong GetPtr(JNIEnv *env, jobject thiz);

// Per context callback bookkeeping.
JavaCallback *GetContextCallback(asIScriptContext *ctx, int kind);
void SetContextCallback(asIScriptContext *ctx, int kind, JavaCallback *cb);
void DestroyContextCallbacks(asIScriptContext *ctx);

// Releases all Java objects and types registered with the given engine.
void DestroyJavaTypes(asIScriptEngine *engine);

// The kinds of per context callbacks.
enum ContextCallbackKind {
    CONTEXT_CALLBACK_LINE = 0,
    CONTEXT_CALLBACK_EXCEPTION = 1
};

// ---------------------------------------------------------------------------
// Bridges for Java implemented engine extensions
// ---------------------------------------------------------------------------

// Creates an asIStringFactory that forwards to a Java StringFactory. The caller owns the result.
asIStringFactory *CreateStringFactory(JNIEnv *env, jobject factory);

// Creates an asIBinaryStream that forwards to a Java BinaryStream. The caller owns the result.
asIBinaryStream *CreateBinaryStream(JNIEnv *env, jobject stream);

// Creates a string factory that forwards to another string factory, so that a factory that was
// registered by an add-on can be used from Java. The caller owns the result.
asIStringFactory *CreateStringFactoryProxy(asIStringFactory *target);

}  // namespace asjava

#endif  // ASJAVA_H
