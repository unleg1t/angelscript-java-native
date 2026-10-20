package org.angelscript;

/**
 * Execution states of a script context, mirroring AngelScript's {@code asEContextState}.
 */
public enum ContextState {
    EXECUTION_FINISHED(0),
    EXECUTION_SUSPENDED(1),
    EXECUTION_ABORTED(2),
    EXECUTION_EXCEPTION(3),
    EXECUTION_PREPARED(4),
    EXECUTION_UNINITIALIZED(5),
    EXECUTION_ACTIVE(6),
    EXECUTION_ERROR(7),
    EXECUTION_DESERIALIZATION(8);

    private final int value;

    ContextState(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static ContextState fromValue(int value) {
        for (ContextState s : values()) {
            if (s.value == value) {
                return s;
            }
        }
        throw new IllegalArgumentException("Unknown context state: " + value);
    }
}
