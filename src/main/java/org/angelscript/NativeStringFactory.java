package org.angelscript;

/**
 * A view of the string factory that is registered with the script engine.
 *
 * <p>The string constants are represented by their native address, which is returned as a
 * {@link Long}.</p>
 */
public final class NativeStringFactory extends StringFactory {

    static {
        Native.load();
    }

    private final long proxy;

    NativeStringFactory(long proxy) {
        this.proxy = proxy;
    }

    @Override
    public Object getStringConstant(String data) {
        return nativeGetStringConstant(proxy, data);
    }

    @Override
    public void releaseStringConstant(Object constant) {
        nativeReleaseStringConstant(proxy, address(constant));
    }

    @Override
    public int getRawStringData(Object constant, byte[] buffer) {
        return nativeGetRawStringData(proxy, address(constant), buffer);
    }

    private static long address(Object constant) {
        if (constant instanceof Long) {
            return (Long) constant;
        }
        throw new IllegalArgumentException("The constant was not created by this factory");
    }

    private static native long nativeGetStringConstant(long proxy, String data);

    private static native void nativeReleaseStringConstant(long proxy, long constant);

    private static native int nativeGetRawStringData(long proxy, long constant, byte[] buffer);
}
