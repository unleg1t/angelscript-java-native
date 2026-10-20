// Bridges for Java implemented engine extensions: string factories and byte streams.
#include "asjava.h"

#include <string>
#include <unordered_map>
#include <vector>

using namespace asjava;

namespace {

// ---------------------------------------------------------------------------
// String factory
// ---------------------------------------------------------------------------

struct StringConstant {
    std::string data;
    jobject ref = nullptr;  // global reference to the Java side constant
};

class JavaStringFactory : public asIStringFactory {
public:
    JavaStringFactory(JNIEnv *env, jobject factory) : vm_(nullptr), factory_(nullptr) {
        env->GetJavaVM(&vm_);
        factory_ = env->NewGlobalRef(factory);

        jclass cls = env->GetObjectClass(factory);
        if (cls != nullptr) {
            getStringConstant_ = env->GetMethodID(cls, "getStringConstant",
                                                 "(Ljava/lang/String;)Ljava/lang/Object;");
            releaseStringConstant_ = env->GetMethodID(cls, "releaseStringConstant",
                                                      "(Ljava/lang/Object;)V");
            getRawStringData_ = env->GetMethodID(cls, "getRawStringData",
                                                 "(Ljava/lang/Object;[B)I");
            env->DeleteLocalRef(cls);
        }
    }

    ~JavaStringFactory() override {
        JNIEnv *env = AttachEnv(vm_);
        if (env != nullptr) {
            for (auto &entry : constants_) {
                if (entry.second->ref != nullptr) {
                    env->DeleteGlobalRef(entry.second->ref);
                }
                delete entry.second;
            }
            if (factory_ != nullptr) {
                env->DeleteGlobalRef(factory_);
            }
            DetachEnv(vm_);
        }
    }

    const void *GetStringConstant(const char *data, asUINT length) override {
        JNIEnv *env = AttachEnv(vm_);
        if (env == nullptr || getStringConstant_ == nullptr) {
            return nullptr;
        }

        std::string text(data != nullptr ? data : "", data != nullptr ? length : 0);
        jstring javaString = FromUtf8(env, text);
        jobject constant = env->CallObjectMethod(factory_, getStringConstant_, javaString);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            if (javaString != nullptr) {
                env->DeleteLocalRef(javaString);
            }
            return nullptr;
        }
        if (javaString != nullptr) {
            env->DeleteLocalRef(javaString);
        }

        StringConstant *entry = new StringConstant();
        entry->data = text;
        entry->ref = constant != nullptr ? env->NewGlobalRef(constant) : nullptr;
        constants_[entry->data.c_str()] = entry;
        return entry->data.c_str();
    }

    int ReleaseStringConstant(const void *str) override {
        auto it = constants_.find(str);
        if (it == constants_.end()) {
            return -1;
        }
        JNIEnv *env = AttachEnv(vm_);
        if (env != nullptr) {
            if (it->second->ref != nullptr) {
                if (releaseStringConstant_ != nullptr) {
                    env->CallVoidMethod(factory_, releaseStringConstant_, it->second->ref);
                    if (env->ExceptionCheck()) {
                        env->ExceptionClear();
                    }
                }
                env->DeleteGlobalRef(it->second->ref);
            }
        }
        delete it->second;
        constants_.erase(it);
        return 0;
    }

    int GetRawStringData(const void *str, char *data, asUINT *length) const override {
        auto it = constants_.find(str);
        if (it == constants_.end()) {
            return -1;
        }
        JNIEnv *env = AttachEnv(vm_);
        if (env == nullptr) {
            return -1;
        }

        const size_t size = it->second->data.size();
        jbyteArray buffer = env->NewByteArray(static_cast<jsize>(size));
        if (buffer == nullptr) {
            env->ExceptionClear();
            return -1;
        }
        jint written = 0;
        if (getRawStringData_ != nullptr) {
            written = env->CallIntMethod(factory_, getRawStringData_, it->second->ref, buffer);
        }
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(buffer);
            return -1;
        }
        if (written < 0) {
            env->DeleteLocalRef(buffer);
            return written;
        }
        if (data != nullptr && length != nullptr) {
            env->GetByteArrayRegion(buffer, 0, written, reinterpret_cast<jbyte *>(data));
            *length = static_cast<asUINT>(written);
        }
        env->DeleteLocalRef(buffer);
        return 0;
    }

private:
    JavaVM *vm_ = nullptr;
    jobject factory_ = nullptr;
    jmethodID getStringConstant_ = nullptr;
    jmethodID releaseStringConstant_ = nullptr;
    jmethodID getRawStringData_ = nullptr;
    mutable std::unordered_map<const void *, StringConstant *> constants_;
};

// ---------------------------------------------------------------------------
// Byte stream
// ---------------------------------------------------------------------------

class JavaBinaryStream : public asIBinaryStream {
public:
    JavaBinaryStream(JNIEnv *env, jobject stream) : vm_(nullptr), stream_(nullptr) {
        env->GetJavaVM(&vm_);
        stream_ = env->NewGlobalRef(stream);

        jclass cls = env->GetObjectClass(stream);
        if (cls != nullptr) {
            read_ = env->GetMethodID(cls, "read", "([BII)I");
            write_ = env->GetMethodID(cls, "write", "([BII)I");
            env->DeleteLocalRef(cls);
        }
    }

    ~JavaBinaryStream() override {
        JNIEnv *env = AttachEnv(vm_);
        if (env != nullptr) {
            if (stream_ != nullptr) {
                env->DeleteGlobalRef(stream_);
            }
            DetachEnv(vm_);
        }
    }

    int Read(void *ptr, asUINT size) override {
        if (ptr == nullptr || size == 0) {
            return 0;
        }
        JNIEnv *env = AttachEnv(vm_);
        if (env == nullptr || read_ == nullptr) {
            return -1;
        }

        jbyteArray buffer = env->NewByteArray(static_cast<jsize>(size));
        if (buffer == nullptr) {
            env->ExceptionClear();
            return -1;
        }
        const jint read = env->CallIntMethod(stream_, read_, buffer, 0, static_cast<jint>(size));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(buffer);
            return -1;
        }
        if (read <= 0) {
            env->DeleteLocalRef(buffer);
            return read;
        }
        env->GetByteArrayRegion(buffer, 0, read, reinterpret_cast<jbyte *>(ptr));
        env->DeleteLocalRef(buffer);
        return read;
    }

    int Write(const void *ptr, asUINT size) override {
        if (ptr == nullptr || size == 0) {
            return 0;
        }
        JNIEnv *env = AttachEnv(vm_);
        if (env == nullptr || write_ == nullptr) {
            return -1;
        }

        jbyteArray buffer = env->NewByteArray(static_cast<jsize>(size));
        if (buffer == nullptr) {
            env->ExceptionClear();
            return -1;
        }
        env->SetByteArrayRegion(buffer, 0, static_cast<jsize>(size),
                                reinterpret_cast<const jbyte *>(ptr));
        const jint written = env->CallIntMethod(stream_, write_, buffer, 0, static_cast<jint>(size));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(buffer);
            return -1;
        }
        env->DeleteLocalRef(buffer);
        return written;
    }

private:
    JavaVM *vm_ = nullptr;
    jobject stream_ = nullptr;
    jmethodID read_ = nullptr;
    jmethodID write_ = nullptr;
};

// ---------------------------------------------------------------------------
// A proxy that exposes a native string factory to Java
// ---------------------------------------------------------------------------

class ProxyStringFactory : public asIStringFactory {
public:
    explicit ProxyStringFactory(asIStringFactory *target) : target_(target) {
    }

    const void *GetStringConstant(const char *data, asUINT length) override {
        return target_->GetStringConstant(data, length);
    }

    int ReleaseStringConstant(const void *str) override {
        return target_->ReleaseStringConstant(str);
    }

    int GetRawStringData(const void *str, char *data, asUINT *length) const override {
        return target_->GetRawStringData(str, data, length);
    }

private:
    asIStringFactory *target_;
};

}  // namespace

namespace asjava {

asIStringFactory *CreateStringFactory(JNIEnv *env, jobject factory) {
    if (factory == nullptr) {
        return nullptr;
    }
    return new JavaStringFactory(env, factory);
}

asIBinaryStream *CreateBinaryStream(JNIEnv *env, jobject stream) {
    if (stream == nullptr) {
        return nullptr;
    }
    return new JavaBinaryStream(env, stream);
}

asIStringFactory *CreateStringFactoryProxy(asIStringFactory *target) {
    if (target == nullptr) {
        return nullptr;
    }
    return new ProxyStringFactory(target);
}

}  // namespace asjava

extern "C" {

JNIEXPORT jlong JNICALL Java_org_angelscript_NativeStringFactory_nativeGetStringConstant(
        JNIEnv *env, jclass, jlong proxy, jstring data) {
    asIStringFactory *factory = reinterpret_cast<asIStringFactory *>(proxy);
    if (factory == nullptr) {
        return 0;
    }
    const std::string text = ToUtf8(env, data);
    const void *constant = factory->GetStringConstant(text.c_str(), static_cast<asUINT>(text.size()));
    return reinterpret_cast<jlong>(constant);
}

JNIEXPORT void JNICALL Java_org_angelscript_NativeStringFactory_nativeReleaseStringConstant(
        JNIEnv *env, jclass, jlong proxy, jlong constant) {
    (void)env;
    asIStringFactory *factory = reinterpret_cast<asIStringFactory *>(proxy);
    if (factory != nullptr && constant != 0) {
        factory->ReleaseStringConstant(reinterpret_cast<const void *>(constant));
    }
}

JNIEXPORT jint JNICALL Java_org_angelscript_NativeStringFactory_nativeGetRawStringData(
        JNIEnv *env, jclass, jlong proxy, jlong constant, jbyteArray buffer) {
    asIStringFactory *factory = reinterpret_cast<asIStringFactory *>(proxy);
    if (factory == nullptr || constant == 0 || buffer == nullptr) {
        return -1;
    }
    const jsize size = env->GetArrayLength(buffer);
    std::vector<char> data(static_cast<size_t>(size) + 1);
    asUINT length = 0;
    const int r = factory->GetRawStringData(reinterpret_cast<const void *>(constant), data.data(),
                                            &length);
    if (r < 0) {
        return r;
    }
    if (length > static_cast<asUINT>(size)) {
        length = static_cast<asUINT>(size);
    }
    env->SetByteArrayRegion(buffer, 0, static_cast<jsize>(length),
                            reinterpret_cast<const jbyte *>(data.data()));
    return static_cast<jint>(length);
}

}  // extern "C"
