package org.angelscript;

/**
 * Object behaviours, mirroring AngelScript's {@code asEBehaviours}.
 */
public enum Behaviour {
    CONSTRUCT,
    LIST_CONSTRUCT,
    DESTRUCT,
    FACTORY,
    LIST_FACTORY,
    ADDREF,
    RELEASE,
    GET_WEAKREF_FLAG,
    TEMPLATE_CALLBACK,
    FIRST_GC,
    GETREFCOUNT,
    SETGCFLAG,
    GETGCFLAG,
    ENUMREFS,
    RELEASEREFS,
    LAST_GC;

    /** The raw native value of the first GC behaviour. */
    public static final int FIRST_GC_VALUE = FIRST_GC.ordinal();
    /** The raw native value of the last GC behaviour. */
    public static final int LAST_GC_VALUE = LAST_GC.ordinal();

    public int value() {
        return ordinal();
    }
}
