// JNI bindings for org.angelscript.ScriptModule (asIScriptModule).
#include "asjava.h"

using namespace asjava;

namespace {

asIScriptModule *ModuleOf(jlong ptr) {
    return reinterpret_cast<asIScriptModule *>(ptr);
}

}  // namespace

extern "C" {

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetEngine(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : reinterpret_cast<jlong>(module->GetEngine());
}

JNIEXPORT void JNICALL Java_org_angelscript_ScriptModule_nativeSetName(JNIEnv *env, jobject thiz,
                                                                       jstring name) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return;
    }
    const std::string str = ToUtf8(env, name);
    module->SetName(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptModule_nativeGetName(JNIEnv *env,
                                                                          jobject thiz) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *name = module->GetName();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT void JNICALL Java_org_angelscript_ScriptModule_nativeDiscard(JNIEnv *env, jobject thiz) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return;
    }
    module->Discard();
}

// ------------------------------------------------------------------ compilation

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeAddScriptSection(
        JNIEnv *env, jobject thiz, jstring name, jstring code, jint lineOffset) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string sectionName = ToUtf8(env, name);
    const std::string source = ToUtf8(env, code);
    return module->AddScriptSection(sectionName.c_str(), source.c_str(), source.size(), lineOffset);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeBuild(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? asERROR : module->Build();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeCompileFunction(
        JNIEnv *env, jobject thiz, jstring sectionName, jstring code, jint lineOffset,
        jint compileFlags) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    const std::string section = ToUtf8(env, sectionName);
    const std::string source = ToUtf8(env, code);
    asIScriptFunction *func = nullptr;
    const int r = module->CompileFunction(section.c_str(), source.c_str(), lineOffset,
                                          static_cast<asDWORD>(compileFlags), &func);
    if (r < 0) {
        return 0;
    }
    return reinterpret_cast<jlong>(func);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeCompileGlobalVar(
        JNIEnv *env, jobject thiz, jstring sectionName, jstring code, jint lineOffset) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string section = ToUtf8(env, sectionName);
    const std::string source = ToUtf8(env, code);
    return module->CompileGlobalVar(section.c_str(), source.c_str(), lineOffset);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeSetAccessMask(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint accessMask) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return static_cast<jint>(
            module->SetAccessMask(static_cast<asDWORD>(accessMask)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeSetDefaultNamespace(
        JNIEnv *env, jobject thiz, jstring nameSpace) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, nameSpace);
    return module->SetDefaultNamespace(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptModule_nativeGetDefaultNamespace(
        JNIEnv *env, jobject thiz) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *name = module->GetDefaultNamespace();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

// ------------------------------------------------------------------ functions

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetFunctionCount(JNIEnv *env,
                                                                               jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetFunctionCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetFunctionByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetFunctionByIndex(static_cast<asUINT>(index)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetFunctionByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, decl);
    return reinterpret_cast<jlong>(module->GetFunctionByDecl(str.c_str()));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetFunctionByName(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, name);
    return reinterpret_cast<jlong>(module->GetFunctionByName(str.c_str()));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeRemoveFunction(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jlong func) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return module->RemoveFunction(reinterpret_cast<asIScriptFunction *>(func));
}

// ------------------------------------------------------------------ global variables

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeResetGlobalVars(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong ctx) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return module->ResetGlobalVars(reinterpret_cast<asIScriptContext *>(ctx));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetGlobalVarCount(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetGlobalVarCount());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetGlobalVarIndexByName(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, name);
    return module->GetGlobalVarIndexByName(str.c_str());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetGlobalVarIndexByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return module->GetGlobalVarIndexByDecl(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptModule_nativeGetGlobalVarDeclaration(
        JNIEnv *env, jobject thiz, jint index, jboolean includeNamespace) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *decl = module->GetGlobalVarDeclaration(static_cast<asUINT>(index),
                                                        includeNamespace == JNI_TRUE);
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jobject JNICALL Java_org_angelscript_ScriptModule_nativeGetGlobalVar(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint index) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *name = nullptr;
    const char *nameSpace = nullptr;
    int typeId = 0;
    bool isConst = false;
    const int r = module->GetGlobalVar(static_cast<asUINT>(index), &name, &nameSpace, &typeId,
                                       &isConst);
    if (r < 0) {
        return nullptr;
    }

    jclass cls = FindClass(env, "org/angelscript/GlobalVarInfo");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>",
                                       "(Ljava/lang/String;Ljava/lang/String;IZ)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, name ? FromUtf8(env, name) : nullptr,
                                    nameSpace ? FromUtf8(env, nameSpace) : nullptr, typeId,
                                    isConst ? JNI_TRUE : JNI_FALSE);
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetAddressOfGlobalVar(
        JNIEnv *env, jobject thiz, jint index) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetAddressOfGlobalVar(static_cast<asUINT>(index)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeRemoveGlobalVar(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint index) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return module->RemoveGlobalVar(static_cast<asUINT>(index));
}

// ------------------------------------------------------------------ types

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetObjectTypeCount(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetObjectTypeCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetObjectTypeByIndex(
        JNIEnv *env, jobject thiz, jint index) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetObjectTypeByIndex(static_cast<asUINT>(index)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetTypeIdByDecl(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jstring decl) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return module->GetTypeIdByDecl(str.c_str());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetTypeInfoByName(
        JNIEnv *env, jobject thiz, jstring name) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, name);
    return reinterpret_cast<jlong>(module->GetTypeInfoByName(str.c_str()));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetTypeInfoByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, decl);
    return reinterpret_cast<jlong>(module->GetTypeInfoByDecl(str.c_str()));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetEnumCount(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetEnumCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetEnumByIndex(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint index) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetEnumByIndex(static_cast<asUINT>(index)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetTypedefCount(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetTypedefCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetTypedefByIndex(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint index) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetTypedefByIndex(static_cast<asUINT>(index)));
}

// ------------------------------------------------------------------ imported functions

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetImportedFunctionCount(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? 0 : static_cast<jint>(module->GetImportedFunctionCount());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeGetImportedFunctionIndexByDecl(
        JNIEnv *env, jobject thiz, jstring decl) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    const std::string str = ToUtf8(env, decl);
    return module->GetImportedFunctionIndexByDecl(str.c_str());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptModule_nativeGetImportedFunctionDeclaration(
        JNIEnv *env, jobject thiz, jint importIndex) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *decl =
            module->GetImportedFunctionDeclaration(static_cast<asUINT>(importIndex));
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jstring JNICALL
Java_org_angelscript_ScriptModule_nativeGetImportedFunctionSourceModule(JNIEnv *env, jobject thiz,
                                                                         jint importIndex) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return nullptr;
    }
    const char *name = module->GetImportedFunctionSourceModule(static_cast<asUINT>(importIndex));
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeBindImportedFunction(
        JNIEnv *env, jobject thiz, jint importIndex, jlong func) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return module->BindImportedFunction(static_cast<asUINT>(importIndex),
                                        reinterpret_cast<asIScriptFunction *>(func));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeUnbindImportedFunction(
        JNIEnv *env, jobject thiz, jint importIndex) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return asERROR;
    }
    return module->UnbindImportedFunction(static_cast<asUINT>(importIndex));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeBindAllImportedFunctions(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? asERROR : module->BindAllImportedFunctions();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeUnbindAllImportedFunctions(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    return module == nullptr ? asERROR : module->UnbindAllImportedFunctions();
}

// ------------------------------------------------------------------ byte code

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeSaveByteCode(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jobject stream,
                                                                            jboolean stripDebugInfo) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr || stream == nullptr) {
        return asERROR;
    }
    asIBinaryStream *impl = CreateBinaryStream(env, stream);
    if (impl == nullptr) {
        return asERROR;
    }
    const int r = module->SaveByteCode(impl, stripDebugInfo == JNI_TRUE);
    delete impl;
    return r;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptModule_nativeLoadByteCode(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jobject stream) {
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr || stream == nullptr) {
        return asERROR;
    }
    asIBinaryStream *impl = CreateBinaryStream(env, stream);
    if (impl == nullptr) {
        return asERROR;
    }
    const int r = module->LoadByteCode(impl, nullptr);
    delete impl;
    return r;
}

// ------------------------------------------------------------------ user data

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeSetUserData(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jlong data,
                                                                           jlong type) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(data)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptModule_nativeGetUserData(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jlong type) {
    (void)env;
    asIScriptModule *module = ModuleOf(GetPtr(env, thiz));
    if (module == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(module->GetUserData(static_cast<asPWORD>(type)));
}

}  // extern "C"
