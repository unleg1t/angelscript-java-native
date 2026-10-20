#include "asjava.h"

#include <cstdio>

namespace asjava {

// ---------------------------------------------------------------------------
// UTF-8 <-> UTF-16 conversion
// ---------------------------------------------------------------------------

std::string ToUtf8(JNIEnv *env, jstring str) {
    if (str == nullptr) {
        return std::string();
    }
    const jsize length = env->GetStringLength(str);
    if (length == 0) {
        return std::string();
    }
    const jchar *chars = env->GetStringCritical(str, nullptr);
    if (chars == nullptr) {
        return std::string();
    }

    std::string result;
    result.reserve(static_cast<size_t>(length));
    for (jsize i = 0; i < length; ++i) {
        unsigned int cp = chars[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < length) {
            const unsigned int low = chars[i + 1];
            if (low >= 0xDC00 && low <= 0xDFFF) {
                cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                ++i;
            }
        }
        if (cp < 0x80) {
            result.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    env->ReleaseStringCritical(str, chars);
    return result;
}

jstring FromUtf8(JNIEnv *env, const char *str) {
    if (str == nullptr) {
        return nullptr;
    }
    return FromUtf8(env, std::string(str));
}

jstring FromUtf8(JNIEnv *env, const std::string &str) {
    if (str.empty()) {
        return env->NewStringUTF("");
    }

    std::vector<jchar> buffer;
    buffer.reserve(str.size());
    size_t i = 0;
    const size_t size = str.size();
    while (i < size) {
        const unsigned char c = static_cast<unsigned char>(str[i]);
        unsigned int cp;
        size_t extra;
        if (c < 0x80) {
            cp = c;
            extra = 0;
        } else if ((c & 0xE0) == 0xC0) {
            cp = c & 0x1F;
            extra = 1;
        } else if ((c & 0xF0) == 0xE0) {
            cp = c & 0x0F;
            extra = 2;
        } else if ((c & 0xF8) == 0xF0) {
            cp = c & 0x07;
            extra = 3;
        } else {
            cp = 0xFFFD;
            extra = 0;
        }
        ++i;
        for (size_t k = 0; k < extra && i < size; ++k, ++i) {
            cp = (cp << 6) | (static_cast<unsigned char>(str[i]) & 0x3F);
        }
        if (cp > 0xFFFF) {
            cp -= 0x10000;
            buffer.push_back(static_cast<jchar>(0xD800 + (cp >> 10)));
            buffer.push_back(static_cast<jchar>(0xDC00 + (cp & 0x3FF)));
        } else {
            buffer.push_back(static_cast<jchar>(cp));
        }
    }
    return env->NewString(buffer.data(), static_cast<jsize>(buffer.size()));
}

// ---------------------------------------------------------------------------
// Java exceptions
// ---------------------------------------------------------------------------

void ThrowJava(JNIEnv *env, const char *className, const std::string &message) {
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }
    jclass cls = env->FindClass(className);
    if (cls == nullptr) {
        env->ExceptionClear();
        cls = env->FindClass("java/lang/RuntimeException");
        if (cls == nullptr) {
            return;
        }
    }
    env->ThrowNew(cls, message.c_str());
    env->DeleteLocalRef(cls);
}

void TranslateJavaException(JNIEnv *env, asIScriptEngine *engine) {
    if (!env->ExceptionCheck()) {
        return;
    }

    std::string message;
    jthrowable exc = env->ExceptionOccurred();
    if (exc != nullptr) {
        env->ExceptionClear();
        jclass cls = env->GetObjectClass(exc);
        if (cls != nullptr) {
            jmethodID toString = env->GetMethodID(cls, "toString", "()Ljava/lang/String;");
            if (toString != nullptr) {
                jstring text = static_cast<jstring>(
                        env->CallObjectMethod(exc, toString));
                if (!env->ExceptionCheck() && text != nullptr) {
                    message = ToUtf8(env, text);
                    env->DeleteLocalRef(text);
                }
            }
            env->DeleteLocalRef(cls);
        }
        env->Throw(exc);  // rethrow so the caller can report it
        env->ExceptionClear();
    }

    if (message.empty()) {
        message = "Java exception in script callback";
    }

    if (engine != nullptr) {
        asIScriptContext *ctx = asGetActiveContext();
        if (ctx != nullptr) {
            ctx->SetException(message.c_str(), true);
        }
    }
}

// ---------------------------------------------------------------------------
// Callbacks into Java
// ---------------------------------------------------------------------------

asSFuncPtr MakeFuncPtr(void *fn) {
    asSFuncPtr p(2);
    p.ptr.f.func = reinterpret_cast<asFUNCTION_t>(reinterpret_cast<uintptr_t>(fn));
    return p;
}

asSFuncPtr MakeGenericFuncPtr(void *fn) {
    asSFuncPtr p(1);
    p.ptr.f.func = reinterpret_cast<asFUNCTION_t>(reinterpret_cast<uintptr_t>(fn));
    return p;
}

JavaCallback *CreateCallback(JNIEnv *env, jobject target, const char *methodName,
                              const char *methodSig) {
    if (target == nullptr) {
        return nullptr;
    }
    jclass cls = env->GetObjectClass(target);
    if (cls == nullptr) {
        return nullptr;
    }
    jmethodID method = env->GetMethodID(cls, methodName, methodSig);
    env->DeleteLocalRef(cls);
    if (method == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    JavaCallback *cb = new JavaCallback();
    cb->target = env->NewGlobalRef(target);
    cb->method = method;
    return cb;
}

void DestroyCallback(JNIEnv *env, JavaCallback *cb) {
    if (cb == nullptr) {
        return;
    }
    if (cb->target != nullptr) {
        env->DeleteGlobalRef(cb->target);
    }
    delete cb;
}

void GenericTrampoline(asIScriptGeneric *gen) {
    JavaCallback *cb = static_cast<JavaCallback *>(gen->GetAuxiliary());
    if (cb == nullptr) {
        return;
    }

    JNIEnv *env = AttachEnv(cb->vm);
    if (env == nullptr) {
        return;
    }

    // Build the Java side view of the generic context.
    jclass genericCls = FindClass(env, "org/angelscript/ScriptGeneric");
    if (genericCls == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }
    jmethodID factory = env->GetStaticMethodID(genericCls, "create",
                                               "(J)Lorg/angelscript/ScriptGeneric;");
    jobject wrapper = nullptr;
    if (factory != nullptr) {
        wrapper = env->CallStaticObjectMethod(genericCls, factory, reinterpret_cast<jlong>(gen));
    }
    env->DeleteLocalRef(genericCls);
    if (wrapper == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }

    jvalue values[1];
    values[0].l = wrapper;
    env->CallVoidMethodA(cb->target, cb->method, values);
    env->DeleteLocalRef(wrapper);

    if (env->ExceptionCheck()) {
        TranslateJavaException(env, gen->GetEngine());
    }
    DetachEnv(cb->vm);
}

void MessageTrampoline(asSMessageInfo *msg, void *param) {
    JavaCallback *cb = static_cast<JavaCallback *>(param);
    if (cb == nullptr) {
        return;
    }

    JNIEnv *env = AttachEnv(cb->vm);
    if (env == nullptr) {
        return;
    }

    jstring section = msg->section != nullptr ? FromUtf8(env, msg->section) : nullptr;
    jstring message = msg->message != nullptr ? FromUtf8(env, msg->message) : nullptr;
    jobject type = MakeMessageType(env, static_cast<int>(msg->type));

    jvalue values[5];
    values[0].l = section;
    values[1].i = msg->row;
    values[2].i = msg->col;
    values[3].l = type;
    values[4].l = message;
    env->CallVoidMethodA(cb->target, cb->method, values);

    if (section != nullptr) {
        env->DeleteLocalRef(section);
    }
    if (message != nullptr) {
        env->DeleteLocalRef(message);
    }
    if (type != nullptr) {
        env->DeleteLocalRef(type);
    }
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    DetachEnv(cb->vm);
}

void LineTrampoline(asIScriptContext *ctx, void *param) {
    JavaCallback *cb = static_cast<JavaCallback *>(param);
    if (cb == nullptr) {
        return;
    }

    JNIEnv *env = AttachEnv(cb->vm);
    if (env == nullptr) {
        return;
    }

    int line = 0;
    const char *section = nullptr;
    if (ctx != nullptr) {
        ctx->GetLineNumber(0, nullptr, &section);
        line = ctx->GetLineNumber(0);
    }

    jclass contextCls = FindClass(env, "org/angelscript/ScriptContext");
    if (contextCls == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }
    jmethodID factory = env->GetStaticMethodID(contextCls, "create",
                                               "(J)Lorg/angelscript/ScriptContext;");
    jobject wrapper = nullptr;
    if (factory != nullptr) {
        wrapper = env->CallStaticObjectMethod(contextCls, factory, reinterpret_cast<jlong>(ctx));
    }
    env->DeleteLocalRef(contextCls);
    if (wrapper == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }

    jvalue values[2];
    values[0].l = wrapper;
    values[1].i = line;
    env->CallVoidMethodA(cb->target, cb->method, values);
    env->DeleteLocalRef(wrapper);

    if (env->ExceptionCheck()) {
        TranslateJavaException(env, ctx != nullptr ? ctx->GetEngine() : nullptr);
    }
    DetachEnv(cb->vm);
}

void ExceptionTrampoline(asIScriptContext *ctx, void *param) {
    JavaCallback *cb = static_cast<JavaCallback *>(param);
    if (cb == nullptr) {
        return;
    }

    JNIEnv *env = AttachEnv(cb->vm);
    if (env == nullptr) {
        return;
    }

    jclass contextCls = FindClass(env, "org/angelscript/ScriptContext");
    if (contextCls == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }
    jmethodID ctor = env->GetMethodID(contextCls, "<init>", "(J)V");
    jobject wrapper = nullptr;
    if (ctor != nullptr) {
        wrapper = env->NewObject(contextCls, ctor, reinterpret_cast<jlong>(ctx));
    }
    env->DeleteLocalRef(contextCls);
    if (wrapper == nullptr) {
        env->ExceptionClear();
        DetachEnv(cb->vm);
        return;
    }

    jvalue values[1];
    values[0].l = wrapper;
    env->CallVoidMethodA(cb->target, cb->method, values);
    env->DeleteLocalRef(wrapper);

    if (env->ExceptionCheck()) {
        TranslateJavaException(env, ctx != nullptr ? ctx->GetEngine() : nullptr);
    }
    DetachEnv(cb->vm);
}

// ---------------------------------------------------------------------------
// Per engine state
// ---------------------------------------------------------------------------

namespace {

std::mutex &StateMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_map<asIScriptEngine *, EngineState *> &StateMap() {
    static std::unordered_map<asIScriptEngine *, EngineState *> map;
    return map;
}

}  // namespace

EngineState *GetEngineState(asIScriptEngine *engine) {
    std::lock_guard<std::mutex> lock(StateMutex());
    auto it = StateMap().find(engine);
    return it == StateMap().end() ? nullptr : it->second;
}

EngineState *CreateEngineState(asIScriptEngine *engine, JavaVM *vm) {
    std::lock_guard<std::mutex> lock(StateMutex());
    auto it = StateMap().find(engine);
    if (it != StateMap().end()) {
        return it->second;
    }
    EngineState *state = new EngineState();
    state->engine = engine;
    state->vm = vm;
    StateMap()[engine] = state;
    return state;
}

void DestroyEngineState(asIScriptEngine *engine) {
    EngineState *state = nullptr;
    {
        std::lock_guard<std::mutex> lock(StateMutex());
        auto it = StateMap().find(engine);
        if (it == StateMap().end()) {
            return;
        }
        state = it->second;
        StateMap().erase(it);
    }

    JNIEnv *env = AttachEnv(state->vm);
    for (JavaCallback *cb : state->callbacks) {
        DestroyCallback(env, cb);
    }
    if (state->messageCallback != nullptr) {
        DestroyCallback(env, state->messageCallback);
    }
    for (auto &entry : state->stringFactories) {
        if (entry.second != nullptr) {
            env->DeleteGlobalRef(entry.second);
        }
    }
    state->stringFactories.clear();
    for (const OwnedResource &resource : state->owned) {
        if (resource.deleter != nullptr) {
            resource.deleter(resource.ptr);
        }
    }
    if (env != nullptr) {
        DetachEnv(state->vm);
    }
    delete state;
}

// ---------------------------------------------------------------------------
// Per context callbacks
// ---------------------------------------------------------------------------

namespace {

struct ContextCallbacks {
    JavaCallback *line = nullptr;
    JavaCallback *exception = nullptr;
};

std::mutex &ContextMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_map<asIScriptContext *, ContextCallbacks *> &ContextMap() {
    static std::unordered_map<asIScriptContext *, ContextCallbacks *> map;
    return map;
}

}  // namespace

JavaCallback *GetContextCallback(asIScriptContext *ctx, int kind) {
    std::lock_guard<std::mutex> lock(ContextMutex());
    auto it = ContextMap().find(ctx);
    if (it == ContextMap().end()) {
        return nullptr;
    }
    return kind == CONTEXT_CALLBACK_LINE ? it->second->line : it->second->exception;
}

void SetContextCallback(asIScriptContext *ctx, int kind, JavaCallback *cb) {
    std::lock_guard<std::mutex> lock(ContextMutex());
    auto it = ContextMap().find(ctx);
    if (it == ContextMap().end()) {
        if (cb == nullptr) {
            return;
        }
        ContextCallbacks *callbacks = new ContextCallbacks();
        callbacks->line = nullptr;
        callbacks->exception = nullptr;
        it = ContextMap().emplace(ctx, callbacks).first;
    }
    if (kind == CONTEXT_CALLBACK_LINE) {
        it->second->line = cb;
    } else {
        it->second->exception = cb;
    }
}

void DestroyContextCallbacks(asIScriptContext *ctx) {
    ContextCallbacks *callbacks = nullptr;
    {
        std::lock_guard<std::mutex> lock(ContextMutex());
        auto it = ContextMap().find(ctx);
        if (it == ContextMap().end()) {
            return;
        }
        callbacks = it->second;
        ContextMap().erase(it);
    }

    JavaCallback *list[2] = {callbacks->line, callbacks->exception};
    for (JavaCallback *cb : list) {
        if (cb == nullptr) {
            continue;
        }
        JNIEnv *env = AttachEnv(cb->vm);
        DestroyCallback(env, cb);
        if (env != nullptr) {
            DetachEnv(cb->vm);
        }
    }
    delete callbacks;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

JNIEnv *AttachEnv(JavaVM *vm) {
    if (vm == nullptr) {
        return nullptr;
    }
    JNIEnv *env = nullptr;
    const jint status = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (status == JNI_OK) {
        return env;
    }
    if (status == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) == JNI_OK) {
            return env;
        }
    }
    return nullptr;
}

void DetachEnv(JavaVM *vm) {
    if (vm == nullptr) {
        return;
    }
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_EDETACHED) {
        vm->DetachCurrentThread();
    }
}

jclass FindClass(JNIEnv *env, const char *name) {
    // The returned reference is local and must be released with DeleteLocalRef.
    jclass local = env->FindClass(name);
    if (local == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    return local;
}

namespace {

asIStringFactory *GetFactory(asIScriptEngine *engine) {
    if (engine == nullptr) {
        return nullptr;
    }
    asDWORD flags = 0;
    asIStringFactory *factory = nullptr;
    engine->GetStringFactory(&flags, &factory);
    return factory;
}

}  // namespace

jstring ReadStringValue(JNIEnv *env, asIScriptEngine *engine, void *value) {
    if (value == nullptr) {
        return nullptr;
    }
    asIStringFactory *factory = GetFactory(engine);
    if (factory == nullptr) {
        return nullptr;
    }
    asUINT length = 0;
    if (factory->GetRawStringData(value, nullptr, &length) != asSUCCESS) {
        return nullptr;
    }
    std::vector<char> buffer(length + 1);
    if (length > 0) {
        asUINT actual = length;
        if (factory->GetRawStringData(value, buffer.data(), &actual) != asSUCCESS) {
            return nullptr;
        }
        length = actual;
    }
    return FromUtf8(env, std::string(buffer.data(), length));
}

const void *CreateStringConstant(asIScriptEngine *engine, const std::string &text) {
    asIStringFactory *factory = GetFactory(engine);
    if (factory == nullptr) {
        return nullptr;
    }
    return factory->GetStringConstant(text.c_str(), static_cast<asUINT>(text.size()));
}

namespace {

std::mutex &PendingMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_map<asIScriptContext *, std::vector<const void *>> &PendingStrings() {
    static std::unordered_map<asIScriptContext *, std::vector<const void *>> map;
    return map;
}

}  // namespace

void TrackPendingString(asIScriptContext *ctx, const void *constant) {
    if (ctx == nullptr || constant == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(PendingMutex());
    PendingStrings()[ctx].push_back(constant);
}

void ReleasePendingStrings(asIScriptContext *ctx) {    std::vector<const void *> constants;
    {
        std::lock_guard<std::mutex> lock(PendingMutex());
        auto it = PendingStrings().find(ctx);
        if (it == PendingStrings().end()) {
            return;
        }
        constants = it->second;
        PendingStrings().erase(it);
    }

    asIScriptEngine *engine = ctx != nullptr ? ctx->GetEngine() : nullptr;
    asIStringFactory *factory = GetFactory(engine);
    if (factory == nullptr) {
        return;
    }
    for (const void *constant : constants) {
        factory->ReleaseStringConstant(constant);
    }
}

jobject MakeMessageType(JNIEnv *env, int value) {    struct Cache {
        std::mutex mutex;
        jclass cls = nullptr;
        jobject values[3] = {nullptr, nullptr, nullptr};
    };
    static Cache cache;

    std::lock_guard<std::mutex> lock(cache.mutex);
    if (cache.cls == nullptr) {
        jclass local = env->FindClass("org/angelscript/MessageType");
        if (local == nullptr) {
            env->ExceptionClear();
            return nullptr;
        }
        cache.cls = static_cast<jclass>(env->NewGlobalRef(local));
        env->DeleteLocalRef(local);
    }
    if (cache.values[0] == nullptr) {
        static const char *names[3] = {"ERROR", "WARNING", "INFORMATION"};
        for (int i = 0; i < 3; ++i) {
            jfieldID field = env->GetStaticFieldID(cache.cls, names[i],
                                                  "Lorg/angelscript/MessageType;");
            if (field == nullptr) {
                env->ExceptionClear();
                continue;
            }
            jobject value = env->GetStaticObjectField(cache.cls, field);
            if (value == nullptr) {
                env->ExceptionClear();
                continue;
            }
            // The reference must be global, local references are released when the native method
            // returns.
            cache.values[i] = env->NewGlobalRef(value);
            env->DeleteLocalRef(value);
        }
    }
    const int index = value < 0 ? 0 : (value > 2 ? 2 : value);
    if (cache.values[index] == nullptr) {
        return nullptr;
    }
    return env->NewLocalRef(cache.values[index]);
}

jlong GetPtr(JNIEnv *env, jobject thiz) {    struct Cache {
        std::mutex mutex;
        std::unordered_map<jclass, jfieldID> fields;
    };
    static Cache cache;

    if (thiz == nullptr) {
        return 0;
    }
    jclass cls = env->GetObjectClass(thiz);
    if (cls == nullptr) {
        return 0;
    }
    jfieldID field = nullptr;
    {
        std::lock_guard<std::mutex> lock(cache.mutex);
        auto it = cache.fields.find(cls);
        if (it != cache.fields.end()) {
            field = it->second;
        } else {
            field = env->GetFieldID(cls, "ptr", "J");
            if (field == nullptr) {
                env->ExceptionClear();
            } else {
                cache.fields[cls] = field;
            }
        }
    }
    env->DeleteLocalRef(cls);
    if (field == nullptr) {
        return 0;
    }
    return env->GetLongField(thiz, field);
}

}  // namespace asjava
