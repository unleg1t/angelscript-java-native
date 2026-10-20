package org.angelscript;

/**
 * Object type flags, mirroring AngelScript's {@code asEObjTypeFlags}.
 *
 * <p>These are bit flags and are therefore exposed as {@code long} constants so that they can be
 * combined with {@code |} exactly like in C++.</p>
 */
public final class ObjTypeFlags {

    private ObjTypeFlags() {
    }

    // Reference counting / value semantics
    public static final long REF = 1L << 0;
    public static final long VALUE = 1L << 1;
    public static final long GC = 1L << 2;
    public static final long POD = 1L << 3;
    public static final long NOHANDLE = 1L << 4;
    public static final long SCOPED = 1L << 5;
    public static final long TEMPLATE = 1L << 6;
    public static final long ASHANDLE = 1L << 7;

    // Application type traits
    public static final long APP_CLASS = 1L << 8;
    public static final long APP_CLASS_CONSTRUCTOR = 1L << 9;
    public static final long APP_CLASS_DESTRUCTOR = 1L << 10;
    public static final long APP_CLASS_ASSIGNMENT = 1L << 11;
    public static final long APP_CLASS_COPY_CONSTRUCTOR = 1L << 12;
    public static final long APP_CLASS_C = APP_CLASS | APP_CLASS_CONSTRUCTOR;
    public static final long APP_CLASS_CD = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_DESTRUCTOR;
    public static final long APP_CLASS_CA = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_ASSIGNMENT;
    public static final long APP_CLASS_CK = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_CDA = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_DESTRUCTOR | APP_CLASS_ASSIGNMENT;
    public static final long APP_CLASS_CDK = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_DESTRUCTOR | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_CAK = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_ASSIGNMENT | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_CDAK = APP_CLASS | APP_CLASS_CONSTRUCTOR | APP_CLASS_DESTRUCTOR | APP_CLASS_ASSIGNMENT | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_D = APP_CLASS | APP_CLASS_DESTRUCTOR;
    public static final long APP_CLASS_DA = APP_CLASS | APP_CLASS_DESTRUCTOR | APP_CLASS_ASSIGNMENT;
    public static final long APP_CLASS_DK = APP_CLASS | APP_CLASS_DESTRUCTOR | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_DAK = APP_CLASS | APP_CLASS_DESTRUCTOR | APP_CLASS_ASSIGNMENT | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_A = APP_CLASS | APP_CLASS_ASSIGNMENT;
    public static final long APP_CLASS_AK = APP_CLASS | APP_CLASS_ASSIGNMENT | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_K = APP_CLASS | APP_CLASS_COPY_CONSTRUCTOR;
    public static final long APP_CLASS_MORE_CONSTRUCTORS = 1L << 31;

    public static final long APP_PRIMITIVE = 1L << 13;
    public static final long APP_FLOAT = 1L << 14;
    public static final long APP_ARRAY = 1L << 15;
    public static final long APP_CLASS_ALLINTS = 1L << 16;
    public static final long APP_CLASS_ALLFLOATS = 1L << 17;
    public static final long NOCOUNT = 1L << 18;
    public static final long APP_CLASS_ALIGN8 = 1L << 19;
    public static final long IMPLICIT_HANDLE = 1L << 20;
    public static final long APP_CLASS_UNION = 1L << 32;
    public static final long MASK_VALID_FLAGS = 0x1801FFFFFL;
}
