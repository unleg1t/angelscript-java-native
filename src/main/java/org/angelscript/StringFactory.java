package org.angelscript;

/**
 * Controls how string constants are created and stored by the script engine.
 *
 * <p>Implementations are pure Java; the engine calls back into them through JNI.</p>
 */
public abstract class StringFactory {

    /**
     * Returns a pointer to a string constant with the given content.
     *
     * <p>The returned pointer must remain valid until {@link #releaseStringConstant(Object)} is
     * called with the same value.</p>
     */
    public abstract Object getStringConstant(String data);

    /**
     * Releases a string constant previously returned by {@link #getStringConstant(String)}.
     */
    public abstract void releaseStringConstant(Object constant);

    /**
     * Copies the raw content of a string constant into {@code buffer}.
     *
     * @return the number of bytes written, or a negative value on error
     */
    public abstract int getRawStringData(Object constant, byte[] buffer);
}
