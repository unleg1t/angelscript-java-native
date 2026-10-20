package org.angelscript;

/**
 * Garbage collector flags, mirroring AngelScript's {@code asEGCFlags}.
 */
public final class GcFlags {

    private GcFlags() {
    }

    public static final int FULL_CYCLE = 1;
    public static final int ONE_STEP = 2;
    public static final int DESTROY_GARBAGE = 4;
    public static final int DETECT_GARBAGE = 8;
}
