package org.angelscript;

/**
 * Type modifiers, mirroring AngelScript's {@code asETypeModifiers}.
 */
public final class TypeModifiers {

    private TypeModifiers() {
    }

    public static final int NONE = 0;
    public static final int INREF = 1;
    public static final int OUTREF = 2;
    public static final int INOUTREF = 3;
    public static final int CONST = 4;
}
