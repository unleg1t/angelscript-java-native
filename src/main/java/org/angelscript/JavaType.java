package org.angelscript;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Map;

/**
 * A Java class that has been registered with the script engine as an AngelScript object type.
 *
 * <p>Instances of the Java class are passed to the script engine as handles. The script engine
 * takes care of the reference counting, the Java side keeps a strong reference for as long as the
 * object is wrapped.</p>
 */
public final class JavaType {

    /** The method returns nothing. */
    public static final int KIND_VOID = 0;
    /** The method takes or returns a boolean. */
    public static final int KIND_BOOL = 1;
    /** The method takes or returns a signed byte. */
    public static final int KIND_INT8 = 2;
    /** The method takes or returns a signed short. */
    public static final int KIND_INT16 = 3;
    /** The method takes or returns a signed int. */
    public static final int KIND_INT32 = 4;
    /** The method takes or returns a signed long. */
    public static final int KIND_INT64 = 5;
    /** The method takes or returns an unsigned 16 bit character. */
    public static final int KIND_UINT8 = 6;
    /** The method takes or returns a float. */
    public static final int KIND_FLOAT = 7;
    /** The method takes or returns a double. */
    public static final int KIND_DOUBLE = 8;
    /** The method takes or returns another registered Java type. */
    public static final int KIND_JAVA = 9;
    /** The method takes or returns an AngelScript string. */
    public static final int KIND_STRING = 10;

    private static final Map<Long, List<JavaType>> BY_ENGINE = new HashMap<>();

    private final long ptr;
    private final String scriptName;
    private final Class<?> javaClass;
    private final Map<Object, Long> wrapped = new IdentityHashMap<>();

    JavaType(long ptr, String scriptName, Class<?> javaClass) {
        this.ptr = ptr;
        this.scriptName = scriptName;
        this.javaClass = javaClass;
    }

    /** @return the name the type is known by in the script engine. */
    public String getScriptName() {
        return scriptName;
    }

    /** @return the Java class this type was created from. */
    public Class<?> getJavaClass() {
        return javaClass;
    }

    /** @return the AngelScript type id of this type. */
    public int getTypeId() {
        return nativeTypeId(ptr);
    }

    /**
     * Wraps a Java instance so that it can be handed to the script engine.
     *
     * <p>The returned handle is a native pointer. It can be passed to
     * {@link ScriptContext#setArgObject(int, long)} and friends.</p>
     */
    public long wrap(Object instance) {
        if (instance == null) {
            return 0;
        }
        synchronized (wrapped) {
            Long existing = wrapped.get(instance);
            if (existing != null) {
                return existing;
            }
            long handle = nativeWrap(ptr, instance);
            if (handle != 0) {
                wrapped.put(instance, handle);
            }
            return handle;
        }
    }

    /** Unwraps a handle previously returned by {@link #wrap(Object)}. */
    public Object unwrap(long handle) {
        return nativeUnwrap(ptr, handle);
    }

    /**
     * Releases the reference the Java side holds on a wrapped instance.
     *
     * <p>The instance stays alive as long as the script engine still references it.</p>
     */
    public void release(Object instance) {
        long handle;
        synchronized (wrapped) {
            handle = wrapped.remove(instance);
        }
        if (handle != 0) {
            nativeRelease(ptr, handle);
        }
    }

    /** @return the number of instances of this type that are currently wrapped. */
    public int getWrappedCount() {
        return nativeObjectCount(ptr);
    }

    /** Releases all references this type holds. Called when the engine is closed. */
    void dispose() {
        synchronized (wrapped) {
            for (Long handle : wrapped.values()) {
                nativeRelease(ptr, handle);
            }
            wrapped.clear();
        }
    }

    /** Releases all Java types that were registered with the given engine. */
    static void disposeEngine(long enginePtr) {
        List<JavaType> types;
        synchronized (BY_ENGINE) {
            types = BY_ENGINE.remove(enginePtr);
        }
        if (types == null) {
            return;
        }
        for (JavaType type : types) {
            type.dispose();
        }
    }

    // ------------------------------------------------------------------ registration

    static JavaType register(AngelScriptEngine engine, Class<?> javaClass, String scriptName) {
        List<MethodDescriptor> descriptors = describe(engine, javaClass);
        long ptr = nativeRegister(engine.nativePtr(), scriptName, javaClass,
                descriptors.toArray(new MethodDescriptor[0]));
        if (ptr == 0) {
            throw new IllegalStateException("Could not register the Java type " + javaClass.getName());
        }
        JavaType type = new JavaType(ptr, scriptName, javaClass);
        synchronized (BY_ENGINE) {
            BY_ENGINE.computeIfAbsent(engine.nativePtr(), k -> new ArrayList<>()).add(type);
        }
        return type;
    }

    static List<MethodDescriptor> describe(AngelScriptEngine engine, Class<?> javaClass) {
        List<MethodDescriptor> result = new ArrayList<>();
        Map<String, Method> seen = new HashMap<>();

        for (Method method : javaClass.getMethods()) {
            if (method.isSynthetic() || method.isBridge()) {
                continue;
            }
            if (Modifier.isStatic(method.getModifiers())) {
                continue;
            }
            if (method.getDeclaringClass() == Object.class
                    && !"toString".equals(method.getName())) {
                continue;
            }

            int returnKind;
            String returnTypeName;
            int[] paramKinds;
            String[] paramTypeNames;
            try {
                returnKind = kindOf(engine, method.getReturnType());
                returnTypeName = returnKind == KIND_JAVA
                        ? nativeResolve(engine.nativePtr(), method.getReturnType())
                        : null;
                if (returnKind < 0) {
                    continue;
                }

                Class<?>[] paramTypes = method.getParameterTypes();
                paramKinds = new int[paramTypes.length];
                paramTypeNames = new String[paramTypes.length];
                for (int i = 0; i < paramTypes.length; i++) {
                    paramKinds[i] = kindOf(engine, paramTypes[i]);
                    if (paramKinds[i] < 0) {
                        paramKinds = null;
                        break;
                    }
                    paramTypeNames[i] = paramKinds[i] == KIND_JAVA
                            ? nativeResolve(engine.nativePtr(), paramTypes[i])
                            : null;
                }
                if (paramKinds == null) {
                    continue;
                }
            } catch (RuntimeException e) {
                continue;
            }

            String declaration = buildDeclaration(engine, method, returnKind, returnTypeName,
                    paramKinds, paramTypeNames);
            if (declaration == null) {
                continue;
            }

            String signature = buildJniSignature(method);
            String key = method.getName() + signature;
            if (seen.containsKey(key)) {
                continue;
            }
            seen.put(key, method);

            result.add(new MethodDescriptor(method.getName(), declaration, signature, returnKind,
                    returnTypeName, paramKinds, paramTypeNames));
        }
        return result;
    }

    private static int kindOf(AngelScriptEngine engine, Class<?> type) {
        if (type == void.class || type == Void.class) {
            return KIND_VOID;
        }
        if (type == boolean.class || type == Boolean.class) {
            return KIND_BOOL;
        }
        if (type == byte.class || type == Byte.class) {
            return KIND_INT8;
        }
        if (type == short.class || type == Short.class) {
            return KIND_INT16;
        }
        if (type == int.class || type == Integer.class) {
            return KIND_INT32;
        }
        if (type == long.class || type == Long.class) {
            return KIND_INT64;
        }
        if (type == char.class || type == Character.class) {
            return KIND_UINT8;
        }
        if (type == float.class || type == Float.class) {
            return KIND_FLOAT;
        }
        if (type == double.class || type == Double.class) {
            return KIND_DOUBLE;
        }
        String resolved = nativeResolve(engine.nativePtr(), type);
        if (resolved == null) {
            return -1;
        }
        if ("string".equals(resolved)) {
            return KIND_STRING;
        }
        return KIND_JAVA;
    }

    private static String buildDeclaration(AngelScriptEngine engine, Method method, int returnKind,
                                           String returnTypeName, int[] paramKinds,
                                           String[] paramTypeNames) {
        StringBuilder builder = new StringBuilder();
        switch (returnKind) {
            case KIND_VOID:
                builder.append("void");
                break;
            case KIND_BOOL:
                builder.append("bool");
                break;
            case KIND_INT8:
                builder.append("int8");
                break;
            case KIND_INT16:
                builder.append("int16");
                break;
            case KIND_INT32:
                builder.append("int");
                break;
            case KIND_INT64:
                builder.append("int64");
                break;
            case KIND_UINT8:
                builder.append("uint16");
                break;
            case KIND_FLOAT:
                builder.append("float");
                break;
            case KIND_DOUBLE:
                builder.append("double");
                break;
            case KIND_JAVA:
                builder.append(returnTypeName).append('+');
                break;
            case KIND_STRING:
                builder.append("string");
                break;
            default:
                return null;
        }

        builder.append(' ').append(method.getName()).append('(');
        Class<?>[] paramTypes = method.getParameterTypes();
        for (int i = 0; i < paramTypes.length; i++) {
            if (i > 0) {
                builder.append(", ");
            }
            switch (paramKinds[i]) {
                case KIND_BOOL:
                    builder.append("bool");
                    break;
                case KIND_INT8:
                    builder.append("int8");
                    break;
                case KIND_INT16:
                    builder.append("int16");
                    break;
                case KIND_INT32:
                    builder.append("int");
                    break;
                case KIND_INT64:
                    builder.append("int64");
                    break;
                case KIND_UINT8:
                    builder.append("uint16");
                    break;
                case KIND_FLOAT:
                    builder.append("float");
                    break;
                case KIND_DOUBLE:
                    builder.append("double");
                    break;
                case KIND_JAVA:
                    builder.append(paramTypeNames[i]).append(" &in");
                    break;
                case KIND_STRING:
                    builder.append("const string &in");
                    break;
                default:
                    return null;
            }
            builder.append(' ').append("arg").append(i);
        }
        builder.append(')');
        return builder.toString();
    }

    private static String buildJniSignature(Method method) {
        StringBuilder builder = new StringBuilder("(");
        for (Class<?> paramType : method.getParameterTypes()) {
            builder.append(jniTypeName(paramType));
        }
        builder.append(')').append(jniTypeName(method.getReturnType()));
        return builder.toString();
    }

    private static String jniTypeName(Class<?> type) {
        if (type == void.class) {
            return "V";
        }
        if (type == boolean.class) {
            return "Z";
        }
        if (type == byte.class) {
            return "B";
        }
        if (type == char.class) {
            return "C";
        }
        if (type == short.class) {
            return "S";
        }
        if (type == int.class) {
            return "I";
        }
        if (type == long.class) {
            return "J";
        }
        if (type == float.class) {
            return "F";
        }
        if (type == double.class) {
            return "D";
        }
        if (type.isArray()) {
            return "[" + jniTypeName(type.getComponentType());
        }
        return "L" + type.getName().replace('.', '/') + ";";
    }

    // ------------------------------------------------------------------ native methods

    private static native long nativeRegister(long engine, String scriptName, Class<?> javaClass,
                                              MethodDescriptor[] methods);

    private static native long nativeWrap(long type, Object instance);

    private static native Object nativeUnwrap(long type, long handle);

    private static native void nativeRelease(long type, long handle);

    private static native int nativeTypeId(long type);

    private static native int nativeObjectCount(long type);

    private static native String nativeResolve(long engine, Class<?> javaClass);

    /**
     * The description of a single Java method that is exposed to the script engine.
     */
    public static final class MethodDescriptor {
        /** The name of the Java method. */
        public final String name;
        /** The AngelScript declaration of the method. */
        public final String declaration;
        /** The JNI signature of the Java method. */
        public final String jniSignature;
        /** The kind of the return value. */
        public final int returnKind;
        /** The script name of the returned Java type, or null. */
        public final String returnTypeName;
        /** The kinds of the parameters. */
        public final int[] paramKinds;
        /** The script names of the Java type parameters, or null. */
        public final String[] paramTypeNames;

        MethodDescriptor(String name, String declaration, String jniSignature, int returnKind,
                         String returnTypeName, int[] paramKinds, String[] paramTypeNames) {
            this.name = name;
            this.declaration = declaration;
            this.jniSignature = jniSignature;
            this.returnKind = returnKind;
            this.returnTypeName = returnTypeName;
            this.paramKinds = paramKinds;
            this.paramTypeNames = paramTypeNames;
        }
    }
}
