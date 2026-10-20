// Java to AngelScript interoperability.
//
// Java classes can be registered with the script engine as reference types. The script engine
// then sees them as ordinary AngelScript objects and can call their public methods.
#include "asjava.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace asjava;

namespace {

// The kinds of values that can be passed between Java and AngelScript.
enum JavaKind {
    KIND_VOID = 0,
    KIND_BOOL = 1,
    KIND_INT8 = 2,
    KIND_INT16 = 3,
    KIND_INT32 = 4,
    KIND_INT64 = 5,
    KIND_UINT8 = 6,
    KIND_FLOAT = 7,
    KIND_DOUBLE = 8,
    KIND_JAVA = 9,
    KIND_STRING = 10
};

struct JavaTypeInfo;

// A Java object that is exposed to the script engine.
struct JavaObject {
    jobject ref = nullptr;  // global reference to the Java object
    int refCount = 0;
    JavaVM *vm = nullptr;
    JavaTypeInfo *type = nullptr;
};

// The description of a single value that crosses the Java/AngelScript boundary.
struct JavaParam {
    int kind = KIND_VOID;
    JavaTypeInfo *javaType = nullptr;  // only for KIND_JAVA
};

struct JavaMethodInfo {
    std::string name;
    jmethodID method = nullptr;
    JavaParam returnParam;
    std::vector<JavaParam> params;
};

struct JavaTypeInfo {
    JavaVM *vm = nullptr;
    asIScriptEngine *engine = nullptr;
    std::string scriptName;
    jclass javaClass = nullptr;  // global reference
    std::vector<std::unique_ptr<JavaMethodInfo>> methods;
    std::vector<JavaObject *> objects;
};

struct JavaRegistry {
    asIScriptEngine *engine = nullptr;
    std::vector<JavaTypeInfo *> types;
    std::unordered_map<std::string, JavaTypeInfo *> byName;
};

std::mutex &RegistryMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_map<asIScriptEngine *, JavaRegistry *> &Registries() {
    static std::unordered_map<asIScriptEngine *, JavaRegistry *> map;
    return map;
}

JavaRegistry *GetRegistry(asIScriptEngine *engine) {
    std::lock_guard<std::mutex> lock(RegistryMutex());
    auto it = Registries().find(engine);
    if (it == Registries().end()) {
        return nullptr;
    }
    return it->second;
}

JavaTypeInfo *FindTypeByName(asIScriptEngine *engine, const std::string &name) {
    JavaRegistry *registry = GetRegistry(engine);
    if (registry == nullptr) {
        return nullptr;
    }
    auto it = registry->byName.find(name);
    return it == registry->byName.end() ? nullptr : it->second;
}

// ---------------------------------------------------------------------------
// String helpers
// ---------------------------------------------------------------------------

asIStringFactory *GetStringFactory(asIScriptEngine *engine) {
    asDWORD flags = 0;
    asIStringFactory *factory = nullptr;
    if (engine == nullptr) {
        return nullptr;
    }
    // The return value is the type id of the string type, so the factory pointer must be checked.
    engine->GetStringFactory(&flags, &factory);
    return factory;
}

// Reads an AngelScript string value into a Java string.
jstring ReadScriptString(JNIEnv *env, asIScriptEngine *engine, void *value) {
    if (value == nullptr) {
        return nullptr;
    }
    asIStringFactory *factory = GetStringFactory(engine);
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

// ---------------------------------------------------------------------------
// Trampolines
// ---------------------------------------------------------------------------

void JavaFactoryTrampoline(asIScriptGeneric *gen) {
    JavaTypeInfo *type = static_cast<JavaTypeInfo *>(gen->GetAuxiliary());
    if (type == nullptr) {
        return;
    }

    JNIEnv *env = AttachEnv(type->vm);
    if (env == nullptr) {
        return;
    }

    jclass cls = type->javaClass;
    jmethodID ctor = env->GetMethodID(cls, "<init>", "()V");
    jobject instance = nullptr;
    if (ctor != nullptr) {
        instance = env->NewObject(cls, ctor);
    }
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        DetachEnv(type->vm);
        return;
    }

    if (instance == nullptr) {
        DetachEnv(type->vm);
        return;
    }

    JavaObject *wrapper = new JavaObject();
    wrapper->ref = env->NewGlobalRef(instance);
    wrapper->refCount = 1;
    wrapper->vm = type->vm;
    wrapper->type = type;
    env->DeleteLocalRef(instance);

    {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        type->objects.push_back(wrapper);
    }

    *(JavaObject **)gen->GetAddressOfReturnLocation() = wrapper;
    DetachEnv(type->vm);
}

void JavaAddRefTrampoline(asIScriptGeneric *gen) {
    JavaObject *object = static_cast<JavaObject *>(gen->GetObject());
    if (object != nullptr) {
        object->refCount++;
    }
}

void JavaReleaseTrampoline(asIScriptGeneric *gen) {
    JavaObject *object = static_cast<JavaObject *>(gen->GetObject());
    if (object == nullptr) {
        return;
    }
    object->refCount--;
    if (object->refCount > 0) {
        return;
    }

    JavaTypeInfo *type = object->type;
    JNIEnv *env = object->vm != nullptr ? AttachEnv(object->vm) : nullptr;
    if (env != nullptr && object->ref != nullptr) {
        env->DeleteGlobalRef(object->ref);
        DetachEnv(object->vm);
    }
    object->ref = nullptr;

    if (type != nullptr) {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        for (size_t i = 0; i < type->objects.size(); ++i) {
            if (type->objects[i] == object) {
                type->objects.erase(type->objects.begin() + static_cast<long>(i));
                break;
            }
        }
    }
    delete object;
}

// Reads a single argument of the current call and converts it to a Java value.
jobject ConvertArgument(JNIEnv *env, asIScriptGeneric *gen, const JavaParam &param, int index,
                        jvalue *value) {
    switch (param.kind) {
        case KIND_BOOL:
            value->z = gen->GetArgByte(static_cast<asUINT>(index)) != 0 ? JNI_TRUE : JNI_FALSE;
            return nullptr;
        case KIND_INT8:
            value->b = gen->GetArgByte(static_cast<asUINT>(index));
            return nullptr;
        case KIND_INT16:
            value->s = gen->GetArgWord(static_cast<asUINT>(index));
            return nullptr;
        case KIND_INT32:
            value->i = static_cast<jint>(gen->GetArgDWord(static_cast<asUINT>(index)));
            return nullptr;
        case KIND_INT64:
            value->j = static_cast<jlong>(gen->GetArgQWord(static_cast<asUINT>(index)));
            return nullptr;
        case KIND_UINT8:
            value->c = static_cast<jchar>(gen->GetArgWord(static_cast<asUINT>(index)));
            return nullptr;
        case KIND_FLOAT:
            value->f = gen->GetArgFloat(static_cast<asUINT>(index));
            return nullptr;
        case KIND_DOUBLE:
            value->d = gen->GetArgDouble(static_cast<asUINT>(index));
            return nullptr;
        case KIND_JAVA: {
            JavaObject *object = static_cast<JavaObject *>(gen->GetArgObject(static_cast<asUINT>(index)));
            if (object == nullptr || object->ref == nullptr) {
                value->l = nullptr;
                return nullptr;
            }
            jobject local = env->NewLocalRef(object->ref);
            value->l = local;
            return local;
        }
        case KIND_STRING: {
            void *address = gen->GetArgAddress(static_cast<asUINT>(index));
            jstring text = ReadScriptString(env, gen->GetEngine(), address);
            value->l = text;
            return text;
        }
        default:
            value->l = nullptr;
            return nullptr;
    }
}

void JavaMethodTrampoline(asIScriptGeneric *gen) {
    JavaMethodInfo *info = static_cast<JavaMethodInfo *>(gen->GetAuxiliary());
    if (info == nullptr) {
        return;
    }
    JavaObject *object = static_cast<JavaObject *>(gen->GetObject());
    if (object == nullptr || object->ref == nullptr) {
        return;
    }
    JavaTypeInfo *type = object->type;

    JNIEnv *env = AttachEnv(type != nullptr ? type->vm : nullptr);
    if (env == nullptr) {
        return;
    }

    const size_t count = info->params.size();
    std::vector<jvalue> args(count);
    std::vector<jobject> locals(count, nullptr);
    for (size_t i = 0; i < count; ++i) {
        locals[i] = ConvertArgument(env, gen, info->params[i], static_cast<int>(i), &args[i]);
    }

    jvalue result;
    result.z = JNI_FALSE;
    result.l = nullptr;

    // The object result is kept separately, the jvalue union is reused for the primitive results.
    jobject objectResult = nullptr;
    switch (info->returnParam.kind) {
        case KIND_VOID:
            env->CallVoidMethodA(object->ref, info->method, args.data());
            break;
        case KIND_BOOL:
            result.z = env->CallBooleanMethodA(object->ref, info->method, args.data());
            break;
        case KIND_INT8:
            result.b = env->CallByteMethodA(object->ref, info->method, args.data());
            break;
        case KIND_INT16:
            result.s = env->CallShortMethodA(object->ref, info->method, args.data());
            break;
        case KIND_INT32:
            result.i = env->CallIntMethodA(object->ref, info->method, args.data());
            break;
        case KIND_INT64:
            result.j = env->CallLongMethodA(object->ref, info->method, args.data());
            break;
        case KIND_UINT8:
            result.c = env->CallCharMethod(object->ref, info->method, args.data());
            break;
        case KIND_FLOAT:
            result.f = env->CallFloatMethodA(object->ref, info->method, args.data());
            break;
        case KIND_DOUBLE:
            result.d = env->CallDoubleMethodA(object->ref, info->method, args.data());
            break;
        default:
            objectResult = env->CallObjectMethodA(object->ref, info->method, args.data());
            result.l = objectResult;
            break;
    }

    bool failed = false;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        failed = true;
    }

    for (size_t i = 0; i < count; ++i) {
        if (locals[i] != nullptr) {
            env->DeleteLocalRef(locals[i]);
        }
    }

    if (failed) {
        DetachEnv(type != nullptr ? type->vm : nullptr);
        return;
    }

    // Store the return value.
    switch (info->returnParam.kind) {
        case KIND_BOOL:
            gen->SetReturnByte(result.z == JNI_TRUE ? 1 : 0);
            break;
        case KIND_INT8:
            gen->SetReturnByte(static_cast<asBYTE>(result.b));
            break;
        case KIND_INT16:
            gen->SetReturnWord(static_cast<asWORD>(result.s));
            break;
        case KIND_INT32:
            gen->SetReturnDWord(static_cast<asDWORD>(result.i));
            break;
        case KIND_INT64:
            gen->SetReturnQWord(static_cast<asQWORD>(result.j));
            break;
        case KIND_UINT8:
            gen->SetReturnWord(static_cast<asWORD>(result.c));
            break;
        case KIND_FLOAT:
            gen->SetReturnFloat(result.f);
            break;
        case KIND_DOUBLE:
            gen->SetReturnDouble(result.d);
            break;
        case KIND_JAVA: {
            if (objectResult == nullptr) {
                break;
            }
            JavaTypeInfo *returnType = info->returnParam.javaType;
            JavaObject *wrapper = new JavaObject();
            wrapper->ref = env->NewGlobalRef(objectResult);
            wrapper->refCount = 0;
            wrapper->vm = type != nullptr ? type->vm : nullptr;
            wrapper->type = returnType;
            if (returnType != nullptr) {
                std::lock_guard<std::mutex> lock(RegistryMutex());
                returnType->objects.push_back(wrapper);
            }
            *(JavaObject **)gen->GetAddressOfReturnLocation() = wrapper;
            break;
        }
        case KIND_STRING: {
            if (objectResult == nullptr) {
                break;
            }
            const char *utf8 =
                    env->GetStringUTFChars(static_cast<jstring>(objectResult), nullptr);
            std::string text = utf8 != nullptr ? utf8 : "";
            if (utf8 != nullptr) {
                env->ReleaseStringUTFChars(static_cast<jstring>(objectResult), utf8);
            }
            asIStringFactory *factory = GetStringFactory(gen->GetEngine());
            if (factory != nullptr) {
                const void *constant = factory->GetStringConstant(text.c_str(),
                                                                   static_cast<asUINT>(text.size()));
                if (constant != nullptr) {
                    gen->SetReturnObject(const_cast<void *>(constant));
                    factory->ReleaseStringConstant(constant);
                }
            }
            break;
        }
        default:
            break;
    }

    if (objectResult != nullptr) {
        env->DeleteLocalRef(objectResult);
    }
    DetachEnv(type != nullptr ? type->vm : nullptr);
}

}  // namespace

namespace asjava {

void DestroyJavaTypes(asIScriptEngine *engine) {
    JavaRegistry *registry = nullptr;
    {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        auto it = Registries().find(engine);
        if (it == Registries().end()) {
            return;
        }
        registry = it->second;
        Registries().erase(it);
    }

    for (JavaTypeInfo *type : registry->types) {
        JNIEnv *env = AttachEnv(type->vm);
        for (JavaObject *object : type->objects) {
            if (env != nullptr && object->ref != nullptr) {
                env->DeleteGlobalRef(object->ref);
            }
            delete object;
        }
        type->objects.clear();
        if (env != nullptr) {
            if (type->javaClass != nullptr) {
                env->DeleteGlobalRef(type->javaClass);
            }
            DetachEnv(type->vm);
        }
        delete type;
    }
    delete registry;
}

}  // namespace asjava

extern "C" {

// Registers a Java class with the engine. The method descriptions are computed on the Java side.
JNIEXPORT jlong JNICALL Java_org_angelscript_JavaType_nativeRegister(
        JNIEnv *env, jclass, jlong enginePtr, jstring scriptName, jclass javaClass,
        jobjectArray methods) {
    asIScriptEngine *engine = reinterpret_cast<asIScriptEngine *>(enginePtr);
    if (engine == nullptr) {
        return 0;
    }

    const std::string name = ToUtf8(env, scriptName);
    JavaTypeInfo *type = new JavaTypeInfo();
    env->GetJavaVM(&type->vm);
    type->engine = engine;
    type->scriptName = name;
    type->javaClass = static_cast<jclass>(env->NewGlobalRef(javaClass));

    // Register the object type.
    int r = engine->RegisterObjectType(name.c_str(), sizeof(JavaObject), asOBJ_REF);
    if (r < 0) {
        env->DeleteGlobalRef(type->javaClass);
        delete type;
        ThrowJava(env, "java/lang/IllegalStateException",
                  "Could not register the Java type '" + name + "': " + std::to_string(r));
        return 0;
    }

    // The factory creates new instances through the no argument constructor.
    const std::string factoryDecl = name + "@ f()";
    r = engine->RegisterObjectBehaviour(name.c_str(), asBEHAVE_FACTORY, factoryDecl.c_str(),
                                        MakeGenericFuncPtr(reinterpret_cast<void *>(&JavaFactoryTrampoline)),
                                        asCALL_GENERIC, type);
    if (r < 0) {
        env->DeleteGlobalRef(type->javaClass);
        delete type;
        ThrowJava(env, "java/lang/IllegalStateException",
                  "Could not register the factory of the Java type '" + name + "': "
                          + std::to_string(r));
        return 0;
    }

    r = engine->RegisterObjectBehaviour(name.c_str(), asBEHAVE_ADDREF, "void f()",
                                        MakeGenericFuncPtr(reinterpret_cast<void *>(&JavaAddRefTrampoline)),
                                        asCALL_GENERIC, type);
    if (r >= 0) {
        r = engine->RegisterObjectBehaviour(name.c_str(), asBEHAVE_RELEASE, "void f()",
                                            MakeGenericFuncPtr(reinterpret_cast<void *>(&JavaReleaseTrampoline)),
                                            asCALL_GENERIC, type);
        if (r < 0) {
            env->DeleteGlobalRef(type->javaClass);
            delete type;
            ThrowJava(env, "java/lang/IllegalStateException",
                      "Could not register the release behaviour of the Java type '" + name + "': "
                              + std::to_string(r));
            return 0;
        }
    } else {
        env->DeleteGlobalRef(type->javaClass);
        delete type;
        ThrowJava(env, "java/lang/IllegalStateException",
                  "Could not register the addref behaviour of the Java type '" + name + "': "
                          + std::to_string(r));
        return 0;
    }

    // Register the methods.
    std::string failedDeclarations;
    const jsize count = methods != nullptr ? env->GetArrayLength(methods) : 0;
    jclass descriptorCls = env->FindClass("org/angelscript/JavaType$MethodDescriptor");
    if (descriptorCls != nullptr) {
        jfieldID fName = env->GetFieldID(descriptorCls, "name", "Ljava/lang/String;");
        jfieldID fDeclaration = env->GetFieldID(descriptorCls, "declaration", "Ljava/lang/String;");
        jfieldID fSignature = env->GetFieldID(descriptorCls, "jniSignature", "Ljava/lang/String;");
        jfieldID fReturnKind = env->GetFieldID(descriptorCls, "returnKind", "I");
        jfieldID fReturnType = env->GetFieldID(descriptorCls, "returnTypeName", "Ljava/lang/String;");
        jfieldID fParamKinds = env->GetFieldID(descriptorCls, "paramKinds", "[I");
        jfieldID fParamTypes = env->GetFieldID(descriptorCls, "paramTypeNames", "[Ljava/lang/String;");

        for (jsize i = 0; i < count; ++i) {
            jobject descriptor = env->GetObjectArrayElement(methods, i);
            if (descriptor == nullptr) {
                continue;
            }

            JavaMethodInfo method;
            method.name = ToUtf8(env, static_cast<jstring>(env->GetObjectField(descriptor, fName)));
            const std::string signature =
                    ToUtf8(env, static_cast<jstring>(env->GetObjectField(descriptor, fSignature)));
            const std::string declaration = ToUtf8(
                    env, static_cast<jstring>(env->GetObjectField(descriptor, fDeclaration)));
            const jint returnKind = env->GetIntField(descriptor, fReturnKind);
            jstring returnTypeName =
                    static_cast<jstring>(env->GetObjectField(descriptor, fReturnType));

            method.returnParam.kind = returnKind;
            if (returnKind == KIND_JAVA && returnTypeName != nullptr) {
                method.returnParam.javaType =
                        FindTypeByName(engine, ToUtf8(env, returnTypeName));
            }

            jintArray kinds = static_cast<jintArray>(env->GetObjectField(descriptor, fParamKinds));
            jobjectArray typeNames =
                    static_cast<jobjectArray>(env->GetObjectField(descriptor, fParamTypes));
            const jsize paramCount = kinds != nullptr ? env->GetArrayLength(kinds) : 0;
            std::vector<jint> kindValues(paramCount);
            if (paramCount > 0) {
                env->GetIntArrayRegion(kinds, 0, paramCount, kindValues.data());
            }
            for (jsize p = 0; p < paramCount; ++p) {
                JavaParam param;
                param.kind = kindValues[p];
                if (param.kind == KIND_JAVA && typeNames != nullptr) {
                    jstring typeName = static_cast<jstring>(env->GetObjectArrayElement(typeNames, p));
                    if (typeName != nullptr) {
                        param.javaType = FindTypeByName(engine, ToUtf8(env, typeName));
                        env->DeleteLocalRef(typeName);
                    }
                }
                method.params.push_back(param);
            }

            // The method is looked up on the Java class, not on the descriptor.
            jmethodID methodId = env->GetMethodID(type->javaClass, method.name.c_str(),
                                                   signature.c_str());
            if (methodId == nullptr) {
                env->ExceptionClear();
                env->DeleteLocalRef(descriptor);
                continue;
            }
            method.method = methodId;

            type->methods.push_back(std::unique_ptr<JavaMethodInfo>(new JavaMethodInfo(method)));
            r = engine->RegisterObjectMethod(name.c_str(), declaration.c_str(),
                                             MakeGenericFuncPtr(reinterpret_cast<void *>(
                                                     &JavaMethodTrampoline)),
                                             asCALL_GENERIC, type->methods.back().get());
            env->DeleteLocalRef(descriptor);
            if (r < 0) {
                // The method could not be registered, remove it from the list again.
                type->methods.pop_back();
                failedDeclarations += declaration + " (" + std::to_string(r) + "); ";
            }
        }
        env->DeleteLocalRef(descriptorCls);
    }

    if (!failedDeclarations.empty()) {
        ThrowJava(env, "java/lang/IllegalStateException",
                  "Could not register the methods of the Java type '" + name + "': "
                          + failedDeclarations);
    }

    {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        JavaRegistry *registry = nullptr;
        auto it = Registries().find(engine);
        if (it == Registries().end()) {
            registry = new JavaRegistry();
            registry->engine = engine;
            Registries()[engine] = registry;
        } else {
            registry = it->second;
        }
        registry->types.push_back(type);
        registry->byName[name] = type;
    }

    return reinterpret_cast<jlong>(type);
}

// Wraps a Java object so that it can be passed to the script engine.
JNIEXPORT jlong JNICALL Java_org_angelscript_JavaType_nativeWrap(JNIEnv *env, jclass,
                                                                  jlong typePtr, jobject instance) {
    JavaTypeInfo *type = reinterpret_cast<JavaTypeInfo *>(typePtr);
    if (type == nullptr || instance == nullptr) {
        return 0;
    }

    JavaObject *wrapper = new JavaObject();
    wrapper->ref = env->NewGlobalRef(instance);
    wrapper->refCount = 1;
    wrapper->vm = type->vm;
    wrapper->type = type;

    {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        type->objects.push_back(wrapper);
    }
    return reinterpret_cast<jlong>(wrapper);
}

// Unwraps a native handle back into the Java object it refers to.
JNIEXPORT jobject JNICALL Java_org_angelscript_JavaType_nativeUnwrap(JNIEnv *env, jclass,
                                                                     jlong typePtr, jlong handle) {
    (void)typePtr;
    JavaObject *wrapper = reinterpret_cast<JavaObject *>(handle);
    if (wrapper == nullptr || wrapper->ref == nullptr) {
        return nullptr;
    }
    return env->NewLocalRef(wrapper->ref);
}

// Releases the reference the Java side holds on a wrapped object.
JNIEXPORT void JNICALL Java_org_angelscript_JavaType_nativeRelease(JNIEnv *env, jclass,
                                                                    jlong typePtr, jlong handle) {
    JavaTypeInfo *type = reinterpret_cast<JavaTypeInfo *>(typePtr);
    JavaObject *wrapper = reinterpret_cast<JavaObject *>(handle);
    if (wrapper == nullptr) {
        return;
    }
    wrapper->refCount--;
    if (wrapper->refCount > 0) {
        return;
    }
    if (wrapper->ref != nullptr) {
        JNIEnv *local = wrapper->vm != nullptr ? AttachEnv(wrapper->vm) : env;
        if (local != nullptr) {
            local->DeleteGlobalRef(wrapper->ref);
            if (local != env && wrapper->vm != nullptr) {
                DetachEnv(wrapper->vm);
            }
        }
        wrapper->ref = nullptr;
    }
    if (type != nullptr) {
        std::lock_guard<std::mutex> lock(RegistryMutex());
        for (size_t i = 0; i < type->objects.size(); ++i) {
            if (type->objects[i] == wrapper) {
                type->objects.erase(type->objects.begin() + static_cast<long>(i));
                break;
            }
        }
    }
    delete wrapper;
}

// Resolves the AngelScript type name for a Java class.
JNIEXPORT jstring JNICALL Java_org_angelscript_JavaType_nativeResolve(JNIEnv *env, jclass,
                                                                        jlong enginePtr,
                                                                        jclass javaClass) {
    asIScriptEngine *engine = reinterpret_cast<asIScriptEngine *>(enginePtr);
    if (engine == nullptr || javaClass == nullptr) {
        return nullptr;
    }

    // Primitives.
    jclass booleanClass = env->FindClass("java/lang/Boolean");
    jclass byteClass = env->FindClass("java/lang/Byte");
    jclass characterClass = env->FindClass("java/lang/Character");
    jclass shortClass = env->FindClass("java/lang/Short");
    jclass integerClass = env->FindClass("java/lang/Integer");
    jclass longClass = env->FindClass("java/lang/Long");
    jclass floatClass = env->FindClass("java/lang/Float");
    jclass doubleClass = env->FindClass("java/lang/Double");
    jclass stringClass = env->FindClass("java/lang/String");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }

    const char *result = nullptr;
    if (booleanClass != nullptr && env->IsSameObject(javaClass, booleanClass)) {
        result = "bool";
    } else if (byteClass != nullptr && env->IsSameObject(javaClass, byteClass)) {
        result = "int8";
    } else if (characterClass != nullptr && env->IsSameObject(javaClass, characterClass)) {
        result = "uint16";
    } else if (shortClass != nullptr && env->IsSameObject(javaClass, shortClass)) {
        result = "int16";
    } else if (integerClass != nullptr && env->IsSameObject(javaClass, integerClass)) {
        result = "int";
    } else if (longClass != nullptr && env->IsSameObject(javaClass, longClass)) {
        result = "int64";
    } else if (floatClass != nullptr && env->IsSameObject(javaClass, floatClass)) {
        result = "float";
    } else if (doubleClass != nullptr && env->IsSameObject(javaClass, doubleClass)) {
        result = "double";
    } else if (stringClass != nullptr && env->IsSameObject(javaClass, stringClass)) {
        // Only usable when the engine has a string type registered.
        if (GetStringFactory(engine) != nullptr &&
            engine->GetTypeInfoByName("string") != nullptr) {
            result = "string";
        }
    }

    const jclass classes[] = {booleanClass, byteClass, characterClass, shortClass, integerClass,
                               longClass, floatClass, doubleClass, stringClass};
    for (jclass cls : classes) {
        if (cls != nullptr) {
            env->DeleteLocalRef(cls);
        }
    }

    if (result != nullptr) {
        return FromUtf8(env, result);
    }

    // A registered Java type.
    JavaRegistry *registry = GetRegistry(engine);
    if (registry != nullptr) {
        for (JavaTypeInfo *type : registry->types) {
            if (env->IsSameObject(javaClass, type->javaClass)) {
                return FromUtf8(env, type->scriptName);
            }
        }
    }
    return nullptr;
}

// Returns the AngelScript type id of a registered Java type.
JNIEXPORT jint JNICALL Java_org_angelscript_JavaType_nativeTypeId(JNIEnv *env, jclass,
                                                                   jlong typePtr) {
    (void)env;
    JavaTypeInfo *type = reinterpret_cast<JavaTypeInfo *>(typePtr);
    if (type == nullptr) {
        return 0;
    }
    return type->engine->GetTypeIdByDecl(type->scriptName.c_str());
}

// Returns the number of live wrappers for a registered Java type.
JNIEXPORT jint JNICALL Java_org_angelscript_JavaType_nativeObjectCount(JNIEnv *env, jclass,
                                                                        jlong typePtr) {
    (void)env;
    JavaTypeInfo *type = reinterpret_cast<JavaTypeInfo *>(typePtr);
    if (type == nullptr) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(RegistryMutex());
    return static_cast<jint>(type->objects.size());
}

}  // extern "C"
