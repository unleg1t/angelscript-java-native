// JNI bindings for org.angelscript.Addons.
#include "asjava.h"

#include <chrono>

#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scriptbuilder/scriptbuilder.h"
#include "datetime/datetime.h"
#include "scriptdictionary/scriptdictionary.h"
#ifdef AS_JNI_WITH_FILE_SYSTEM
#include "scriptfile/scriptfilesystem.h"
#endif
#include "scriptgrid/scriptgrid.h"
#include "scripthandle/scripthandle.h"
#include "scripthelper/scripthelper.h"
#include "scriptmath/scriptmath.h"
#include "scriptstdstring/scriptstdstring.h"
#include "contextmgr/contextmgr.h"
#include "weakref/weakref.h"

using namespace asjava;

namespace {

asIScriptEngine *EngineOf(jlong ptr) {
    return reinterpret_cast<asIScriptEngine *>(ptr);
}

// Provides the time in milliseconds to the context manager.
asUINT GetTimeMillis() {
    using namespace std::chrono;
    return static_cast<asUINT>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

}  // namespace

extern "C" {

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterStdString(JNIEnv *env, jclass,
                                                                            jlong engine) {
    (void)env;
    RegisterStdString(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptArray(JNIEnv *env, jclass,
                                                                              jlong engine,
                                                                              jboolean defaultArray) {
    (void)env;
    RegisterScriptArray(EngineOf(engine), defaultArray == JNI_TRUE);
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptDictionary(JNIEnv *env,
                                                                                   jclass,
                                                                                   jlong engine) {
    (void)env;
    RegisterScriptDictionary(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptAny(JNIEnv *env, jclass,
                                                                            jlong engine) {
    (void)env;
    RegisterScriptAny(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptMath(JNIEnv *env, jclass,
                                                                            jlong engine) {
    (void)env;
    RegisterScriptMath(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptDateTime(JNIEnv *env,
                                                                                 jclass,
                                                                                 jlong engine) {
    (void)env;
    RegisterScriptDateTime(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptHandle(JNIEnv *env, jclass,
                                                                              jlong engine) {
    (void)env;
    RegisterScriptHandle(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptFileSystem(JNIEnv *env,
                                                                                  jclass,
                                                                                  jlong engine) {
#ifdef AS_JNI_WITH_FILE_SYSTEM
    (void)env;
    RegisterScriptFileSystem(EngineOf(engine));
#else
    (void)engine;
    ThrowJava(env, "java/lang/UnsupportedOperationException",
              "This build of the native library does not include the file system add-on");
#endif
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptGrid(JNIEnv *env, jclass,
                                                                            jlong engine) {
    (void)env;
    RegisterScriptGrid(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterScriptWeakRef(JNIEnv *env,
                                                                                jclass,
                                                                                jlong engine) {
    (void)env;
    RegisterScriptWeakRef(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterExceptionRoutines(JNIEnv *env,
                                                                                    jclass,
                                                                                    jlong engine) {
    (void)env;
    RegisterExceptionRoutines(EngineOf(engine));
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterThreadSupport(JNIEnv *env,
                                                                                jclass,
                                                                                jlong engine) {
    (void)env;
    asIScriptEngine *scriptEngine = EngineOf(engine);
    EngineState *state = GetEngineState(scriptEngine);
    if (state == nullptr) {
        return;
    }
    CContextMgr *manager = new CContextMgr();
    manager->SetGetTimeCallback(GetTimeMillis);
    manager->RegisterThreadSupport(scriptEngine);
    OwnedResource resource;
    resource.ptr = manager;
    resource.deleter = [](void *ptr) { delete static_cast<CContextMgr *>(ptr); };
    state->owned.push_back(resource);
}

JNIEXPORT void JNICALL Java_org_angelscript_Addons_nativeRegisterCoRoutineSupport(JNIEnv *env,
                                                                                  jclass,
                                                                                  jlong engine) {
    (void)env;
    asIScriptEngine *scriptEngine = EngineOf(engine);
    EngineState *state = GetEngineState(scriptEngine);
    if (state == nullptr) {
        return;
    }
    CContextMgr *manager = new CContextMgr();
    manager->SetGetTimeCallback(GetTimeMillis);
    manager->RegisterCoRoutineSupport(scriptEngine);
    OwnedResource resource;
    resource.ptr = manager;
    resource.deleter = [](void *ptr) { delete static_cast<CContextMgr *>(ptr); };
    state->owned.push_back(resource);
}

}  // extern "C"
