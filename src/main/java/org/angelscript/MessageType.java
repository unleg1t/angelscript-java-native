package org.angelscript;

/**
 * Compiler message types, mirroring AngelScript's {@code asEMsgType}.
 */
public enum MessageType {
    ERROR(0),
    WARNING(1),
    INFORMATION(2);

    private final int value;

    MessageType(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static MessageType fromValue(int value) {
        for (MessageType t : values()) {
            if (t.value == value) {
                return t;
            }
        }
        throw new IllegalArgumentException("Unknown message type: " + value);
    }
}
