package org.angelscript;

/**
 * A script or application function. This is the Java counterpart of {@code asIScriptFunction}.
 */
public final class ScriptFunction {

    private long ptr;

    ScriptFunction(long ptr) {
        this.ptr = ptr;
    }

    /** @return the native {@code asIScriptFunction *} pointer. */
    long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ miscellaneous

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    public int addRef() {
        return nativeAddRef();
    }

    public int release() {
        return nativeRelease();
    }

    public int getId() {
        return nativeGetId();
    }

    public FuncType getFuncType() {
        return FuncType.fromValue(nativeGetFuncType());
    }

    public String getModuleName() {
        return nativeGetModuleName();
    }

    public ScriptModule getModule() {
        long p = nativeGetModule();
        return p == 0 ? null : new ScriptModule(p);
    }

    public String getConfigGroup() {
        return nativeGetConfigGroup();
    }

    public int getAccessMask() {
        return nativeGetAccessMask();
    }

    public long getAuxiliary() {
        return nativeGetAuxiliary();
    }

    // ------------------------------------------------------------------ function signature

    public TypeInfo getObjectType() {
        long p = nativeGetObjectType();
        return p == 0 ? null : new TypeInfo(p);
    }

    public String getObjectName() {
        return nativeGetObjectName();
    }

    public String getName() {
        return nativeGetName();
    }

    public String getNamespace() {
        return nativeGetNamespace();
    }

    public String getDeclaration() {
        return getDeclaration(true, false, false);
    }

    public String getDeclaration(boolean includeObjectName, boolean includeNamespace,
                                 boolean includeParamNames) {
        return nativeGetDeclaration(includeObjectName, includeNamespace, includeParamNames);
    }

    public boolean isReadOnly() {
        return nativeIsReadOnly();
    }

    public boolean isPrivate() {
        return nativeIsPrivate();
    }

    public boolean isProtected() {
        return nativeIsProtected();
    }

    public boolean isFinal() {
        return nativeIsFinal();
    }

    public boolean isOverride() {
        return nativeIsOverride();
    }

    public boolean isShared() {
        return nativeIsShared();
    }

    public boolean isExplicit() {
        return nativeIsExplicit();
    }

    public boolean isProperty() {
        return nativeIsProperty();
    }

    public boolean isVariadic() {
        return nativeIsVariadic();
    }

    public int getParamCount() {
        return nativeGetParamCount();
    }

    public ParamInfo getParam(int index) {
        return nativeGetParam(index);
    }

    public int getReturnTypeId() {
        return nativeGetReturnTypeId();
    }

    // ------------------------------------------------------------------ template functions

    public int getSubTypeCount() {
        return nativeGetSubTypeCount();
    }

    public int getSubTypeId(int subTypeIndex) {
        return nativeGetSubTypeId(subTypeIndex);
    }

    public TypeInfo getSubType(int subTypeIndex) {
        long p = nativeGetSubType(subTypeIndex);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ function pointers

    public int getTypeId() {
        return nativeGetTypeId();
    }

    public boolean isCompatibleWithTypeId(int typeId) {
        return nativeIsCompatibleWithTypeId(typeId);
    }

    // ------------------------------------------------------------------ delegates

    public long getDelegateObject() {
        return nativeGetDelegateObject();
    }

    public TypeInfo getDelegateObjectType() {
        long p = nativeGetDelegateObjectType();
        return p == 0 ? null : new TypeInfo(p);
    }

    public ScriptFunction getDelegateFunction() {
        long p = nativeGetDelegateFunction();
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ debug information

    public int getVarCount() {
        return nativeGetVarCount();
    }

    public String getVarName(int index) {
        return nativeGetVarName(index);
    }

    public String getVarDecl(int index) {
        return nativeGetVarDecl(index, false);
    }

    public String getVarDecl(int index, boolean includeNamespace) {
        return nativeGetVarDecl(index, includeNamespace);
    }

    public int findNextLineWithCode(int line) {
        return nativeFindNextLineWithCode(line);
    }

    public int getDeclaredAtRow() {
        return nativeGetDeclaredAtRow();
    }

    public int getDeclaredAtColumn() {
        return nativeGetDeclaredAtColumn();
    }

    public String getDeclaredAtSection() {
        return nativeGetDeclaredAtSection();
    }

    // ------------------------------------------------------------------ user data

    public long setUserData(long userData, long type) {
        return nativeSetUserData(userData, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    @Override
    public String toString() {
        return "ScriptFunction{" + getDeclaration() + '}';
    }

    // ------------------------------------------------------------------ native methods

    private native long nativeGetEngine();

    private native int nativeAddRef();

    private native int nativeRelease();

    private native int nativeGetId();

    private native int nativeGetFuncType();

    private native String nativeGetModuleName();

    private native long nativeGetModule();

    private native String nativeGetConfigGroup();

    private native int nativeGetAccessMask();

    private native long nativeGetAuxiliary();

    private native long nativeGetObjectType();

    private native String nativeGetObjectName();

    private native String nativeGetName();

    private native String nativeGetNamespace();

    private native String nativeGetDeclaration(boolean includeObjectName, boolean includeNamespace,
                                               boolean includeParamNames);

    private native boolean nativeIsReadOnly();

    private native boolean nativeIsPrivate();

    private native boolean nativeIsProtected();

    private native boolean nativeIsFinal();

    private native boolean nativeIsOverride();

    private native boolean nativeIsShared();

    private native boolean nativeIsExplicit();

    private native boolean nativeIsProperty();

    private native boolean nativeIsVariadic();

    private native int nativeGetParamCount();

    private native ParamInfo nativeGetParam(int index);

    private native int nativeGetReturnTypeId();

    private native int nativeGetSubTypeCount();

    private native int nativeGetSubTypeId(int subTypeIndex);

    private native long nativeGetSubType(int subTypeIndex);

    private native int nativeGetTypeId();

    private native boolean nativeIsCompatibleWithTypeId(int typeId);

    private native long nativeGetDelegateObject();

    private native long nativeGetDelegateObjectType();

    private native long nativeGetDelegateFunction();

    private native int nativeGetVarCount();

    private native String nativeGetVarName(int index);

    private native String nativeGetVarDecl(int index, boolean includeNamespace);

    private native int nativeFindNextLineWithCode(int line);

    private native int nativeGetDeclaredAtRow();

    private native int nativeGetDeclaredAtColumn();

    private native String nativeGetDeclaredAtSection();

    private native long nativeSetUserData(long userData, long type);

    private native long nativeGetUserData(long type);
}
