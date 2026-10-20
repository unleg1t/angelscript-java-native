package org.angelscript;

/**
 * Description of a global variable of a module.
 */
public final class GlobalVarInfo {
    public final String name;
    public final String nameSpace;
    public final int typeId;
    public final boolean isConst;

    public GlobalVarInfo(String name, String nameSpace, int typeId, boolean isConst) {
        this.name = name;
        this.nameSpace = nameSpace;
        this.typeId = typeId;
        this.isConst = isConst;
    }

    @Override
    public String toString() {
        return "GlobalVarInfo{" + name + ", typeId=" + typeId + '}';
    }
}
