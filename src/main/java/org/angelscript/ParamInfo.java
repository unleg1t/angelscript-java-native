package org.angelscript;

/**
 * Description of a function parameter.
 */
public final class ParamInfo {
    public final int typeId;
    public final int flags;
    public final String name;
    public final String defaultArg;

    public ParamInfo(int typeId, int flags, String name, String defaultArg) {
        this.typeId = typeId;
        this.flags = flags;
        this.name = name;
        this.defaultArg = defaultArg;
    }

    @Override
    public String toString() {
        return "ParamInfo{" + name + ", typeId=" + typeId + '}';
    }
}
