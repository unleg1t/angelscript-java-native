package org.angelscript;

/**
 * A compiled script module. This is the Java counterpart of {@code asIScriptModule}.
 *
 * <p>Modules are owned by the engine that created them. {@link #discard()} releases the module
 * and everything that was compiled into it.</p>
 */
public final class ScriptModule {

    private long ptr;

    ScriptModule(long ptr) {
        this.ptr = ptr;
    }

    /** @return the native {@code asIScriptModule *} pointer. */
    long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ miscellaneous

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    public void setName(String name) {
        nativeSetName(name);
    }

    public String getName() {
        return nativeGetName();
    }

    /** Discards the module and releases all resources associated with it. */
    public void discard() {
        if (ptr != 0) {
            nativeDiscard();
            ptr = 0;
        }
    }

    // ------------------------------------------------------------------ compilation

    /** Adds a section of script code to the module. */
    public int addScriptSection(String name, String code) {
        return nativeAddScriptSection(name, code, 0);
    }

    /** Adds a section of script code to the module with an explicit line offset. */
    public int addScriptSection(String name, String code, int lineOffset) {
        return nativeAddScriptSection(name, code, lineOffset);
    }

    /** Builds the module, compiling all added sections. */
    public int build() {
        return nativeBuild();
    }

    /** Compiles a single function without adding it to the module. */
    public ScriptFunction compileFunction(String sectionName, String code, int lineOffset,
                                          int compileFlags) {
        long p = nativeCompileFunction(sectionName, code, lineOffset, compileFlags);
        return p == 0 ? null : new ScriptFunction(p);
    }

    /** Compiles a single global variable without adding it to the module. */
    public int compileGlobalVar(String sectionName, String code, int lineOffset) {
        return nativeCompileGlobalVar(sectionName, code, lineOffset);
    }

    public int setAccessMask(int accessMask) {
        return nativeSetAccessMask(accessMask);
    }

    public int setDefaultNamespace(String nameSpace) {
        return nativeSetDefaultNamespace(nameSpace);
    }

    public String getDefaultNamespace() {
        return nativeGetDefaultNamespace();
    }

    // ------------------------------------------------------------------ functions

    public int getFunctionCount() {
        return nativeGetFunctionCount();
    }

    public ScriptFunction getFunctionByIndex(int index) {
        long p = nativeGetFunctionByIndex(index);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getFunctionByDecl(String decl) {
        long p = nativeGetFunctionByDecl(decl);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getFunctionByName(String name) {
        long p = nativeGetFunctionByName(name);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public int removeFunction(ScriptFunction func) {
        return nativeRemoveFunction(func.nativePtr());
    }

    // ------------------------------------------------------------------ global variables

    public int resetGlobalVars(ScriptContext ctx) {
        return nativeResetGlobalVars(ctx == null ? 0 : ctx.nativePtr());
    }

    public int getGlobalVarCount() {
        return nativeGetGlobalVarCount();
    }

    public int getGlobalVarIndexByName(String name) {
        return nativeGetGlobalVarIndexByName(name);
    }

    public int getGlobalVarIndexByDecl(String decl) {
        return nativeGetGlobalVarIndexByDecl(decl);
    }

    public String getGlobalVarDeclaration(int index, boolean includeNamespace) {
        return nativeGetGlobalVarDeclaration(index, includeNamespace);
    }

    public GlobalVarInfo getGlobalVar(int index) {
        return nativeGetGlobalVar(index);
    }

    public long getAddressOfGlobalVar(int index) {
        return nativeGetAddressOfGlobalVar(index);
    }

    public int removeGlobalVar(int index) {
        return nativeRemoveGlobalVar(index);
    }

    // ------------------------------------------------------------------ type identification

    public int getObjectTypeCount() {
        return nativeGetObjectTypeCount();
    }

    public TypeInfo getObjectTypeByIndex(int index) {
        long p = nativeGetObjectTypeByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    public int getTypeIdByDecl(String decl) {
        return nativeGetTypeIdByDecl(decl);
    }

    public TypeInfo getTypeInfoByName(String name) {
        long p = nativeGetTypeInfoByName(name);
        return p == 0 ? null : new TypeInfo(p);
    }

    public TypeInfo getTypeInfoByDecl(String decl) {
        long p = nativeGetTypeInfoByDecl(decl);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ enums and typedefs

    public int getEnumCount() {
        return nativeGetEnumCount();
    }

    public TypeInfo getEnumByIndex(int index) {
        long p = nativeGetEnumByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    public int getTypedefCount() {
        return nativeGetTypedefCount();
    }

    public TypeInfo getTypedefByIndex(int index) {
        long p = nativeGetTypedefByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ imported functions

    public int getImportedFunctionCount() {
        return nativeGetImportedFunctionCount();
    }

    public int getImportedFunctionIndexByDecl(String decl) {
        return nativeGetImportedFunctionIndexByDecl(decl);
    }

    public String getImportedFunctionDeclaration(int importIndex) {
        return nativeGetImportedFunctionDeclaration(importIndex);
    }

    public String getImportedFunctionSourceModule(int importIndex) {
        return nativeGetImportedFunctionSourceModule(importIndex);
    }

    public int bindImportedFunction(int importIndex, ScriptFunction func) {
        return nativeBindImportedFunction(importIndex, func.nativePtr());
    }

    public int unbindImportedFunction(int importIndex) {
        return nativeUnbindImportedFunction(importIndex);
    }

    public int bindAllImportedFunctions() {
        return nativeBindAllImportedFunctions();
    }

    public int unbindAllImportedFunctions() {
        return nativeUnbindAllImportedFunctions();
    }

    // ------------------------------------------------------------------ byte code

    /** Saves the byte code of the module to the given stream. */
    public int saveByteCode(BinaryStream stream, boolean stripDebugInfo) {
        return nativeSaveByteCode(stream, stripDebugInfo);
    }

    /** Loads byte code from the given stream into the module. */
    public int loadByteCode(BinaryStream stream) {
        return nativeLoadByteCode(stream);
    }

    // ------------------------------------------------------------------ user data

    public long setUserData(long data, long type) {
        return nativeSetUserData(data, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    // ------------------------------------------------------------------ native methods

    private native long nativeGetEngine();

    private native void nativeSetName(String name);

    private native String nativeGetName();

    private native void nativeDiscard();

    private native int nativeAddScriptSection(String name, String code, int lineOffset);

    private native int nativeBuild();

    private native long nativeCompileFunction(String sectionName, String code, int lineOffset,
                                              int compileFlags);

    private native int nativeCompileGlobalVar(String sectionName, String code, int lineOffset);

    private native int nativeSetAccessMask(int accessMask);

    private native int nativeSetDefaultNamespace(String nameSpace);

    private native String nativeGetDefaultNamespace();

    private native int nativeGetFunctionCount();

    private native long nativeGetFunctionByIndex(int index);

    private native long nativeGetFunctionByDecl(String decl);

    private native long nativeGetFunctionByName(String name);

    private native int nativeRemoveFunction(long func);

    private native int nativeResetGlobalVars(long ctx);

    private native int nativeGetGlobalVarCount();

    private native int nativeGetGlobalVarIndexByName(String name);

    private native int nativeGetGlobalVarIndexByDecl(String decl);

    private native String nativeGetGlobalVarDeclaration(int index, boolean includeNamespace);

    private native GlobalVarInfo nativeGetGlobalVar(int index);

    private native long nativeGetAddressOfGlobalVar(int index);

    private native int nativeRemoveGlobalVar(int index);

    private native int nativeGetObjectTypeCount();

    private native long nativeGetObjectTypeByIndex(int index);

    private native int nativeGetTypeIdByDecl(String decl);

    private native long nativeGetTypeInfoByName(String name);

    private native long nativeGetTypeInfoByDecl(String decl);

    private native int nativeGetEnumCount();

    private native long nativeGetEnumByIndex(int index);

    private native int nativeGetTypedefCount();

    private native long nativeGetTypedefByIndex(int index);

    private native int nativeGetImportedFunctionCount();

    private native int nativeGetImportedFunctionIndexByDecl(String decl);

    private native String nativeGetImportedFunctionDeclaration(int importIndex);

    private native String nativeGetImportedFunctionSourceModule(int importIndex);

    private native int nativeBindImportedFunction(int importIndex, long func);

    private native int nativeUnbindImportedFunction(int importIndex);

    private native int nativeBindAllImportedFunctions();

    private native int nativeUnbindAllImportedFunctions();

    private native int nativeSaveByteCode(BinaryStream stream, boolean stripDebugInfo);

    private native int nativeLoadByteCode(BinaryStream stream);

    private native long nativeSetUserData(long data, long type);

    private native long nativeGetUserData(long type);
}
