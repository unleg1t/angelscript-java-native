package org.angelscript;

/**
 * A small helper to work with native memory from Java.
 *
 * <p>Some parts of the AngelScript API, such as registering global properties, work with raw
 * addresses. This class allocates and frees native memory and provides typed accessors for the
 * primitive types that AngelScript supports.</p>
 *
 * <p>The memory must be released with {@link #free(long)} once it is no longer needed.</p>
 */
public final class NativeMemory {

    static {
        Native.load();
    }

    private NativeMemory() {
    }

    /** Allocates a block of native memory. */
    public static native long allocate(int size);

    /** Releases a block of native memory that was allocated with {@link #allocate(int)}. */
    public static native void free(long address);

    /** Allocates memory for an int and stores the given value in it. */
    public static long allocateInt(int value) {
        long address = allocate(4);
        setInt(address, value);
        return address;
    }

    /** Allocates memory for a long and stores the given value in it. */
    public static long allocateLong(long value) {
        long address = allocate(8);
        setLong(address, value);
        return address;
    }

    /** Allocates memory for a double and stores the given value in it. */
    public static long allocateDouble(double value) {
        long address = allocate(8);
        setDouble(address, value);
        return address;
    }

    public static native void setByte(long address, byte value);

    public static native byte getByte(long address);

    public static native void setShort(long address, short value);

    public static native short getShort(long address);

    public static native void setInt(long address, int value);

    public static native int getInt(long address);

    public static native void setLong(long address, long value);

    public static native long getLong(long address);

    public static native void setFloat(long address, float value);

    public static native float getFloat(long address);

    public static native void setDouble(long address, double value);

    public static native double getDouble(long address);
}
