package org.angelscript;

/**
 * Type id flag bits, mirroring AngelScript's {@code asETypeIdFlags}.
 */
public final class TypeIdFlags {

    private TypeIdFlags() {
    }

    public static final int VOID = 0;
    public static final int BOOL = 1;
    public static final int INT8 = 2;
    public static final int INT16 = 3;
    public static final int INT32 = 4;
    public static final int INT64 = 5;
    public static final int UINT8 = 6;
    public static final int UINT16 = 7;
    public static final int UINT32 = 8;
    public static final int UINT64 = 9;
    public static final int FLOAT = 10;
    public static final int DOUBLE = 11;

    public static final int OBJHANDLE = 0x40000000;
    public static final int HANDLEOCONST = 0x20000000;
    public static final int MASK_OBJECT = 0x1C000000;
    public static final int APPOBJECT = 0x04000000;
    public static final int SCRIPTOBJECT = 0x08000000;
    public static final int TEMPLATE = 0x10000000;
    public static final int MASK_SEQNBR = 0x03FFFFFF;
}
