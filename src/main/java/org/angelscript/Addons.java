package org.angelscript;

/**
 * Registration of the AngelScript add-ons.
 *
 * <p>Every add-on shipped with the AngelScript SDK is compiled into the native bridge and can be
 * registered on an engine with the methods of this class.</p>
 */
public final class Addons {

    static {
        Native.load();
    }

    private Addons() {
    }

    /** Registers the {@code string} type and its helper functions. */
    public static void registerStdString(AngelScriptEngine engine) {
        nativeRegisterStdString(engine.nativePtr());
    }

    /** Registers the {@code array} template type. */
    public static void registerScriptArray(AngelScriptEngine engine) {
        nativeRegisterScriptArray(engine.nativePtr(), false);
    }

    /** Registers the {@code array} template type, optionally as the default array type. */
    public static void registerScriptArray(AngelScriptEngine engine, boolean defaultArray) {
        nativeRegisterScriptArray(engine.nativePtr(), defaultArray);
    }

    /** Registers the {@code dictionary} type. */
    public static void registerScriptDictionary(AngelScriptEngine engine) {
        nativeRegisterScriptDictionary(engine.nativePtr());
    }

    /** Registers the {@code any} type. */
    public static void registerScriptAny(AngelScriptEngine engine) {
        nativeRegisterScriptAny(engine.nativePtr());
    }

    /** Registers the math functions and types. */
    public static void registerScriptMath(AngelScriptEngine engine) {
        nativeRegisterScriptMath(engine.nativePtr());
    }

    /** Registers the {@code dateTime} type. */
    public static void registerScriptDateTime(AngelScriptEngine engine) {
        nativeRegisterScriptDateTime(engine.nativePtr());
    }

    /** Registers the {@code handle} template type. */
    public static void registerScriptHandle(AngelScriptEngine engine) {
        nativeRegisterScriptHandle(engine.nativePtr());
    }

    /**
     * Registers the file system access functions.
     *
     * <p>This add-on is only available when the native library was built with support for it,
     * which requires the platform SDK headers.</p>
     */
    public static void registerScriptFileSystem(AngelScriptEngine engine) {
        nativeRegisterScriptFileSystem(engine.nativePtr());
    }

    /** Registers the {@code grid} template type. */
    public static void registerScriptGrid(AngelScriptEngine engine) {
        nativeRegisterScriptGrid(engine.nativePtr());
    }

    /** Registers the {@code weakref} template type. */
    public static void registerScriptWeakRef(AngelScriptEngine engine) {
        nativeRegisterScriptWeakRef(engine.nativePtr());
    }

    /** Registers the exception handling routines. */
    public static void registerExceptionRoutines(AngelScriptEngine engine) {
        nativeRegisterExceptionRoutines(engine.nativePtr());
    }

    /** Registers thread support, allowing multiple contexts to run concurrently. */
    public static void registerThreadSupport(AngelScriptEngine engine) {
        nativeRegisterThreadSupport(engine.nativePtr());
    }

    /** Registers co-routine support. */
    public static void registerCoRoutineSupport(AngelScriptEngine engine) {
        nativeRegisterCoRoutineSupport(engine.nativePtr());
    }

    private static native void nativeRegisterStdString(long engine);

    private static native void nativeRegisterScriptArray(long engine, boolean defaultArray);

    private static native void nativeRegisterScriptDictionary(long engine);

    private static native void nativeRegisterScriptAny(long engine);

    private static native void nativeRegisterScriptMath(long engine);

    private static native void nativeRegisterScriptDateTime(long engine);

    private static native void nativeRegisterScriptHandle(long engine);

    private static native void nativeRegisterScriptFileSystem(long engine);

    private static native void nativeRegisterScriptGrid(long engine);

    private static native void nativeRegisterScriptWeakRef(long engine);

    private static native void nativeRegisterExceptionRoutines(long engine);

    private static native void nativeRegisterThreadSupport(long engine);

    private static native void nativeRegisterCoRoutineSupport(long engine);
}
