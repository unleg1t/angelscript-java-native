package org.angelscript;

/**
 * Description of a global property registered with the script engine.
 */
public final class GlobalPropertyInfo {
    public final String name;
    public final String nameSpace;
    public final int typeId;
    public final boolean isConst;
    public final String configGroup;
    public final long pointer;
    public final int accessMask;

    public GlobalPropertyInfo(String name, String nameSpace, int typeId, boolean isConst,
                              String configGroup, long pointer, int accessMask) {
        this.name = name;
        this.nameSpace = nameSpace;
        this.typeId = typeId;
        this.isConst = isConst;
        this.configGroup = configGroup;
        this.pointer = pointer;
        this.accessMask = accessMask;
    }

    @Override
    public String toString() {
        return "GlobalPropertyInfo{" + name + ", typeId=" + typeId + '}';
    }
}
