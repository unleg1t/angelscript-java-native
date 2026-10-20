package org.angelscript;

/**
 * Token classes produced by the AngelScript tokenizer, mirroring {@code asETokenClass}.
 */
public enum TokenClass {
    UNKNOWN(0),
    KEYWORD(1),
    VALUE(2),
    IDENTIFIER(3),
    COMMENT(4),
    WHITESPACE(5);

    private final int value;

    TokenClass(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public static TokenClass fromValue(int value) {
        for (TokenClass t : values()) {
            if (t.value == value) {
                return t;
            }
        }
        throw new IllegalArgumentException("Unknown token class: " + value);
    }
}
