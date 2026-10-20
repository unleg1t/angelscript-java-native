package org.angelscript;

/**
 * Information about a type registered with the engine. This is the Java counterpart of
 * {@code asITypeInfo}.
 */
public final class TypeInfo {

    private long ptr;

    TypeInfo(long ptr) {
        this.ptr = ptr;
    }

    /** @return the native {@code asITypeInfo *} pointer. */
    long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ miscellaneous

    public AngelScriptEngine getEngine() {
        long p = nativeGetEngine();
        return p == 0 ? null : AngelScriptEngine.wrap(p);
    }

    public String getConfigGroup() {
        return nativeGetConfigGroup();
    }

    public int getAccessMask() {
        return nativeGetAccessMask();
    }

    public ScriptModule getModule() {
        long p = nativeGetModule();
        return p == 0 ? null : new ScriptModule(p);
    }

    // ------------------------------------------------------------------ memory management

    public int addRef() {
        return nativeAddRef();
    }

    public int release() {
        return nativeRelease();
    }

    // ------------------------------------------------------------------ type info

    public String getName() {
        return nativeGetName();
    }

    public String getNamespace() {
        return nativeGetNamespace();
    }

    public TypeInfo getBaseType() {
        long p = nativeGetBaseType();
        return p == 0 ? null : new TypeInfo(p);
    }

    public boolean derivesFrom(TypeInfo objType) {
        return nativeDerivesFrom(objType.nativePtr());
    }

    public long getFlags() {
        return nativeGetFlags();
    }

    public int getSize() {
        return nativeGetSize();
    }

    public int getTypeId() {
        return nativeGetTypeId();
    }

    public int getSubTypeId(int subTypeIndex) {
        return nativeGetSubTypeId(subTypeIndex);
    }

    public TypeInfo getSubType(int subTypeIndex) {
        long p = nativeGetSubType(subTypeIndex);
        return p == 0 ? null : new TypeInfo(p);
    }

    public int getSubTypeCount() {
        return nativeGetSubTypeCount();
    }

    // ------------------------------------------------------------------ interfaces

    public int getInterfaceCount() {
        return nativeGetInterfaceCount();
    }

    public TypeInfo getInterface(int index) {
        long p = nativeGetInterface(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    public boolean implementsInterface(TypeInfo objType) {
        return nativeImplements(objType.nativePtr());
    }

    // ------------------------------------------------------------------ factories

    public int getFactoryCount() {
        return nativeGetFactoryCount();
    }

    public ScriptFunction getFactoryByIndex(int index) {
        long p = nativeGetFactoryByIndex(index);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getFactoryByDecl(String decl) {
        long p = nativeGetFactoryByDecl(decl);
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ methods

    public int getMethodCount() {
        return nativeGetMethodCount();
    }

    public ScriptFunction getMethodByIndex(int index) {
        return getMethodByIndex(index, true);
    }

    public ScriptFunction getMethodByIndex(int index, boolean getVirtual) {
        long p = nativeGetMethodByIndex(index, getVirtual);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getMethodByName(String name) {
        return getMethodByName(name, true);
    }

    public ScriptFunction getMethodByName(String name, boolean getVirtual) {
        long p = nativeGetMethodByName(name, getVirtual);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getMethodByDecl(String decl) {
        return getMethodByDecl(decl, true);
    }

    public ScriptFunction getMethodByDecl(String decl, boolean getVirtual) {
        long p = nativeGetMethodByDecl(decl, getVirtual);
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ properties

    public int getPropertyCount() {
        return nativeGetPropertyCount();
    }

    public PropertyInfo getProperty(int index) {
        return nativeGetProperty(index);
    }

    public String getPropertyDeclaration(int index, boolean includeNamespace) {
        return nativeGetPropertyDeclaration(index, includeNamespace);
    }

    // ------------------------------------------------------------------ behaviours

    public int getBehaviourCount() {
        return nativeGetBehaviourCount();
    }

    public ScriptFunction getBehaviourByIndex(int index) {
        long p = nativeGetBehaviourByIndex(index);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public Behaviour getBehaviourTypeByIndex(int index) {
        return Behaviour.values()[nativeGetBehaviourTypeByIndex(index)];
    }

    // ------------------------------------------------------------------ child types

    public int getChildFuncdefCount() {
        return nativeGetChildFuncdefCount();
    }

    public TypeInfo getChildFuncdef(int index) {
        long p = nativeGetChildFuncdef(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    public TypeInfo getParentType() {
        long p = nativeGetParentType();
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ enums

    public int getEnumValueCount() {
        return nativeGetEnumValueCount();
    }

    public String getEnumValueByIndex(int index, int[] outValue) {
        return nativeGetEnumValueByIndex(index, outValue);
    }

    // ------------------------------------------------------------------ typedef / funcdef

    public int getTypedefTypeId() {
        return nativeGetTypedefTypeId();
    }

    public ScriptFunction getFuncdefSignature() {
        long p = nativeGetFuncdefSignature();
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ user data

    public long setUserData(long data, long type) {
        return nativeSetUserData(data, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    @Override
    public String toString() {
        return "TypeInfo{" + getName() + '}';
    }

    // ------------------------------------------------------------------ native methods

    private native long nativeGetEngine();

    private native String nativeGetConfigGroup();

    private native int nativeGetAccessMask();

    private native long nativeGetModule();

    private native int nativeAddRef();

    private native int nativeRelease();

    private native String nativeGetName();

    private native String nativeGetNamespace();

    private native long nativeGetBaseType();

    private native boolean nativeDerivesFrom(long objType);

    private native long nativeGetFlags();

    private native int nativeGetSize();

    private native int nativeGetTypeId();

    private native int nativeGetSubTypeId(int subTypeIndex);

    private native long nativeGetSubType(int subTypeIndex);

    private native int nativeGetSubTypeCount();

    private native int nativeGetInterfaceCount();

    private native long nativeGetInterface(int index);

    private native boolean nativeImplements(long objType);

    private native int nativeGetFactoryCount();

    private native long nativeGetFactoryByIndex(int index);

    private native long nativeGetFactoryByDecl(String decl);

    private native int nativeGetMethodCount();

    private native long nativeGetMethodByIndex(int index, boolean getVirtual);

    private native long nativeGetMethodByName(String name, boolean getVirtual);

    private native long nativeGetMethodByDecl(String decl, boolean getVirtual);

    private native int nativeGetPropertyCount();

    private native PropertyInfo nativeGetProperty(int index);

    private native String nativeGetPropertyDeclaration(int index, boolean includeNamespace);

    private native int nativeGetBehaviourCount();

    private native long nativeGetBehaviourByIndex(int index);

    private native int nativeGetBehaviourTypeByIndex(int index);

    private native int nativeGetChildFuncdefCount();

    private native long nativeGetChildFuncdef(int index);

    private native long nativeGetParentType();

    private native int nativeGetEnumValueCount();

    private native String nativeGetEnumValueByIndex(int index, int[] outValue);

    private native int nativeGetTypedefTypeId();

    private native long nativeGetFuncdefSignature();

    private native long nativeSetUserData(long data, long type);

    private native long nativeGetUserData(long type);
}
