// JNI bindings for org.angelscript.ScriptContext (asIScriptContext).
#include "asjava.h"

using namespace asjava;

namespace {

asIScriptContext *ContextOf(jlong ptr) {
    return reinterpret_cast<asIScriptContext *>(ptr);
}

}  // namespace

extern "C" {

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeAddRef(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : ctx->AddRef();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeRelease(JNIEnv *env,
                                                                        jobject thiz) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    DestroyContextCallbacks(ctx);
    return ctx->Release();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetEngine(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : reinterpret_cast<jlong>(ctx->GetEngine());
}

// ------------------------------------------------------------------ execution

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativePrepare(JNIEnv *env, jobject thiz,
                                                                        jlong func) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->Prepare(reinterpret_cast<asIScriptFunction *>(func));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeUnprepare(JNIEnv *env,
                                                                          jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->Unprepare();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeExecute(JNIEnv *env,
                                                                        jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    const int r = ctx->Execute();
    ReleasePendingStrings(ctx);
    return r;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeAbort(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->Abort();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSuspend(JNIEnv *env,
                                                                         jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->Suspend();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetState(JNIEnv *env,
                                                                         jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : static_cast<jint>(ctx->GetState());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativePushState(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->PushState();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativePopState(JNIEnv *env,
                                                                          jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->PopState();
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptContext_nativeIsNested(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx != nullptr && ctx->IsNested() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetNestCount(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    asUINT count = 0;
    ctx->IsNested(&count);
    return static_cast<jint>(count);
}

// ------------------------------------------------------------------ object

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetObject(JNIEnv *env, jobject thiz,
                                                                          jlong obj) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->SetObject(reinterpret_cast<void *>(static_cast<intptr_t>(obj)));
}

// ------------------------------------------------------------------ arguments

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgByte(JNIEnv *env,
                                                                            jobject thiz, jint arg,
                                                                            jbyte value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgByte(arg, value);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgWord(JNIEnv *env,
                                                                            jobject thiz, jint arg,
                                                                            jshort value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgWord(arg, value);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgDWord(JNIEnv *env,
                                                                             jobject thiz, jint arg,
                                                                             jint value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgDWord(arg, static_cast<asDWORD>(value));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgQWord(JNIEnv *env,
                                                                             jobject thiz, jint arg,
                                                                             jlong value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgQWord(arg, static_cast<asQWORD>(value));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgFloat(JNIEnv *env,
                                                                            jobject thiz, jint arg,
                                                                            jfloat value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgFloat(arg, value);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgDouble(JNIEnv *env,
                                                                             jobject thiz, jint arg,
                                                                             jdouble value) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->SetArgDouble(arg, value);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgAddress(JNIEnv *env,
                                                                              jobject thiz, jint arg,
                                                                              jlong addr) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->SetArgAddress(arg, reinterpret_cast<void *>(static_cast<intptr_t>(addr)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgObject(JNIEnv *env,
                                                                             jobject thiz, jint arg,
                                                                             jlong obj) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->SetArgObject(arg, reinterpret_cast<void *>(static_cast<intptr_t>(obj)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgVarType(JNIEnv *env,
                                                                              jobject thiz, jint arg,
                                                                              jlong ptr, jint typeId) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->SetArgVarType(arg, reinterpret_cast<void *>(static_cast<intptr_t>(ptr)), typeId);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetArgString(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint arg,
                                                                            jstring value) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    const std::string text = ToUtf8(env, value);
    const void *constant = CreateStringConstant(ctx->GetEngine(), text);
    if (constant == nullptr) {
        return asINVALID_TYPE;
    }
    TrackPendingString(ctx, constant);
    return ctx->SetArgAddress(arg, const_cast<void *>(constant));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetAddressOfArg(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jint arg) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetAddressOfArg(arg));
}

// ------------------------------------------------------------------ return value

JNIEXPORT jbyte JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnByte(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : ctx->GetReturnByte();
}

JNIEXPORT jshort JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnWord(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : ctx->GetReturnWord();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnDWord(JNIEnv *env,
                                                                               jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : static_cast<jint>(ctx->GetReturnDWord());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnQWord(JNIEnv *env,
                                                                               jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : static_cast<jlong>(ctx->GetReturnQWord());
}

JNIEXPORT jfloat JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnFloat(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0.0f : ctx->GetReturnFloat();
}

JNIEXPORT jdouble JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnDouble(JNIEnv *env,
                                                                                  jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0.0 : ctx->GetReturnDouble();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnAddress(JNIEnv *env,
                                                                                  jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetReturnAddress());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetReturnObject(JNIEnv *env,
                                                                                  jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetReturnObject());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetAddressOfReturnValue(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetAddressOfReturnValue());
}

// ------------------------------------------------------------------ exceptions

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeSetException(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jstring info,
                                                                             jboolean allowCatch) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    const std::string message = ToUtf8(env, info);
    return ctx->SetException(message.c_str(), allowCatch == JNI_TRUE);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetExceptionLineNumber(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    int column = 0;
    const char *section = nullptr;
    return ctx->GetExceptionLineNumber(&column, &section);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptContext_nativeGetExceptionSection(
        JNIEnv *env, jobject thiz) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return nullptr;
    }
    int column = 0;
    const char *section = nullptr;
    ctx->GetExceptionLineNumber(&column, &section);
    return section == nullptr ? nullptr : FromUtf8(env, section);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetExceptionFunction(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetExceptionFunction());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptContext_nativeGetExceptionString(
        JNIEnv *env, jobject thiz) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return nullptr;
    }
    const char *message = ctx->GetExceptionString();
    return message == nullptr ? nullptr : FromUtf8(env, message);
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptContext_nativeWillExceptionBeCaught(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx != nullptr && ctx->WillExceptionBeCaught() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_org_angelscript_ScriptContext_nativeSetExceptionCallback(
        JNIEnv *env, jobject thiz, jobject listener) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return;
    }
    JavaCallback *previous = GetContextCallback(ctx, CONTEXT_CALLBACK_EXCEPTION);
    if (previous != nullptr) {
        ctx->ClearExceptionCallback();
        DestroyCallback(env, previous);
        SetContextCallback(ctx, CONTEXT_CALLBACK_EXCEPTION, nullptr);
    }
    if (listener == nullptr) {
        return;
    }

    JavaVM *vm = nullptr;
    env->GetJavaVM(&vm);
    JavaCallback *cb = CreateCallback(env, listener, "onException",
                                       "(Lorg/angelscript/ScriptContext;)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The exception listener must implement onException(ScriptContext)");
        return;
    }
    cb->vm = vm;
    SetContextCallback(ctx, CONTEXT_CALLBACK_EXCEPTION, cb);
    ctx->SetExceptionCallback(MakeFuncPtr(reinterpret_cast<void *>(&ExceptionTrampoline)), cb,
                              asCALL_CDECL_OBJLAST);
}

JNIEXPORT void JNICALL Java_org_angelscript_ScriptContext_nativeClearExceptionCallback(
        JNIEnv *env, jobject thiz) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return;
    }
    JavaCallback *cb = GetContextCallback(ctx, CONTEXT_CALLBACK_EXCEPTION);
    if (cb == nullptr) {
        return;
    }
    ctx->ClearExceptionCallback();
    DestroyCallback(env, cb);
    SetContextCallback(ctx, CONTEXT_CALLBACK_EXCEPTION, nullptr);
}

// ------------------------------------------------------------------ debugging

JNIEXPORT void JNICALL Java_org_angelscript_ScriptContext_nativeSetLineCallback(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jobject listener) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return;
    }
    JavaCallback *previous = GetContextCallback(ctx, CONTEXT_CALLBACK_LINE);
    if (previous != nullptr) {
        ctx->ClearLineCallback();
        DestroyCallback(env, previous);
        SetContextCallback(ctx, CONTEXT_CALLBACK_LINE, nullptr);
    }
    if (listener == nullptr) {
        return;
    }

    JavaVM *vm = nullptr;
    env->GetJavaVM(&vm);
    JavaCallback *cb = CreateCallback(env, listener, "onLine",
                                       "(Lorg/angelscript/ScriptContext;I)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The line listener must implement onLine(ScriptContext, int)");
        return;
    }
    cb->vm = vm;
    SetContextCallback(ctx, CONTEXT_CALLBACK_LINE, cb);
    ctx->SetLineCallback(MakeFuncPtr(reinterpret_cast<void *>(&LineTrampoline)), cb,
                         asCALL_CDECL_OBJLAST);
}

JNIEXPORT void JNICALL Java_org_angelscript_ScriptContext_nativeClearLineCallback(JNIEnv *env,
                                                                                  jobject thiz) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return;
    }
    JavaCallback *cb = GetContextCallback(ctx, CONTEXT_CALLBACK_LINE);
    if (cb == nullptr) {
        return;
    }
    ctx->ClearLineCallback();
    DestroyCallback(env, cb);
    SetContextCallback(ctx, CONTEXT_CALLBACK_LINE, nullptr);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetCallstackSize(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? 0 : static_cast<jint>(ctx->GetCallstackSize());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetFunction(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetFunction(static_cast<asUINT>(stackLevel)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetLineNumber(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return ctx->GetLineNumber(static_cast<asUINT>(stackLevel));
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptContext_nativeGetLineSection(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jint stackLevel) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return nullptr;
    }
    int column = 0;
    const char *section = nullptr;
    ctx->GetLineNumber(static_cast<asUINT>(stackLevel), &column, &section);
    return section == nullptr ? nullptr : FromUtf8(env, section);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetVarCount(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return ctx->GetVarCount(static_cast<asUINT>(stackLevel));
}

JNIEXPORT jobject JNICALL Java_org_angelscript_ScriptContext_nativeGetVar(JNIEnv *env,
                                                                          jobject thiz,
                                                                          jint varIndex,
                                                                          jint stackLevel) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return nullptr;
    }
    const char *name = nullptr;
    int typeId = 0;
    asETypeModifiers typeModifiers = asTM_NONE;
    bool isVarOnHeap = false;
    int stackOffset = 0;
    const int r = ctx->GetVar(static_cast<asUINT>(varIndex), static_cast<asUINT>(stackLevel), &name,
                              &typeId, &typeModifiers, &isVarOnHeap, &stackOffset);
    if (r < 0) {
        return nullptr;
    }

    jclass cls = FindClass(env, "org/angelscript/VarInfo");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;IIZII)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, name ? FromUtf8(env, name) : nullptr, typeId,
                                    static_cast<jint>(typeModifiers),
                                    isVarOnHeap ? JNI_TRUE : JNI_FALSE, stackOffset);
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptContext_nativeGetVarDeclaration(
        JNIEnv *env, jobject thiz, jint varIndex, jint stackLevel, jboolean includeNamespace) {
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return nullptr;
    }
    const char *decl = ctx->GetVarDeclaration(static_cast<asUINT>(varIndex),
                                               static_cast<asUINT>(stackLevel),
                                               includeNamespace == JNI_TRUE);
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetAddressOfVar(
        JNIEnv *env, jobject thiz, jint varIndex, jint stackLevel, jboolean dontDereference,
        jboolean returnAddressOfUnitializedObjects) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetAddressOfVar(
            static_cast<asUINT>(varIndex), static_cast<asUINT>(stackLevel),
            dontDereference == JNI_TRUE, returnAddressOfUnitializedObjects == JNI_TRUE));
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptContext_nativeIsVarInScope(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jint varIndex,
                                                                                  jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return JNI_FALSE;
    }
    return ctx->IsVarInScope(static_cast<asUINT>(varIndex), static_cast<asUINT>(stackLevel))
                   ? JNI_TRUE
                   : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetThisTypeId(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return ctx->GetThisTypeId(static_cast<asUINT>(stackLevel));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetThisPointer(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetThisPointer(static_cast<asUINT>(stackLevel)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetSystemFunction(JNIEnv *env,
                                                                                  jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetSystemFunction());
}

// ------------------------------------------------------------------ user data

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeSetUserData(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jlong data,
                                                                             jlong type) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(data)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptContext_nativeGetUserData(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jlong type) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(ctx->GetUserData(static_cast<asPWORD>(type)));
}

// ------------------------------------------------------------------ serialization

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeStartDeserialization(JNIEnv *env,
                                                                                    jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->StartDeserialization();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeFinishDeserialization(JNIEnv *env,
                                                                                     jobject thiz) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    return ctx == nullptr ? asERROR : ctx->FinishDeserialization();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativePushFunction(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jlong func,
                                                                             jlong object) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->PushFunction(reinterpret_cast<asIScriptFunction *>(func),
                              reinterpret_cast<void *>(static_cast<intptr_t>(object)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetArgsOnStackCount(JNIEnv *env,
                                                                                     jobject thiz,
                                                                                     jint stackLevel) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    return ctx->GetArgsOnStackCount(static_cast<asUINT>(stackLevel));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptContext_nativeGetArgOnStack(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint stackLevel,
                                                                             jint arg) {
    (void)env;
    asIScriptContext *ctx = ContextOf(GetPtr(env, thiz));
    if (ctx == nullptr) {
        return asERROR;
    }
    int typeId = 0;
    asUINT flags = 0;
    void *address = nullptr;
    const int r = ctx->GetArgOnStack(static_cast<asUINT>(stackLevel), static_cast<asUINT>(arg),
                                     &typeId, &flags, &address);
    return r < 0 ? r : typeId;
}

}  // extern "C"
