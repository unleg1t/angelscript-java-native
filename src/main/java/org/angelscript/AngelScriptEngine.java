package org.angelscript;

/**
 * The AngelScript script engine. This is the Java counterpart of {@code asIScriptEngine}.
 *
 * <p>An engine owns all modules, types and functions registered with it. Closing the engine
 * releases every resource it holds.</p>
 */
public final class AngelScriptEngine implements AutoCloseable {

    static {
        Native.load();
    }

    /** The native {@code asIScriptEngine *}. */
    private long ptr;

    private AngelScriptEngine(long ptr) {
        this.ptr = ptr;
    }

    /** Wraps an existing native {@code asIScriptEngine *}. */
    static AngelScriptEngine wrap(long ptr) {
        return new AngelScriptEngine(ptr);
    }

    /** Creates a new script engine. */
    public static AngelScriptEngine create() {
        long ptr = nativeCreate();
        if (ptr == 0) {
            throw new IllegalStateException("Could not create the AngelScript engine");
        }
        return new AngelScriptEngine(ptr);
    }

    /** @return the version of the AngelScript library, e.g. {@code "2.38.0"}. */
    public static String getLibraryVersion() {
        return nativeLibraryVersion();
    }

    /** @return the compile time options the AngelScript library was built with. */
    public static String getLibraryOptions() {
        return nativeLibraryOptions();
    }

    // ------------------------------------------------------------------ engine properties

    public int setEngineProperty(EngineProperty property, long value) {
        return nativeSetEngineProperty(property.value(), value);
    }

    public long getEngineProperty(EngineProperty property) {
        return nativeGetEngineProperty(property.value());
    }

    // ------------------------------------------------------------------ messages

    /** Installs a listener that receives all compiler messages. */
    public void setMessageCallback(MessageListener listener) {
        nativeSetMessageCallback(listener);
    }

    /** Removes a previously installed message listener. */
    public void clearMessageCallback() {
        nativeClearMessageCallback();
    }

    /** Writes a message to the message callback. */
    public int writeMessage(String section, int row, int col, MessageType type, String message) {
        return nativeWriteMessage(section, row, col, type.value(), message);
    }

    // ------------------------------------------------------------------ global functions

    /**
     * Registers a global function implemented in Java.
     *
     * @param declaration the AngelScript declaration, e.g. {@code "int add(int a, int b)"}
     * @param callback    the Java implementation
     */
    public int registerGlobalFunction(String declaration, ScriptCallback callback) {
        return nativeRegisterGlobalFunction(declaration, callback);
    }

    public int getGlobalFunctionCount() {
        return nativeGetGlobalFunctionCount();
    }

    public ScriptFunction getGlobalFunctionByIndex(int index) {
        long p = nativeGetGlobalFunctionByIndex(index);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public ScriptFunction getGlobalFunctionByDecl(String declaration) {
        long p = nativeGetGlobalFunctionByDecl(declaration);
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ global properties

    /**
     * Registers a global property.
     *
     * @param declaration the AngelScript declaration, e.g. {@code "int g_value"}
     * @param pointer     the address of the variable
     */
    public int registerGlobalProperty(String declaration, long pointer) {
        return nativeRegisterGlobalProperty(declaration, pointer);
    }

    public int getGlobalPropertyCount() {
        return nativeGetGlobalPropertyCount();
    }

    public GlobalPropertyInfo getGlobalPropertyByIndex(int index) {
        return nativeGetGlobalPropertyByIndex(index);
    }

    public int getGlobalPropertyIndexByName(String name) {
        return nativeGetGlobalPropertyIndexByName(name);
    }

    public int getGlobalPropertyIndexByDecl(String decl) {
        return nativeGetGlobalPropertyIndexByDecl(decl);
    }

    // ------------------------------------------------------------------ object types

    /** Registers an application object type. */
    public int registerObjectType(String obj, int byteSize, long flags) {
        return nativeRegisterObjectType(obj, byteSize, flags);
    }

    /** Registers a property on an application object type. */
    public int registerObjectProperty(String obj, String declaration, int byteOffset) {
        return nativeRegisterObjectProperty(obj, declaration, byteOffset, 0, false);
    }

    /** Registers a property on an application object type. */
    public int registerObjectProperty(String obj, String declaration, int byteOffset,
                                      int compositeOffset, boolean isCompositeIndirect) {
        return nativeRegisterObjectProperty(obj, declaration, byteOffset, compositeOffset,
                isCompositeIndirect);
    }

    /** Registers a method on an application object type, implemented in Java. */
    public int registerObjectMethod(String obj, String declaration, ScriptCallback callback) {
        return nativeRegisterObjectMethod(obj, declaration, callback);
    }

    /** Registers a behaviour on an application object type, implemented in Java. */
    public int registerObjectBehaviour(String obj, Behaviour behaviour, String declaration,
                                       ScriptCallback callback) {
        return nativeRegisterObjectBehaviour(obj, behaviour.value(), declaration, callback);
    }

    public int registerInterface(String name) {
        return nativeRegisterInterface(name);
    }

    public int registerInterfaceMethod(String intf, String declaration) {
        return nativeRegisterInterfaceMethod(intf, declaration);
    }

    public int getObjectTypeCount() {
        return nativeGetObjectTypeCount();
    }

    public TypeInfo getObjectTypeByIndex(int index) {
        long p = nativeGetObjectTypeByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ string factory

    public int registerStringFactory(String datatype, StringFactory factory) {
        return nativeRegisterStringFactory(datatype, factory);
    }

    /**
     * Returns the string factory that is registered with the engine.
     *
     * @return the factory, or null if the engine does not use a string factory
     */
    public StringFactory getStringFactory() {
        return nativeGetStringFactory();
    }

    /**
     * Returns the type modifiers of the registered string factory.
     *
     * @return the modifiers, or 0 if the engine does not use a string factory
     */
    public int getStringFactoryTypeModifiers() {
        return nativeGetStringFactoryTypeModifiers();
    }

    // ------------------------------------------------------------------ default array type

    public int registerDefaultArrayType(String type) {
        return nativeRegisterDefaultArrayType(type);
    }

    public int getDefaultArrayTypeId() {
        return nativeGetDefaultArrayTypeId();
    }

    // ------------------------------------------------------------------ enums

    public int registerEnum(String type) {
        return nativeRegisterEnum(type);
    }

    public int registerEnumValue(String type, String name, int value) {
        return nativeRegisterEnumValue(type, name, value);
    }

    public int getEnumCount() {
        return nativeGetEnumCount();
    }

    public TypeInfo getEnumByIndex(int index) {
        long p = nativeGetEnumByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ funcdefs and typedefs

    public int registerFuncdef(String decl) {
        return nativeRegisterFuncdef(decl);
    }

    public int getFuncdefCount() {
        return nativeGetFuncdefCount();
    }

    public TypeInfo getFuncdefByIndex(int index) {
        long p = nativeGetFuncdefByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    public int registerTypedef(String type, String decl) {
        return nativeRegisterTypedef(type, decl);
    }

    public int getTypedefCount() {
        return nativeGetTypedefCount();
    }

    public TypeInfo getTypedefByIndex(int index) {
        long p = nativeGetTypedefByIndex(index);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ configuration groups

    public int beginConfigGroup(String groupName) {
        return nativeBeginConfigGroup(groupName);
    }

    public int endConfigGroup() {
        return nativeEndConfigGroup();
    }

    public int removeConfigGroup(String groupName) {
        return nativeRemoveConfigGroup(groupName);
    }

    public int setDefaultAccessMask(int defaultMask) {
        return nativeSetDefaultAccessMask(defaultMask);
    }

    public int setDefaultNamespace(String nameSpace) {
        return nativeSetDefaultNamespace(nameSpace);
    }

    public String getDefaultNamespace() {
        return nativeGetDefaultNamespace();
    }

    // ------------------------------------------------------------------ modules

    public ScriptModule getModule(String module, ModuleFlags flag) {
        long p = nativeGetModule(module, flag.value());
        return p == 0 ? null : new ScriptModule(p);
    }

    public ScriptModule getModule(String module) {
        return getModule(module, ModuleFlags.ONLY_IF_EXISTS);
    }

    public int discardModule(String module) {
        return nativeDiscardModule(module);
    }

    public int getModuleCount() {
        return nativeGetModuleCount();
    }

    public ScriptModule getModuleByIndex(int index) {
        long p = nativeGetModuleByIndex(index);
        return p == 0 ? null : new ScriptModule(p);
    }

    // ------------------------------------------------------------------ functions

    public int getLastFunctionId() {
        return nativeGetLastFunctionId();
    }

    public ScriptFunction getFunctionById(int funcId) {
        long p = nativeGetFunctionById(funcId);
        return p == 0 ? null : new ScriptFunction(p);
    }

    // ------------------------------------------------------------------ type identification

    public int getTypeIdByDecl(String decl) {
        return nativeGetTypeIdByDecl(decl);
    }

    public String getTypeDeclaration(int typeId, boolean includeNamespace) {
        return nativeGetTypeDeclaration(typeId, includeNamespace);
    }

    public int getSizeOfPrimitiveType(int typeId) {
        return nativeGetSizeOfPrimitiveType(typeId);
    }

    public TypeInfo getTypeInfoById(int typeId) {
        long p = nativeGetTypeInfoById(typeId);
        return p == 0 ? null : new TypeInfo(p);
    }

    public TypeInfo getTypeInfoByName(String name) {
        long p = nativeGetTypeInfoByName(name);
        return p == 0 ? null : new TypeInfo(p);
    }

    public TypeInfo getTypeInfoByDecl(String decl) {
        long p = nativeGetTypeInfoByDecl(decl);
        return p == 0 ? null : new TypeInfo(p);
    }

    // ------------------------------------------------------------------ script execution

    public ScriptContext createContext() {
        long p = nativeCreateContext();
        return p == 0 ? null : new ScriptContext(p);
    }

    /**
     * Creates a new instance of an application object type.
     *
     * @return the native pointer of the new object
     */
    public long createScriptObject(TypeInfo type) {
        return nativeCreateScriptObject(type.nativePtr());
    }

    /** Creates a copy of an application object. */
    public long createScriptObjectCopy(long obj, TypeInfo type) {
        return nativeCreateScriptObjectCopy(obj, type.nativePtr());
    }

    /** Creates an uninitialized instance of an application object type. */
    public long createUninitializedScriptObject(TypeInfo type) {
        return nativeCreateUninitializedScriptObject(type.nativePtr());
    }

    /** Creates a delegate that calls {@code func} on {@code obj}. */
    public ScriptFunction createDelegate(ScriptFunction func, long obj) {
        long p = nativeCreateDelegate(func.nativePtr(), obj);
        return p == 0 ? null : new ScriptFunction(p);
    }

    public int assignScriptObject(long dstObj, long srcObj, TypeInfo type) {
        return nativeAssignScriptObject(dstObj, srcObj, type.nativePtr());
    }

    public void releaseScriptObject(long obj, TypeInfo type) {
        nativeReleaseScriptObject(obj, type.nativePtr());
    }

    public void addRefScriptObject(long obj, TypeInfo type) {
        nativeAddRefScriptObject(obj, type.nativePtr());
    }

    public int refCastObject(long obj, TypeInfo fromType, TypeInfo toType, long[] outPtr,
                             boolean useOnlyImplicitCast) {
        return nativeRefCastObject(obj, fromType.nativePtr(), toType.nativePtr(), outPtr,
                useOnlyImplicitCast);
    }

    // ------------------------------------------------------------------ context pooling

    public ScriptContext requestContext() {
        long p = nativeRequestContext();
        return p == 0 ? null : new ScriptContext(p);
    }

    public void returnContext(ScriptContext ctx) {
        nativeReturnContext(ctx.nativePtr());
    }

    // ------------------------------------------------------------------ tokenizer

    /** Tokenizes the given string and returns the class of the first token. */
    public TokenClass parseToken(String string) {
        return TokenClass.fromValue(nativeParseToken(string));
    }

    // ------------------------------------------------------------------ garbage collection

    public int garbageCollect(int flags, int numIterations) {
        return nativeGarbageCollect(flags, numIterations);
    }

    public GCStatistics getGCStatistics() {
        return nativeGetGCStatistics();
    }

    // ------------------------------------------------------------------ user data

    public long setUserData(long data, long type) {
        return nativeSetUserData(data, type);
    }

    public long getUserData(long type) {
        return nativeGetUserData(type);
    }

    // ------------------------------------------------------------------ Java interoperability

    /**
     * Registers a Java class with the engine as an AngelScript object type.
     *
     * <p>The public instance methods of the class become methods of the AngelScript type. Methods
     * with parameter or return types that cannot be mapped are skipped.</p>
     *
     * @param javaClass  the Java class to expose
     * @param scriptName the name the type is known by in the script engine
     * @return the registered type
     */
    public JavaType registerJavaType(Class<?> javaClass, String scriptName) {
        return JavaType.register(this, javaClass, scriptName);
    }

    /** Registers a Java class with the engine using its simple name as the script name. */
    public JavaType registerJavaType(Class<?> javaClass) {
        return registerJavaType(javaClass, javaClass.getSimpleName());
    }

    // ------------------------------------------------------------------ shutdown

    /**
     * Shuts the engine down and releases it.
     *
     * @return 0 on success
     */
    public int shutDownAndRelease() {
        if (ptr == 0) {
            return 0;
        }
        // Release the Java side references first, while the engine is still alive. The engine then
        // releases the references it still holds while shutting down.
        JavaType.disposeEngine(ptr);
        int r = nativeShutDownAndRelease();
        ptr = 0;
        return r;
    }

    @Override
    public void close() {
        shutDownAndRelease();
    }

    /** @return the native {@code asIScriptEngine *} pointer. */
    public long nativePtr() {
        return ptr;
    }

    // ------------------------------------------------------------------ native methods

    private static native long nativeCreate();

    private static native String nativeLibraryVersion();

    private static native String nativeLibraryOptions();

    private native int nativeSetEngineProperty(int property, long value);

    private native long nativeGetEngineProperty(int property);

    private native void nativeSetMessageCallback(MessageListener listener);

    private native void nativeClearMessageCallback();

    private native int nativeWriteMessage(String section, int row, int col, int type, String message);

    private native int nativeRegisterGlobalFunction(String declaration, ScriptCallback callback);

    private native int nativeGetGlobalFunctionCount();

    private native long nativeGetGlobalFunctionByIndex(int index);

    private native long nativeGetGlobalFunctionByDecl(String declaration);

    private native int nativeRegisterGlobalProperty(String declaration, long pointer);

    private native int nativeGetGlobalPropertyCount();

    private native GlobalPropertyInfo nativeGetGlobalPropertyByIndex(int index);

    private native int nativeGetGlobalPropertyIndexByName(String name);

    private native int nativeGetGlobalPropertyIndexByDecl(String decl);

    private native int nativeRegisterObjectType(String obj, int byteSize, long flags);

    private native int nativeRegisterObjectProperty(String obj, String declaration, int byteOffset,
                                                     int compositeOffset,
                                                     boolean isCompositeIndirect);

    private native int nativeRegisterObjectMethod(String obj, String declaration,
                                                  ScriptCallback callback);

    private native int nativeRegisterObjectBehaviour(String obj, int behaviour, String declaration,
                                                     ScriptCallback callback);

    private native int nativeRegisterInterface(String name);

    private native int nativeRegisterInterfaceMethod(String intf, String declaration);

    private native int nativeGetObjectTypeCount();

    private native long nativeGetObjectTypeByIndex(int index);

    private native int nativeRegisterStringFactory(String datatype, StringFactory factory);

    private native StringFactory nativeGetStringFactory();

    private native int nativeGetStringFactoryTypeModifiers();

    private native int nativeRegisterDefaultArrayType(String type);

    private native int nativeGetDefaultArrayTypeId();

    private native int nativeRegisterEnum(String type);

    private native int nativeRegisterEnumValue(String type, String name, int value);

    private native int nativeGetEnumCount();

    private native long nativeGetEnumByIndex(int index);

    private native int nativeRegisterFuncdef(String decl);

    private native int nativeGetFuncdefCount();

    private native long nativeGetFuncdefByIndex(int index);

    private native int nativeRegisterTypedef(String type, String decl);

    private native int nativeGetTypedefCount();

    private native long nativeGetTypedefByIndex(int index);

    private native int nativeBeginConfigGroup(String groupName);

    private native int nativeEndConfigGroup();

    private native int nativeRemoveConfigGroup(String groupName);

    private native int nativeSetDefaultAccessMask(int defaultMask);

    private native int nativeSetDefaultNamespace(String nameSpace);

    private native String nativeGetDefaultNamespace();

    private native long nativeGetModule(String module, int flag);

    private native int nativeDiscardModule(String module);

    private native int nativeGetModuleCount();

    private native long nativeGetModuleByIndex(int index);

    private native int nativeGetLastFunctionId();

    private native long nativeGetFunctionById(int funcId);

    private native int nativeGetTypeIdByDecl(String decl);

    private native String nativeGetTypeDeclaration(int typeId, boolean includeNamespace);

    private native int nativeGetSizeOfPrimitiveType(int typeId);

    private native long nativeGetTypeInfoById(int typeId);

    private native long nativeGetTypeInfoByName(String name);

    private native long nativeGetTypeInfoByDecl(String decl);

    private native long nativeCreateContext();

    private native long nativeCreateScriptObject(long type);

    private native long nativeCreateScriptObjectCopy(long obj, long type);

    private native long nativeCreateUninitializedScriptObject(long type);

    private native long nativeCreateDelegate(long func, long obj);

    private native int nativeAssignScriptObject(long dstObj, long srcObj, long type);

    private native void nativeReleaseScriptObject(long obj, long type);

    private native void nativeAddRefScriptObject(long obj, long type);

    private native int nativeRefCastObject(long obj, long fromType, long toType, long[] outPtr,
                                           boolean useOnlyImplicitCast);

    private native long nativeRequestContext();

    private native void nativeReturnContext(long ctx);

    private native int nativeParseToken(String string);

    private native int nativeGarbageCollect(int flags, int numIterations);

    private native GCStatistics nativeGetGCStatistics();

    private native long nativeSetUserData(long data, long type);

    private native long nativeGetUserData(long type);

    private native int nativeShutDownAndRelease();
}
