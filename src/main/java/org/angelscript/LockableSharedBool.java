package org.angelscript;

/**
 * A boolean value that can be shared between threads and whose access is guarded by a lock.
 */
public final class LockableSharedBool implements AutoCloseable {

    static {
        Native.load();
    }

    private long ptr;

    LockableSharedBool(long ptr) {
        this.ptr = ptr;
    }

    private native int nativeAddRef();

    private native int nativeRelease();

    private native boolean nativeGet();

    private native void nativeSet(boolean value);

    private native void nativeLock();

    private native void nativeUnlock();

    public int addRef() {
        return nativeAddRef();
    }

    public int release() {
        return nativeRelease();
    }

    public boolean get() {
        return nativeGet();
    }

    public void set(boolean value) {
        nativeSet(value);
    }

    public void lock() {
        nativeLock();
    }

    public void unlock() {
        nativeUnlock();
    }

    @Override
    public void close() {
        if (ptr != 0) {
            nativeRelease();
            ptr = 0;
        }
    }
}
