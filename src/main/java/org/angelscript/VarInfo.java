package org.angelscript;

/**
 * Description of a local variable visible in a script call stack frame.
 */
public final class VarInfo {
    public final String name;
    public final int typeId;
    public final int typeModifiers;
    public final boolean isVarOnHeap;
    public final int stackOffset;

    public VarInfo(String name, int typeId, int typeModifiers, boolean isVarOnHeap, int stackOffset) {
        this.name = name;
        this.typeId = typeId;
        this.typeModifiers = typeModifiers;
        this.isVarOnHeap = isVarOnHeap;
        this.stackOffset = stackOffset;
    }

    @Override
    public String toString() {
        return "VarInfo{" + name + ", typeId=" + typeId + '}';
    }
}
