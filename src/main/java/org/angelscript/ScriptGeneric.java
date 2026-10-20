package org.angelscript;

/**
 * The generic calling convention context. This is the Java counterpart of {@code asIScriptGeneric}.
 *
 * <p>An instance is only valid for the duration of the callback that received it.</p>
 */
public final class ScriptGeneric {

    private long ptr;

    ScriptGeneric(long ptr) {
        this.ptr = ptr;
    }

    /**
     * Creates a wrapper around a native {@code asIScriptGeneric *}.
     *
     * <p>Used by the native bridge to hand the generic context to Java callbacks.</p>
     */
    public static ScriptGeneric create(long ptr) {
        return new ScriptGeneric(ptr);
    }

    /** @return the native {@code asIScriptGeneric *} pointer. */
    long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ miscellaneous

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    public ScriptFunction getFunction() {
        long p = nativeGetFunction();
        return p == 0 ? null : new ScriptFunction(p);
    }

    public long getAuxiliary() {
        return nativeGetAuxiliary();
    }

    // ------------------------------------------------------------------ object

    /** @return the object the method is called on, or 0 for global functions. */
    public long getObject() {
        return nativeGetObject();
    }

    public int getObjectTypeId() {
        return nativeGetObjectTypeId();
    }

    // ------------------------------------------------------------------ arguments

    public int getArgCount() {
        return nativeGetArgCount();
    }

    public int getArgTypeId(int arg) {
        return nativeGetArgTypeId(arg);
    }

    public byte getArgByte(int arg) {
        return nativeGetArgByte(arg);
    }

    public short getArgWord(int arg) {
        return nativeGetArgWord(arg);
    }

    public int getArgDWord(int arg) {
        return nativeGetArgDWord(arg);
    }

    public long getArgQWord(int arg) {
        return nativeGetArgQWord(arg);
    }

    public float getArgFloat(int arg) {
        return nativeGetArgFloat(arg);
    }

    public double getArgDouble(int arg) {
        return nativeGetArgDouble(arg);
    }

    public long getArgAddress(int arg) {
        return nativeGetArgAddress(arg);
    }

    public long getArgObject(int arg) {
        return nativeGetArgObject(arg);
    }

    public long getAddressOfArg(int arg) {
        return nativeGetAddressOfArg(arg);
    }

    // ------------------------------------------------------------------ return value

    public int getReturnTypeId() {
        return nativeGetReturnTypeId();
    }

    public int setReturnByte(byte val) {
        return nativeSetReturnByte(val);
    }

    public int setReturnWord(short val) {
        return nativeSetReturnWord(val);
    }

    public int setReturnDWord(int val) {
        return nativeSetReturnDWord(val);
    }

    public int setReturnQWord(long val) {
        return nativeSetReturnQWord(val);
    }

    public int setReturnFloat(float val) {
        return nativeSetReturnFloat(val);
    }

    public int setReturnDouble(double val) {
        return nativeSetReturnDouble(val);
    }

    public int setReturnAddress(long addr) {
        return nativeSetReturnAddress(addr);
    }

    public int setReturnObject(long obj) {
        return nativeSetReturnObject(obj);
    }

    public long getAddressOfReturnLocation() {
        return nativeGetAddressOfReturnLocation();
    }

    // ------------------------------------------------------------------ native methods

    private native long nativeGetEngine();

    private native long nativeGetFunction();

    private native long nativeGetAuxiliary();

    private native long nativeGetObject();

    private native int nativeGetObjectTypeId();

    private native int nativeGetArgCount();

    private native int nativeGetArgTypeId(int arg);

    private native byte nativeGetArgByte(int arg);

    private native short nativeGetArgWord(int arg);

    private native int nativeGetArgDWord(int arg);

    private native long nativeGetArgQWord(int arg);

    private native float nativeGetArgFloat(int arg);

    private native double nativeGetArgDouble(int arg);

    private native long nativeGetArgAddress(int arg);

    private native long nativeGetArgObject(int arg);

    private native long nativeGetAddressOfArg(int arg);

    private native int nativeGetReturnTypeId();

    private native int nativeSetReturnByte(byte val);

    private native int nativeSetReturnWord(short val);

    private native int nativeSetReturnDWord(int val);

    private native int nativeSetReturnQWord(long val);

    private native int nativeSetReturnFloat(float val);

    private native int nativeSetReturnDouble(double val);

    private native int nativeSetReturnAddress(long addr);

    private native int nativeSetReturnObject(long obj);

    private native long nativeGetAddressOfReturnLocation();
}
