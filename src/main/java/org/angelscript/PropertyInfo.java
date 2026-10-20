package org.angelscript;

/**
 * Description of a property of an object type.
 */
public final class PropertyInfo {
    public final String name;
    public final int typeId;
    public final boolean isPrivate;
    public final boolean isProtected;
    public final int offset;
    public final boolean isReference;
    public final int accessMask;
    public final int compositeOffset;
    public final boolean isCompositeIndirect;
    public final boolean isConst;

    public PropertyInfo(String name, int typeId, boolean isPrivate, boolean isProtected, int offset,
                        boolean isReference, int accessMask, int compositeOffset,
                        boolean isCompositeIndirect, boolean isConst) {
        this.name = name;
        this.typeId = typeId;
        this.isPrivate = isPrivate;
        this.isProtected = isProtected;
        this.offset = offset;
        this.isReference = isReference;
        this.accessMask = accessMask;
        this.compositeOffset = compositeOffset;
        this.isCompositeIndirect = isCompositeIndirect;
        this.isConst = isConst;
    }

    @Override
    public String toString() {
        return "PropertyInfo{" + name + ", typeId=" + typeId + '}';
    }
}
