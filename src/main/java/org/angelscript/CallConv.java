package org.angelscript;

/**
 * Calling conventions, mirroring AngelScript's {@code asECallConvTypes}.
 */
public enum CallConv {
    CDECL(0),
    STDCALL(1),
    THISCALL_ASGLOBAL(2),
    THISCALL(3),
    CDECL_OBJLAST(4),
    CDECL_OBJFIRST(5),
    GENERIC(6),
    THISCALL_OBJLAST(7),
    THISCALL_OBJFIRST(8);

    private final int value;

    CallConv(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static CallConv fromValue(int value) {
        for (CallConv c : values()) {
            if (c.value == value) {
                return c;
            }
        }
        throw new IllegalArgumentException("Unknown calling convention: " + value);
    }
}
