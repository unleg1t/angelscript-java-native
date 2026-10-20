package org.angelscript;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * A byte stream used by the script engine to save and load byte code.
 *
 * <p>Implementations are pure Java; the engine calls back into them through JNI.</p>
 */
public abstract class BinaryStream {

    /**
     * Reads up to {@code size} bytes into {@code buffer}.
     *
     * @return the number of bytes read, or a negative value on error
     */
    public abstract int read(byte[] buffer, int offset, int size) throws IOException;

    /**
     * Writes {@code size} bytes from {@code buffer}.
     *
     * @return the number of bytes written, or a negative value on error
     */
    public abstract int write(byte[] buffer, int offset, int size) throws IOException;

    /** Convenience overload reading into a freshly allocated array. */
    public byte[] readBytes(int size) throws IOException {
        byte[] buffer = new byte[size];
        int read = read(buffer, 0, size);
        if (read < size) {
            byte[] result = new byte[Math.max(read, 0)];
            System.arraycopy(buffer, 0, result, 0, result.length);
            return result;
        }
        return buffer;
    }

    /** Convenience overload writing the whole array. */
    public int writeBytes(byte[] data) throws IOException {
        return write(data, 0, data.length);
    }
}
