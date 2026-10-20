// JNI bindings for org.angelscript.TypeInfo, ScriptObject, ScriptFunction, ScriptGeneric and
// LockableSharedBool.
#include "asjava.h"

using namespace asjava;

namespace {

asITypeInfo *TypeInfoOf(jlong ptr) {
    return reinterpret_cast<asITypeInfo *>(ptr);
}

asIScriptObject *ObjectOf(jlong ptr) {
    return reinterpret_cast<asIScriptObject *>(ptr);
}

asIScriptFunction *FunctionOf(jlong ptr) {
    return reinterpret_cast<asIScriptFunction *>(ptr);
}

asIScriptGeneric *GenericOf(jlong ptr) {
    return reinterpret_cast<asIScriptGeneric *>(ptr);
}

}  // namespace

extern "C" {

// ==================================================================== TypeInfo

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetEngine(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : reinterpret_cast<jlong>(type->GetEngine());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_TypeInfo_nativeGetConfigGroup(JNIEnv *env,
                                                                             jobject thiz) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    const char *group = type->GetConfigGroup();
    return group == nullptr ? nullptr : FromUtf8(env, group);
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetAccessMask(JNIEnv *env,
                                                                       jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetAccessMask());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetModule(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : reinterpret_cast<jlong>(type->GetModule());
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeAddRef(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : type->AddRef();
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeRelease(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : type->Release();
}

JNIEXPORT jstring JNICALL Java_org_angelscript_TypeInfo_nativeGetName(JNIEnv *env, jobject thiz) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    const char *name = type->GetName();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_TypeInfo_nativeGetNamespace(JNIEnv *env,
                                                                           jobject thiz) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    const char *name = type->GetNamespace();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetBaseType(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetBaseType());
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_TypeInfo_nativeDerivesFrom(JNIEnv *env,
                                                                         jobject thiz,
                                                                         jlong objType) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return JNI_FALSE;
    }
    return type->DerivesFrom(reinterpret_cast<asITypeInfo *>(objType)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetFlags(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jlong>(type->GetFlags());
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetSize(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetSize());
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetTypeId(JNIEnv *env, jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : type->GetTypeId();
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetSubTypeId(JNIEnv *env, jobject thiz,
                                                                        jint subTypeIndex) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : type->GetSubTypeId(static_cast<asUINT>(subTypeIndex));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetSubType(JNIEnv *env, jobject thiz,
                                                                       jint subTypeIndex) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetSubType(static_cast<asUINT>(subTypeIndex)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetSubTypeCount(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetSubTypeCount());
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetInterfaceCount(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetInterfaceCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetInterface(JNIEnv *env, jobject thiz,
                                                                        jint index) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetInterface(static_cast<asUINT>(index)));
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_TypeInfo_nativeImplements(JNIEnv *env, jobject thiz,
                                                                         jlong objType) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return JNI_FALSE;
    }
    return type->Implements(reinterpret_cast<asITypeInfo *>(objType)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetFactoryCount(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetFactoryCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetFactoryByIndex(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint index) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetFactoryByIndex(static_cast<asUINT>(index)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetFactoryByDecl(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jstring decl) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, decl);
    return reinterpret_cast<jlong>(type->GetFactoryByDecl(str.c_str()));
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetMethodCount(JNIEnv *env,
                                                                         jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetMethodCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetMethodByIndex(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint index,
                                                                             jboolean getVirtual) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(
            type->GetMethodByIndex(static_cast<asUINT>(index), getVirtual == JNI_TRUE));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetMethodByName(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring name,
                                                                           jboolean getVirtual) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, name);
    return reinterpret_cast<jlong>(type->GetMethodByName(str.c_str(), getVirtual == JNI_TRUE));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetMethodByDecl(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jstring decl,
                                                                            jboolean getVirtual) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    const std::string str = ToUtf8(env, decl);
    return reinterpret_cast<jlong>(type->GetMethodByDecl(str.c_str(), getVirtual == JNI_TRUE));
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetPropertyCount(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetPropertyCount());
}

JNIEXPORT jobject JNICALL Java_org_angelscript_TypeInfo_nativeGetProperty(JNIEnv *env, jobject thiz,
                                                                           jint index) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    const char *name = nullptr;
    int typeId = 0;
    bool isPrivate = false;
    bool isProtected = false;
    int offset = 0;
    bool isReference = false;
    asDWORD accessMask = 0;
    int compositeOffset = 0;
    bool isCompositeIndirect = false;
    bool isConst = false;
    const int r = type->GetProperty(static_cast<asUINT>(index), &name, &typeId, &isPrivate,
                                     &isProtected, &offset, &isReference, &accessMask,
                                     &compositeOffset, &isCompositeIndirect, &isConst);
    if (r < 0) {
        return nullptr;
    }

    jclass cls = FindClass(env, "org/angelscript/PropertyInfo");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>",
                                       "(Ljava/lang/String;IZZIZIIZZ)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, name ? FromUtf8(env, name) : nullptr, typeId,
                                    isPrivate ? JNI_TRUE : JNI_FALSE,
                                    isProtected ? JNI_TRUE : JNI_FALSE, offset,
                                    isReference ? JNI_TRUE : JNI_FALSE,
                                    static_cast<jint>(accessMask), compositeOffset,
                                    isCompositeIndirect ? JNI_TRUE : JNI_FALSE,
                                    isConst ? JNI_TRUE : JNI_FALSE);
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jstring JNICALL Java_org_angelscript_TypeInfo_nativeGetPropertyDeclaration(
        JNIEnv *env, jobject thiz, jint index, jboolean includeNamespace) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    const char *decl = type->GetPropertyDeclaration(static_cast<asUINT>(index),
                                                     includeNamespace == JNI_TRUE);
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetBehaviourCount(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetBehaviourCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetBehaviourByIndex(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint index) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    asEBehaviours behaviour = asBEHAVE_CONSTRUCT;
    return reinterpret_cast<jlong>(type->GetBehaviourByIndex(static_cast<asUINT>(index),
                                                              &behaviour));
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetBehaviourTypeByIndex(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jint index) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    asEBehaviours behaviour = asBEHAVE_CONSTRUCT;
    type->GetBehaviourByIndex(static_cast<asUINT>(index), &behaviour);
    return static_cast<jint>(behaviour);
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetChildFuncdefCount(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetChildFuncdefCount());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetChildFuncdef(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint index) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetChildFuncdef(static_cast<asUINT>(index)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetParentType(JNIEnv *env,
                                                                         jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetParentType());
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetEnumValueCount(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : static_cast<jint>(type->GetEnumValueCount());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_TypeInfo_nativeGetEnumValueByIndex(
        JNIEnv *env, jobject thiz, jint index, jintArray outValue) {
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return nullptr;
    }
    int value = 0;
    const char *name = type->GetEnumValueByIndex(static_cast<asUINT>(index), &value);
    if (outValue != nullptr) {
        jint javaValue = value;
        env->SetIntArrayRegion(outValue, 0, 1, &javaValue);
    }
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jint JNICALL Java_org_angelscript_TypeInfo_nativeGetTypedefTypeId(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    return type == nullptr ? 0 : type->GetTypedefTypeId();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetFuncdefSignature(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asITypeInfo *type = TypeInfoOf(GetPtr(env, thiz));
    if (type == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(type->GetFuncdefSignature());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeSetUserData(JNIEnv *env, jobject thiz,
                                                                        jlong data, jlong type) {
    (void)env;
    asITypeInfo *info = TypeInfoOf(GetPtr(env, thiz));
    if (info == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(info->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(data)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_TypeInfo_nativeGetUserData(JNIEnv *env, jobject thiz,
                                                                        jlong type) {
    (void)env;
    asITypeInfo *info = TypeInfoOf(GetPtr(env, thiz));
    if (info == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(info->GetUserData(static_cast<asPWORD>(type)));
}

// ==================================================================== ScriptObject

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeAddRef(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : object->AddRef();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeRelease(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : object->Release();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeGetWeakRefFlag(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(object->GetWeakRefFlag());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeGetTypeId(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : object->GetTypeId();
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeGetObjectType(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(object->GetObjectType());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeGetPropertyCount(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : static_cast<jint>(object->GetPropertyCount());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeGetPropertyTypeId(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jint prop) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : object->GetPropertyTypeId(static_cast<asUINT>(prop));
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptObject_nativeGetPropertyName(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint prop) {
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return nullptr;
    }
    const char *name = object->GetPropertyName(static_cast<asUINT>(prop));
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeGetAddressOfProperty(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jint prop) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(object->GetAddressOfProperty(static_cast<asUINT>(prop)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeGetEngine(JNIEnv *env,
                                                                         jobject thiz) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    return object == nullptr ? 0 : reinterpret_cast<jlong>(object->GetEngine());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptObject_nativeCopyFrom(JNIEnv *env, jobject thiz,
                                                                         jlong other) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return asERROR;
    }
    return object->CopyFrom(reinterpret_cast<asIScriptObject *>(other));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeSetUserData(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jlong data, jlong type) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(object->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(data)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptObject_nativeGetUserData(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jlong type) {
    (void)env;
    asIScriptObject *object = ObjectOf(GetPtr(env, thiz));
    if (object == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(object->GetUserData(static_cast<asPWORD>(type)));
}

// ==================================================================== ScriptFunction

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetEngine(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : reinterpret_cast<jlong>(func->GetEngine());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeAddRef(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->AddRef();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeRelease(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->Release();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetId(JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->GetId();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetFuncType(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : static_cast<jint>(func->GetFuncType());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetModuleName(JNIEnv *env,
                                                                                  jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *name = func->GetModuleName();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetModule(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetModule());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetConfigGroup(JNIEnv *env,
                                                                                   jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *group = func->GetConfigGroup();
    return group == nullptr ? nullptr : FromUtf8(env, group);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetAccessMask(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : static_cast<jint>(func->GetAccessMask());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetAuxiliary(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : reinterpret_cast<jlong>(func->GetAuxiliary());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetObjectType(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetObjectType());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetObjectName(JNIEnv *env,
                                                                                 jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *name = func->GetObjectName();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetName(JNIEnv *env,
                                                                            jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *name = func->GetName();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetNamespace(JNIEnv *env,
                                                                                jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *name = func->GetNamespace();
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetDeclaration(
        JNIEnv *env, jobject thiz, jboolean includeObjectName, jboolean includeNamespace,
        jboolean includeParamNames) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *decl = func->GetDeclaration(includeObjectName == JNI_TRUE,
                                            includeNamespace == JNI_TRUE,
                                            includeParamNames == JNI_TRUE);
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsReadOnly(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsReadOnly() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsPrivate(JNIEnv *env,
                                                                               jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsPrivate() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsProtected(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsProtected() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsFinal(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsFinal() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsOverride(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsOverride() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsShared(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsShared() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsExplicit(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsExplicit() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsProperty(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsProperty() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsVariadic(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsVariadic() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetParamCount(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : static_cast<jint>(func->GetParamCount());
}

JNIEXPORT jobject JNICALL Java_org_angelscript_ScriptFunction_nativeGetParam(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint index) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    int typeId = 0;
    asDWORD flags = 0;
    const char *name = nullptr;
    const char *defaultArg = nullptr;
    const int r = func->GetParam(static_cast<asUINT>(index), &typeId, &flags, &name, &defaultArg);
    if (r < 0) {
        return nullptr;
    }

    jclass cls = FindClass(env, "org/angelscript/ParamInfo");
    if (cls == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>", "(IILjava/lang/String;Ljava/lang/String;)V");
    if (ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return nullptr;
    }
    jobject result = env->NewObject(cls, ctor, typeId, static_cast<jint>(flags),
                                    name ? FromUtf8(env, name) : nullptr,
                                    defaultArg ? FromUtf8(env, defaultArg) : nullptr);
    env->DeleteLocalRef(cls);
    return result;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetReturnTypeId(JNIEnv *env,
                                                                                  jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->GetReturnTypeId();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetSubTypeCount(JNIEnv *env,
                                                                                 jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : static_cast<jint>(func->GetSubTypeCount());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetSubTypeId(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint subTypeIndex) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return func->GetSubTypeId(static_cast<asUINT>(subTypeIndex));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetSubType(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint subTypeIndex) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetSubType(static_cast<asUINT>(subTypeIndex)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetTypeId(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->GetTypeId();
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_ScriptFunction_nativeIsCompatibleWithTypeId(
        JNIEnv *env, jobject thiz, jint typeId) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func != nullptr && func->IsCompatibleWithTypeId(typeId) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetDelegateObject(JNIEnv *env,
                                                                                    jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : reinterpret_cast<jlong>(func->GetDelegateObject());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetDelegateObjectType(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetDelegateObjectType());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetDelegateFunction(JNIEnv *env,
                                                                                    jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetDelegateFunction());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetVarCount(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : static_cast<jint>(func->GetVarCount());
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetVarName(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint index) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *name = nullptr;
    int typeId = 0;
    if (func->GetVar(static_cast<asUINT>(index), &name, &typeId) < 0) {
        return nullptr;
    }
    return name == nullptr ? nullptr : FromUtf8(env, name);
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetVarDecl(
        JNIEnv *env, jobject thiz, jint index, jboolean includeNamespace) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *decl = func->GetVarDecl(static_cast<asUINT>(index),
                                         includeNamespace == JNI_TRUE);
    return decl == nullptr ? nullptr : FromUtf8(env, decl);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeFindNextLineWithCode(
        JNIEnv *env, jobject thiz, jint line) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    return func == nullptr ? 0 : func->FindNextLineWithCode(line);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetDeclaredAtRow(JNIEnv *env,
                                                                                 jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    const char *section = nullptr;
    int row = 0;
    int col = 0;
    func->GetDeclaredAt(&section, &row, &col);
    return row;
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptFunction_nativeGetDeclaredAtColumn(JNIEnv *env,
                                                                                    jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    const char *section = nullptr;
    int row = 0;
    int col = 0;
    func->GetDeclaredAt(&section, &row, &col);
    return col;
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptFunction_nativeGetDeclaredAtSection(
        JNIEnv *env, jobject thiz) {
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return nullptr;
    }
    const char *section = nullptr;
    int row = 0;
    int col = 0;
    func->GetDeclaredAt(&section, &row, &col);
    return section == nullptr ? nullptr : FromUtf8(env, section);
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeSetUserData(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong userData,
                                                                              jlong type) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->SetUserData(
            reinterpret_cast<void *>(static_cast<intptr_t>(userData)),
            static_cast<asPWORD>(type)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptFunction_nativeGetUserData(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong type) {
    (void)env;
    asIScriptFunction *func = FunctionOf(GetPtr(env, thiz));
    if (func == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(func->GetUserData(static_cast<asPWORD>(type)));
}

// ==================================================================== ScriptGeneric

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetEngine(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : reinterpret_cast<jlong>(generic->GetEngine());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetFunction(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetFunction());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetAuxiliary(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : reinterpret_cast<jlong>(generic->GetAuxiliary());
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetObject(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetObject());
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeGetObjectTypeId(JNIEnv *env,
                                                                              jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetObjectTypeId();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgCount(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetArgCount();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgTypeId(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetArgTypeId(static_cast<asUINT>(arg));
}

JNIEXPORT jbyte JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgByte(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetArgByte(static_cast<asUINT>(arg));
}

JNIEXPORT jshort JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgWord(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetArgWord(static_cast<asUINT>(arg));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgDWord(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0
                              : static_cast<jint>(generic->GetArgDWord(static_cast<asUINT>(arg)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgQWord(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0
                              : static_cast<jlong>(generic->GetArgQWord(static_cast<asUINT>(arg)));
}

JNIEXPORT jfloat JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgFloat(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0.0f : generic->GetArgFloat(static_cast<asUINT>(arg));
}

JNIEXPORT jdouble JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgDouble(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0.0 : generic->GetArgDouble(static_cast<asUINT>(arg));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgAddress(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetArgAddress(static_cast<asUINT>(arg)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgObject(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetArgObject(static_cast<asUINT>(arg)));
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetAddressOfArg(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jint arg) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetAddressOfArg(static_cast<asUINT>(arg)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeGetReturnTypeId(JNIEnv *env,
                                                                                jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? 0 : generic->GetReturnTypeId();
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnByte(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jbyte val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR : generic->SetReturnByte(val);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnWord(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jshort val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR : generic->SetReturnWord(val);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnDWord(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR : generic->SetReturnDWord(static_cast<asDWORD>(val));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnQWord(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jlong val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR
                              : generic->SetReturnQWord(static_cast<asQWORD>(val));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnFloat(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jfloat val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR : generic->SetReturnFloat(val);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnDouble(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jdouble val) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    return generic == nullptr ? asERROR : generic->SetReturnDouble(val);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnAddress(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jlong addr) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return asERROR;
    }
    return generic->SetReturnAddress(reinterpret_cast<void *>(static_cast<intptr_t>(addr)));
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnObject(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jlong obj) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return asERROR;
    }
    return generic->SetReturnObject(reinterpret_cast<void *>(static_cast<intptr_t>(obj)));
}

JNIEXPORT jstring JNICALL Java_org_angelscript_ScriptGeneric_nativeGetArgString(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint arg) {
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return nullptr;
    }
    void *address = generic->GetArgAddress(static_cast<asUINT>(arg));
    return ReadStringValue(env, generic->GetEngine(), address);
}

JNIEXPORT jint JNICALL Java_org_angelscript_ScriptGeneric_nativeSetReturnString(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jstring value) {
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return asERROR;
    }
    const std::string text = ToUtf8(env, value);
    const void *constant = CreateStringConstant(generic->GetEngine(), text);
    if (constant == nullptr) {
        return asINVALID_TYPE;
    }
    const int r = generic->SetReturnObject(const_cast<void *>(constant));
    return r;
}

JNIEXPORT jlong JNICALL Java_org_angelscript_ScriptGeneric_nativeGetAddressOfReturnLocation(
        JNIEnv *env, jobject thiz) {
    (void)env;
    asIScriptGeneric *generic = GenericOf(GetPtr(env, thiz));
    if (generic == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(generic->GetAddressOfReturnLocation());
}

// ==================================================================== LockableSharedBool

JNIEXPORT jint JNICALL Java_org_angelscript_LockableSharedBool_nativeAddRef(JNIEnv *env,
                                                                            jobject thiz) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    return shared == nullptr ? 0 : shared->AddRef();
}

JNIEXPORT jint JNICALL Java_org_angelscript_LockableSharedBool_nativeRelease(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    return shared == nullptr ? 0 : shared->Release();
}

JNIEXPORT jboolean JNICALL Java_org_angelscript_LockableSharedBool_nativeGet(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    return shared != nullptr && shared->Get() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_org_angelscript_LockableSharedBool_nativeSet(JNIEnv *env, jobject thiz,
                                                                         jboolean value) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    if (shared != nullptr) {
        shared->Set(value == JNI_TRUE);
    }
}

JNIEXPORT void JNICALL Java_org_angelscript_LockableSharedBool_nativeLock(JNIEnv *env,
                                                                           jobject thiz) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    if (shared != nullptr) {
        shared->Lock();
    }
}

JNIEXPORT void JNICALL Java_org_angelscript_LockableSharedBool_nativeUnlock(JNIEnv *env,
                                                                             jobject thiz) {
    (void)env;
    asILockableSharedBool *shared = reinterpret_cast<asILockableSharedBool *>(GetPtr(env, thiz));
    if (shared != nullptr) {
        shared->Unlock();
    }
}

}  // extern "C"
