package org.angelscript;

/**
 * Flags controlling how a module is created, mirroring AngelScript's {@code asEGMFlags}.
 */
public enum ModuleFlags {
    ONLY_IF_EXISTS(0),
    CREATE_IF_NOT_EXISTS(1),
    ALWAYS_CREATE(2);

    private final int value;

    ModuleFlags(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static ModuleFlags fromValue(int value) {
        for (ModuleFlags f : values()) {
            if (f.value == value) {
                return f;
            }
        }
        throw new IllegalArgumentException("Unknown module flag: " + value);
    }
}
