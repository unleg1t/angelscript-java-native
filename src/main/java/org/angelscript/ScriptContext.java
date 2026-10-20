package org.angelscript;

/**
 * A script execution context. This is the Java counterpart of {@code asIScriptContext}.
 *
 * <p>Contexts are reference counted. Use {@link #close()} (or {@link #release()}) when the context
 * is no longer needed.</p>
 */
public final class ScriptContext implements AutoCloseable {

    private long ptr;

    ScriptContext(long ptr) {
        this.ptr = ptr;
    }

    /**
     * Creates a wrapper around a native {@code asIScriptContext *}.
     *
     * <p>Used by the native bridge to hand the context to Java callbacks. The returned wrapper does
     * not own the context.</p>
     */
    public static ScriptContext create(long ptr) {
        return new ScriptContext(ptr);
    }

    /** @return the native {@code asIScriptContext *} pointer. */
    long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ memory management

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

    // ------------------------------------------------------------------ miscellaneous

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    // ------------------------------------------------------------------ execution

    /** Prepares the context for calling the given function. */
    public int prepare(ScriptFunction func) {
        return nativePrepare(func == null ? 0 : func.nativePtr());
    }

    /** Unprepares the context. */
    public int unprepare() {
        return nativeUnprepare();
    }

    /** Executes the prepared function. */
    public int execute() {
        return nativeExecute();
    }

    /** Aborts the execution. */
    public int abort() {
        return nativeAbort();
    }

    /** Suspends the execution, the context can later be resumed with {@link #execute()}. */
    public int suspend() {
        return nativeSuspend();
    }

    public ContextState getState() {
        return ContextState.fromValue(nativeGetState());
    }

    public int pushState() {
        return nativePushState();
    }

    public int popState() {
        return nativePopState();
    }

    public boolean isNested() {
        return nativeIsNested();
    }

    public int getNestCount() {
        return nativeGetNestCount();
    }

    // ------------------------------------------------------------------ object

    /** Sets the object that the called method should be invoked on. */
    public int setObject(long obj) {
        return nativeSetObject(obj);
    }

    // ------------------------------------------------------------------ arguments

    public int setArgByte(int arg, byte value) {
        return nativeSetArgByte(arg, value);
    }

    public int setArgWord(int arg, short value) {
        return nativeSetArgWord(arg, value);
    }

    public int setArgDWord(int arg, int value) {
        return nativeSetArgDWord(arg, value);
    }

    public int setArgQWord(int arg, long value) {
        return nativeSetArgQWord(arg, value);
    }

    public int setArgFloat(int arg, float value) {
        return nativeSetArgFloat(arg, value);
    }

    public int setArgDouble(int arg, double value) {
        return nativeSetArgDouble(arg, value);
    }

    public int setArgAddress(int arg, long addr) {
        return nativeSetArgAddress(arg, addr);
    }

    public int setArgObject(int arg, long obj) {
        return nativeSetArgObject(arg, obj);
    }

    public int setArgVarType(int arg, long ptr, int typeId) {
        return nativeSetArgVarType(arg, ptr, typeId);
    }

    public long getAddressOfArg(int arg) {
        return nativeGetAddressOfArg(arg);
    }

    // ------------------------------------------------------------------ return value

    public byte getReturnByte() {
        return nativeGetReturnByte();
    }

    public short getReturnWord() {
        return nativeGetReturnWord();
    }

    public int getReturnDWord() {
        return nativeGetReturnDWord();
    }

    public long getReturnQWord() {
        return nativeGetReturnQWord();
    }

    public float getReturnFloat() {
        return nativeGetReturnFloat();
    }

    public double getReturnDouble() {
        return nativeGetReturnDouble();
    }

    public long getReturnAddress() {
        return nativeGetReturnAddress();
    }

    public long getReturnObject() {
        return nativeGetReturnObject();
    }

    public long getAddressOfReturnValue() {
        return nativeGetAddressOfReturnValue();
    }

    // ------------------------------------------------------------------ exceptions

    /** Raises a script exception. */
    public int setException(String info, boolean allowCatch) {
        return nativeSetException(info, allowCatch);
    }

    /** Raises a script exception. */
    public int setException(String info) {
        return nativeSetException(info, true);
    }

    public int getExceptionLineNumber() {
        return nativeGetExceptionLineNumber();
    }

    public String getExceptionSection() {
        return nativeGetExceptionSection();
    }

    public ScriptFunction getExceptionFunction() {
        long p = nativeGetExceptionFunction();
        return p == 0 ? null : new ScriptFunction(p);
    }

    public String getExceptionString() {
        return nativeGetExceptionString();
    }

    public boolean willExceptionBeCaught() {
        return nativeWillExceptionBeCaught();
    }

    /** Installs a callback that is invoked when a script exception is raised. */
    public void setExceptionCallback(ExceptionListener listener) {
        nativeSetExceptionCallback(listener);
    }

    public void clearExceptionCallback() {
        nativeClearExceptionCallback();
    }

    // ------------------------------------------------------------------ debugging

    /** Installs a callback that is invoked for every executed line. */
    public void setLineCallback(LineListener listener) {
        nativeSetLineCallback(listener);
    }

    public void clearLineCallback() {
        nativeClearLineCallback();
    }

    public int getCallstackSize() {
        return nativeGetCallstackSize();
    }

    public ScriptFunction getFunction(int stackLevel) {
        long p = nativeGetFunction(stackLevel);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public int getLineNumber(int stackLevel) {
        return nativeGetLineNumber(stackLevel);
    }

    public String getLineSection(int stackLevel) {
        return nativeGetLineSection(stackLevel);
    }

    public int getVarCount(int stackLevel) {
        return nativeGetVarCount(stackLevel);
    }

    public VarInfo getVar(int varIndex, int stackLevel) {
        return nativeGetVar(varIndex, stackLevel);
    }

    public String getVarDeclaration(int varIndex, int stackLevel, boolean includeNamespace) {
        return nativeGetVarDeclaration(varIndex, stackLevel, includeNamespace);
    }

    public long getAddressOfVar(int varIndex, int stackLevel) {
        return nativeGetAddressOfVar(varIndex, stackLevel, false, false);
    }

    public long getAddressOfVar(int varIndex, int stackLevel, boolean dontDereference,
                                boolean returnAddressOfUnitializedObjects) {
        return nativeGetAddressOfVar(varIndex, stackLevel, dontDereference,
                returnAddressOfUnitializedObjects);
    }

    public boolean isVarInScope(int varIndex, int stackLevel) {
        return nativeIsVarInScope(varIndex, stackLevel);
    }

    public int getThisTypeId(int stackLevel) {
        return nativeGetThisTypeId(stackLevel);
    }

    public long getThisPointer(int stackLevel) {
        return nativeGetThisPointer(stackLevel);
    }

    public ScriptFunction getSystemFunction() {
        long p = nativeGetSystemFunction();
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ user data

    public long setUserData(long data, long type) {
        return nativeSetUserData(data, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    // ------------------------------------------------------------------ serialization

    public int startDeserialization() {
        return nativeStartDeserialization();
    }

    public int finishDeserialization() {
        return nativeFinishDeserialization();
    }

    public int pushFunction(ScriptFunction func, long object) {
        return nativePushFunction(func == null ? 0 : func.nativePtr(), object);
    }

    public int getArgsOnStackCount(int stackLevel) {
        return nativeGetArgsOnStackCount(stackLevel);
    }

    public int getArgOnStack(int stackLevel, int arg) {
        return nativeGetArgOnStack(stackLevel, arg);
    }

    // ------------------------------------------------------------------ native methods

    private native int nativeAddRef();

    private native int nativeRelease();

    private native long nativeGetEngine();

    private native int nativePrepare(long func);

    private native int nativeUnprepare();

    private native int nativeExecute();

    private native int nativeAbort();

    private native int nativeSuspend();

    private native int nativeGetState();

    private native int nativePushState();

    private native int nativePopState();

    private native boolean nativeIsNested();

    private native int nativeGetNestCount();

    private native int nativeSetObject(long obj);

    private native int nativeSetArgByte(int arg, byte value);

    private native int nativeSetArgWord(int arg, short value);

    private native int nativeSetArgDWord(int arg, int value);

    private native int nativeSetArgQWord(int arg, long value);

    private native int nativeSetArgFloat(int arg, float value);

    private native int nativeSetArgDouble(int arg, double value);

    private native int nativeSetArgAddress(int arg, long addr);

    private native int nativeSetArgObject(int arg, long obj);

    private native int nativeSetArgVarType(int arg, long ptr, int typeId);

    private native long nativeGetAddressOfArg(int arg);

    private native byte nativeGetReturnByte();

    private native short nativeGetReturnWord();

    private native int nativeGetReturnDWord();

    private native long nativeGetReturnQWord();

    private native float nativeGetReturnFloat();

    private native double nativeGetReturnDouble();

    private native long nativeGetReturnAddress();

    private native long nativeGetReturnObject();

    private native long nativeGetAddressOfReturnValue();

    private native int nativeSetException(String info, boolean allowCatch);

    private native int nativeGetExceptionLineNumber();

    private native String nativeGetExceptionSection();

    private native long nativeGetExceptionFunction();

    private native String nativeGetExceptionString();

    private native boolean nativeWillExceptionBeCaught();

    private native void nativeSetExceptionCallback(ExceptionListener listener);

    private native void nativeClearExceptionCallback();

    private native void nativeSetLineCallback(LineListener listener);

    private native void nativeClearLineCallback();

    private native int nativeGetCallstackSize();

    private native long nativeGetFunction(int stackLevel);

    private native int nativeGetLineNumber(int stackLevel);

    private native String nativeGetLineSection(int stackLevel);

    private native int nativeGetVarCount(int stackLevel);

    private native VarInfo nativeGetVar(int varIndex, int stackLevel);

    private native String nativeGetVarDeclaration(int varIndex, int stackLevel,
                                                 boolean includeNamespace);

    private native long nativeGetAddressOfVar(int varIndex, int stackLevel, boolean dontDereference,
                                              boolean returnAddressOfUnitializedObjects);

    private native boolean nativeIsVarInScope(int varIndex, int stackLevel);

    private native int nativeGetThisTypeId(int stackLevel);

    private native long nativeGetThisPointer(int stackLevel);

    private native long nativeGetSystemFunction();

    private native long nativeSetUserData(long data, long type);

    private native long nativeGetUserData(long type);

    private native int nativeStartDeserialization();

    private native int nativeFinishDeserialization();

    private native int nativePushFunction(long func, long object);

    private native int nativeGetArgsOnStackCount(int stackLevel);

    private native int nativeGetArgOnStack(int stackLevel, int arg);
}
