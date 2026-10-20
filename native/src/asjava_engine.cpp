// JNI bindings for org.angelscript.AngelScriptEngine (asIScriptEngine).
#include "asjava.h"

#include <string>

using namespace asjava;

namespace {

asIScriptEngine *EngineOf(jlong ptr) {
    return reinterpret_cast<asIScriptEngine *>(ptr);
}

jstring NewStringOrNull(JNIEnv *env, const char *str) {
    if (str == nullptr) {
        return nullptr;
    }
    return FromUtf8(env, str);
}

}  // namespace

extern "C" {

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeCreate(JNIEnv *env,
                                                                              jclass) {
    asIScriptEngine *engine = asCreateScriptEngine();
    if (engine == nullptr) {
        return 0;
    }
    JavaVM *vm = nullptr;
    env->GetJavaVM(&vm);
    CreateEngineState(engine, vm);
    return reinterpret_cast<jlong>(engine);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_AngelScriptEngine_nativeLibraryVersion(JNIEnv *env,
                                                                                       jclass) {
    return FromUtf8(env, asGetLibraryVersion());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_AngelScriptEngine_nativeLibraryOptions(
        JNIEnv *env, jclass) {
    return FromUtf8(env, asGetLibraryOptions());
}

// ------------------------------------------------------------------ properties

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeSetEngineProperty(
        JNIEnv *env, jobject thiz, jint property, jlong value) {
    (void)env;
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    return engine->SetEngineProperty(static_cast<asEEngineProp>(property),
                                     static_cast<asPWORD>(value));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetEngineProperty(
        JNIEnv *env, jobject thiz, jint property) {
    (void)env;
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return static_cast<jlong>(engine->GetEngineProperty(static_cast<asEEngineProp>(property)));
}

// ------------------------------------------------------------------ messages

JNIEXPORT void JNICALL Java_org_angelscript_AngelScriptEngine_nativeSetMessageCallback(
        JNIEnv *env, jobject thiz, jobject listener) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return;
    }

    if (state->messageCallback != nullptr) {
        engine->ClearMessageCallback();
        DestroyCallback(env, state->messageCallback);
        state->messageCallback = nullptr;
    }
    if (listener == nullptr) {
        return;
    }

    JavaCallback *cb = CreateCallback(env, listener, "onMessage",
                                       "(Ljava/lang/String;IILorg/angelscript/MessageType;"
                                       "Ljava/lang/String;)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The message listener must implement onMessage(String, int, int, MessageType, "
                  "String)");
        return;
    }
    cb->vm = state->vm;
    state->messageCallback = cb;
    engine->SetMessageCallback(MakeFuncPtr(reinterpret_cast<void *>(&MessageTrampoline)), cb,
                               asCALL_CDECL_OBJLAST);
}

JNIEXPORT void JNICALL Java_org_angelscript_AngelScriptEngine_nativeClearMessageCallback(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr || state->messageCallback == nullptr) {
        return;
    }
    engine->ClearMessageCallback();
    DestroyCallback(env, state->messageCallback);
    state->messageCallback = nullptr;
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeWriteMessage(
        JNIEnv *env, jobject thiz, jstring section, jint row, jint col, jint type,
        jstring message) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string sectionStr = ToUtf8(env, section);
    const std::string messageStr = ToUtf8(env, message);
    return engine->WriteMessage(sectionStr.c_str(), row, col, static_cast<asEMsgType>(type),
                                messageStr.c_str());
}

// ------------------------------------------------------------------ global functions

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterGlobalFunction(
        JNIEnv *env, jobject thiz, jstring declaration, jobject callback) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return asERROR;
    }

    JavaCallback *cb = CreateCallback(env, callback, "invoke",
                                       "(Lorg/angelscript/ScriptGeneric;)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The script callback must implement invoke(ScriptGeneric)");
        return asERROR;
    }
    cb->vm = state->vm;
    state->callbacks.push_back(cb);

    const std::string decl = ToUtf8(env, declaration);
    const int r = engine->RegisterGlobalFunction(decl.c_str(),
                                                 MakeGenericFuncPtr(reinterpret_cast<void *>(
                                                         &GenericTrampoline)),
                                                 asCALL_GENERIC, cb);
    if (r < 0) {
        DestroyCallback(env, cb);
        state->callbacks.pop_back();
    }
    return r;
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalFunctionCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetGlobalFunctionCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalFunctionByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetGlobalFunctionByIndex(
            static_cast<asUINT>(index)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalFunctionByDecl(
        JNIEnv *env, jobject thiz, jstring declaration) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    const std::string decl = ToUtf8(env, declaration);
    return reinterpret_cast<jlong>(engine->GetGlobalFunctionByDecl(decl.c_str()));
}

// ------------------------------------------------------------------ global properties

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterGlobalProperty(
        JNIEnv *env, jobject thiz, jstring declaration, jlong pointer) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string decl = ToUtf8(env, declaration);
    return engine->RegisterGlobalProperty(decl.c_str(),
                                          reinterpret_cast<void *>(static_cast<intptr_t>(pointer)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalPropertyCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetGlobalPropertyCount());
}

JNIEXPORT jobject JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalPropertyByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return nullptr;
    }

    const char *name = nullptr;
    const char *nameSpace = nullptr;
    const char *configGroup = nullptr;
    int typeId = 0;
    bool isConst = false;
    void *pointer = nullptr;
    asDWORD accessMask = 0;

    const int r = engine->GetGlobalPropertyByIndex(static_cast<asUINT>(index), &name, &nameSpace,
                                                    &typeId, &isConst, &configGroup, &pointer,
                                                    &accessMask);
    if (r < 0) {
        return nullptr;
    }

    jclass cls = FindClass(env, "org/angelscript/GlobalPropertyInfo");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>",
                                       "(Ljava/lang/String;Ljava/lang/String;IZLjava/lang/String;JI)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, NewStringOrNull(env, name),
                                    NewStringOrNull(env, nameSpace), typeId,
                                    isConst ? JNI_TRUE : JNI_FALSE,
                                    NewStringOrNull(env, configGroup),
                                    reinterpret_cast<jlong>(pointer),
                                    static_cast<jint>(accessMask));
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalPropertyIndexByName(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, name);
    return engine->GetGlobalPropertyIndexByName(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGlobalPropertyIndexByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return engine->GetGlobalPropertyIndexByDecl(str.c_str());
}

// ------------------------------------------------------------------ object types

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterObjectType(
        JNIEnv *env, jobject thiz, jstring obj, jint byteSize, jlong flags) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string name = ToUtf8(env, obj);
    return engine->RegisterObjectType(name.c_str(), byteSize, static_cast<asQWORD>(flags));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterObjectProperty(
        JNIEnv *env, jobject thiz, jstring obj, jstring declaration, jint byteOffset,
        jint compositeOffset, jboolean isCompositeIndirect) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string typeName = ToUtf8(env, obj);
    const std::string decl = ToUtf8(env, declaration);
    return engine->RegisterObjectProperty(typeName.c_str(), decl.c_str(), byteOffset,
                                          compositeOffset,
                                          isCompositeIndirect == JNI_TRUE);
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterObjectMethod(
        JNIEnv *env, jobject thiz, jstring obj, jstring declaration, jobject callback) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return asERROR;
    }

    JavaCallback *cb = CreateCallback(env, callback, "invoke",
                                       "(Lorg/angelscript/ScriptGeneric;)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The script callback must implement invoke(ScriptGeneric)");
        return asERROR;
    }
    cb->vm = state->vm;
    state->callbacks.push_back(cb);

    const std::string typeName = ToUtf8(env, obj);
    const std::string decl = ToUtf8(env, declaration);
    const int r = engine->RegisterObjectMethod(typeName.c_str(), decl.c_str(),
                                               MakeGenericFuncPtr(reinterpret_cast<void *>(
                                                       &GenericTrampoline)),
                                               asCALL_GENERIC, cb);
    if (r < 0) {
        DestroyCallback(env, cb);
        state->callbacks.pop_back();
    }
    return r;
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterObjectBehaviour(
        JNIEnv *env, jobject thiz, jstring obj, jint behaviour, jstring declaration,
        jobject callback) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return asERROR;
    }

    JavaCallback *cb = CreateCallback(env, callback, "invoke",
                                       "(Lorg/angelscript/ScriptGeneric;)V");
    if (cb == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "The script callback must implement invoke(ScriptGeneric)");
        return asERROR;
    }
    cb->vm = state->vm;
    state->callbacks.push_back(cb);

    const std::string typeName = ToUtf8(env, obj);
    const std::string decl = ToUtf8(env, declaration);
    const int r = engine->RegisterObjectBehaviour(typeName.c_str(),
                                                   static_cast<asEBehaviours>(behaviour),
                                                   decl.c_str(),
                                                   MakeGenericFuncPtr(reinterpret_cast<void *>(
                                                           &GenericTrampoline)),
                                                   asCALL_GENERIC, cb);
    if (r < 0) {
        DestroyCallback(env, cb);
        state->callbacks.pop_back();
    }
    return r;
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterInterface(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, name);
    return engine->RegisterInterface(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterInterfaceMethod(
        JNIEnv *env, jobject thiz, jstring intf, jstring declaration) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string interfaceName = ToUtf8(env, intf);
    const std::string decl = ToUtf8(env, declaration);
    return engine->RegisterInterfaceMethod(interfaceName.c_str(), decl.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetObjectTypeCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetObjectTypeCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetObjectTypeByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetObjectTypeByIndex(static_cast<asUINT>(index)));
}

// ------------------------------------------------------------------ string factory

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterStringFactory(
        JNIEnv *env, jobject thiz, jstring datatype, jobject factory) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return asERROR;
    }
    const std::string typeName = ToUtf8(env, datatype);
    asIStringFactory *impl = CreateStringFactory(env, factory);
    if (impl == nullptr) {
        ThrowJava(env, "java/lang/IllegalArgumentException",
                  "Could not create the string factory bridge");
        return asERROR;
    }
    const int r = engine->RegisterStringFactory(typeName.c_str(), impl);
    if (r < 0) {
        delete impl;
    } else {
        state->stringFactories[impl] = env->NewGlobalRef(factory);
    }
    return r;
}

JNIEXPORT jobject JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetStringFactory(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return nullptr;
    }
    EngineState *state = GetEngineState(engine);
    if (state == nullptr) {
        return nullptr;
    }

    asDWORD modifiers = 0;
    asIStringFactory *factory = nullptr;
    engine->GetStringFactory(&modifiers, &factory);
    if (factory == nullptr) {
        return nullptr;
    }

    // A factory that was registered from Java is returned as it is.
    auto it = state->stringFactories.find(factory);
    if (it != state->stringFactories.end()) {
        return env->NewLocalRef(it->second);
    }

    // A factory that was registered by an add-on is wrapped in a proxy.
    asIStringFactory *proxy = CreateStringFactoryProxy(factory);
    if (proxy == nullptr) {
        return nullptr;
    }
    OwnedResource resource;
    resource.ptr = proxy;
    resource.deleter = [](void *ptr) { delete static_cast<asIStringFactory *>(ptr); };
    state->owned.push_back(resource);

    jclass cls = FindClass(env, "org/angelscript/NativeStringFactory");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>", "(J)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, reinterpret_cast<jlong>(proxy));
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jint JNICALL
Java_org_angelscript_AngelScriptEngine_nativeGetStringFactoryTypeModifiers(JNIEnv *env,
                                                                            jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    asDWORD modifiers = 0;
    engine->GetStringFactory(&modifiers, nullptr);
    return static_cast<jint>(modifiers);
}

// ------------------------------------------------------------------ array type

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterDefaultArrayType(
        JNIEnv *env, jobject thiz, jstring type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, type);
    return engine->RegisterDefaultArrayType(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetDefaultArrayTypeId(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : engine->GetDefaultArrayTypeId();
}

// ------------------------------------------------------------------ enums

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterEnum(
        JNIEnv *env, jobject thiz, jstring type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, type);
    return engine->RegisterEnum(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterEnumValue(
        JNIEnv *env, jobject thiz, jstring type, jstring name, jint value) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string typeName = ToUtf8(env, type);
    const std::string valueName = ToUtf8(env, name);
    return engine->RegisterEnumValue(typeName.c_str(), valueName.c_str(), value);
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetEnumCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetEnumCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetEnumByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetEnumByIndex(static_cast<asUINT>(index)));
}

// ------------------------------------------------------------------ funcdefs and typedefs

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterFuncdef(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return engine->RegisterFuncdef(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetFuncdefCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetFuncdefCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetFuncdefByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetFuncdefByIndex(static_cast<asUINT>(index)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRegisterTypedef(
        JNIEnv *env, jobject thiz, jstring type, jstring decl) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string typeName = ToUtf8(env, type);
    const std::string declStr = ToUtf8(env, decl);
    return engine->RegisterTypedef(typeName.c_str(), declStr.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypedefCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetTypedefCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypedefByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetTypedefByIndex(static_cast<asUINT>(index)));
}

// ------------------------------------------------------------------ configuration groups

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeBeginConfigGroup(
        JNIEnv *env, jobject thiz, jstring groupName) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, groupName);
    return engine->BeginConfigGroup(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeEndConfigGroup(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? asERROR : engine->EndConfigGroup();
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRemoveConfigGroup(
        JNIEnv *env, jobject thiz, jstring groupName) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, groupName);
    return engine->RemoveConfigGroup(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeSetDefaultAccessMask(
        JNIEnv *env, jobject thiz, jint defaultMask) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? asERROR
                             : static_cast<jint>(engine->SetDefaultAccessMask(
                                     static_cast<asDWORD>(defaultMask)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeSetDefaultNamespace(
        JNIEnv *env, jobject thiz, jstring nameSpace) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, nameSpace);
    return engine->SetDefaultNamespace(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetDefaultNamespace(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return nullptr;
    }
    return NewStringOrNull(env, engine->GetDefaultNamespace());
}

// ------------------------------------------------------------------ modules

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetModule(
        JNIEnv *env, jobject thiz, jstring module, jint flag) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, module);
    return reinterpret_cast<jlong>(
            engine->GetModule(str.c_str(), static_cast<asEGMFlags>(flag)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeDiscardModule(
        JNIEnv *env, jobject thiz, jstring module) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, module);
    return engine->DiscardModule(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetModuleCount(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : static_cast<jint>(engine->GetModuleCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetModuleByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetModuleByIndex(static_cast<asUINT>(index)));
}

// ------------------------------------------------------------------ functions

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetLastFunctionId(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : engine->GetLastFunctionId();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetFunctionById(
        JNIEnv *env, jobject thiz, jint funcId) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetFunctionById(funcId));
}

// ------------------------------------------------------------------ type identification

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypeIdByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return engine->GetTypeIdByDecl(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypeDeclaration(
        JNIEnv *env, jobject thiz, jint typeId, jboolean includeNamespace) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return nullptr;
    }
    return NewStringOrNull(env,
                           engine->GetTypeDeclaration(typeId, includeNamespace == JNI_TRUE));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetSizeOfPrimitiveType(
        JNIEnv *env, jobject thiz, jint typeId) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    return engine == nullptr ? 0 : engine->GetSizeOfPrimitiveType(typeId);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypeInfoById(
        JNIEnv *env, jobject thiz, jint typeId) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetTypeInfoById(typeId));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypeInfoByName(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, name);
    return reinterpret_cast<jlong>(engine->GetTypeInfoByName(str.c_str()));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetTypeInfoByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, decl);
    return reinterpret_cast<jlong>(engine->GetTypeInfoByDecl(str.c_str()));
}

// ------------------------------------------------------------------ script execution

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeCreateContext(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->CreateContext());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeCreateScriptObject(
        JNIEnv *env, jobject thiz, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(
            engine->CreateScriptObject(reinterpret_cast<asITypeInfo *>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeCreateScriptObjectCopy(
        JNIEnv *env, jobject thiz, jlong obj, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->CreateScriptObjectCopy(
            reinterpret_cast<void *>(static_cast<intptr_t>(obj)),
            reinterpret_cast<asITypeInfo *>(type)));
}

JNIEXPORT jlong JNICALL
Java_org_angelscript_AngelScriptEngine_nativeCreateUninitializedScriptObject(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(
            engine->CreateUninitializedScriptObject(reinterpret_cast<asITypeInfo *>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeCreateDelegate(
        JNIEnv *env, jobject thiz, jlong func, jlong obj) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->CreateDelegate(
            reinterpret_cast<asIScriptFunction *>(func),
            reinterpret_cast<void *>(static_cast<intptr_t>(obj))));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeAssignScriptObject(
        JNIEnv *env, jobject thiz, jlong dstObj, jlong srcObj, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    return engine->AssignScriptObject(reinterpret_cast<void *>(static_cast<intptr_t>(dstObj)),
                                      reinterpret_cast<void *>(static_cast<intptr_t>(srcObj)),
                                      reinterpret_cast<asITypeInfo *>(type));
}

JNIEXPORT void JNICALL Java_org_angelscript_AngelScriptEngine_nativeReleaseScriptObject(
        JNIEnv *env, jobject thiz, jlong obj, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return;
    }
    engine->ReleaseScriptObject(reinterpret_cast<void *>(static_cast<intptr_t>(obj)),
                                 reinterpret_cast<asITypeInfo *>(type));
}

JNIEXPORT void JNICALL Java_org_angelscript_AngelScriptEngine_nativeAddRefScriptObject(
        JNIEnv *env, jobject thiz, jlong obj, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return;
    }
    engine->AddRefScriptObject(reinterpret_cast<void *>(static_cast<intptr_t>(obj)),
                               reinterpret_cast<asITypeInfo *>(type));
}

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeRefCastObject(
        JNIEnv *env, jobject thiz, jlong obj, jlong fromType, jlong toType, jlongArray outPtr,
        jboolean useOnlyImplicitCast) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    void *newPtr = nullptr;
    const int r = engine->RefCastObject(reinterpret_cast<void *>(static_cast<intptr_t>(obj)),
                                         reinterpret_cast<asITypeInfo *>(fromType),
                                         reinterpret_cast<asITypeInfo *>(toType), &newPtr,
                                         useOnlyImplicitCast == JNI_TRUE);
    if (outPtr != nullptr && newPtr != nullptr) {
        jlong value = reinterpret_cast<jlong>(newPtr);
        env->SetLongArrayRegion(outPtr, 0, 1, &value);
    }
    return r;
}

// ------------------------------------------------------------------ context pooling

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeRequestContext(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->RequestContext());
}

JNIEXPORT void JNICALL Java_org_angelscript_AngelScriptEngine_nativeReturnContext(
        JNIEnv *env, jobject thiz, jlong ctx) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return;
    }
    engine->ReturnContext(reinterpret_cast<asIScriptContext *>(ctx));
}

// ------------------------------------------------------------------ tokenizer

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeParseToken(
        JNIEnv *env, jobject thiz, jstring string) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, string);
    return static_cast<jint>(engine->ParseToken(str.c_str(), str.size()));
}

// ------------------------------------------------------------------ garbage collection

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeGarbageCollect(
        JNIEnv *env, jobject thiz, jint flags, jint numIterations) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return asERROR;
    }
    return engine->GarbageCollect(static_cast<asDWORD>(flags),
                                  static_cast<asUINT>(numIterations));
}

JNIEXPORT jobject JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetGCStatistics(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return nullptr;
    }
    asUINT currentSize = 0;
    asUINT totalDestroyed = 0;
    asUINT totalDetected = 0;
    asUINT newObjects = 0;
    asUINT totalNewDestroyed = 0;
    engine->GetGCStatistics(&currentSize, &totalDestroyed, &totalDetected, &newObjects,
                            &totalNewDestroyed);

    jclass cls = FindClass(env, "org/angelscript/GCStatistics");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>", "(IIIII)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, static_cast<jint>(currentSize),
                                    static_cast<jint>(totalDestroyed),
                                    static_cast<jint>(totalDetected),
                                    static_cast<jint>(newObjects),
                                    static_cast<jint>(totalNewDestroyed));
    env->DeleteLocalRef(cls);
    return result;
}

// ------------------------------------------------------------------ user data

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeSetUserData(
        JNIEnv *env, jobject thiz, jlong data, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(data)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_AngelScriptEngine_nativeGetUserData(
        JNIEnv *env, jobject thiz, jlong type) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(engine->GetUserData(static_cast<asPWORD>(type)));
}

// ------------------------------------------------------------------ shutdown

JNIEXPORT jint JNICALL Java_org_angelscript_AngelScriptEngine_nativeShutDownAndRelease(
        JNIEnv *env, jobject thiz) {
    asIScriptEngine *engine = EngineOf(GetPtr(env, thiz));
    if (engine == nullptr) {
        return 0;
    }
    // Shut the engine down first so that it releases all references to the Java objects, then
    // clean up the Java types and the engine state.
    const int r = engine->ShutDownAndRelease();
    DestroyJavaTypes(engine);
    DestroyEngineState(engine);
    return r;
}

}  // extern "C"
