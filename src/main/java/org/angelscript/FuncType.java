package org.angelscript;

/**
 * Function kinds, mirroring AngelScript's {@code asEFuncType}.
 */
public enum FuncType {
    DUMMY(-1),
    SYSTEM(0),
    SCRIPT(1),
    INTERFACE(2),
    VIRTUAL(3),
    FUNCDEF(4),
    IMPORTED(5),
    DELEGATE(6),
    TEMPLATE(7);

    private final int value;

    FuncType(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static FuncType fromValue(int value) {
        for (FuncType t : values()) {
            if (t.value == value) {
                return t;
            }
        }
        throw new IllegalArgumentException("Unknown function type: " + value);
    }
}
