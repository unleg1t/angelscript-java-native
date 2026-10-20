package org.angelscript;

/**
 * An instance of a script class. This is the Java counterpart of {@code asIScriptObject}.
 *
 * <p>Script objects are reference counted; use {@link #close()} to release the reference held by
 * this wrapper.</p>
 */
public final class ScriptObject implements AutoCloseable {

    private long ptr;

    ScriptObject(long ptr) {
        this.ptr = ptr;
    }

    /** @return the native {@code asIScriptObject *} pointer. */
    long nativePtr() {
        return ptr;
    }

    public int addRef() {
        return nativeAddRef();
    }

    public int release() {
        if (ptr == 0) {
            return 0;
        }
        int r = nativeRelease();
        ptr = 0;
        return r;
    }

    @Override
    public void close() {
        release();
    }

    public LockableSharedBool getWeakRefFlag() {
        long p = nativeGetWeakRefFlag();
        return p == 0 ? null : new LockableSharedBool(p);
    }

    public int getTypeId() {
        return nativeGetTypeId();
    }

    public TypeInfo getObjectType() {
        long p = nativeGetObjectType();
        return p == 0 ? null : new TypeInfo(p);
    }

    public int getPropertyCount() {
        return nativeGetPropertyCount();
    }

    public int getPropertyTypeId(int prop) {
        return nativeGetPropertyTypeId(prop);
    }

    public String getPropertyName(int prop) {
        return nativeGetPropertyName(prop);
    }

    public long getAddressOfProperty(int prop) {
        return nativeGetAddressOfProperty(prop);
    }

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    /** Copies the content of another object of the same type into this object. */
    public int copyFrom(ScriptObject other) {
        return nativeCopyFrom(other.nativePtr());
    }

    public long setUserData(long data, long type) {
        return nativeSetUserData(data, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    private native int nativeAddRef();

    private native int nativeRelease();

    private native long nativeGetWeakRefFlag();

    private native int nativeGetTypeId();

    private native long nativeGetObjectType();

    private native int nativeGetPropertyCount();

    private native int nativeGetPropertyTypeId(int prop);

    private native String nativeGetPropertyName(int prop);

    private native long nativeGetAddressOfProperty(int prop);

    private native long nativeGetEngine();

    private native int nativeCopyFrom(long other);

    private native long nativeSetUserData(long data, long type);

    private native long nativeGetUserData(long type);
}
