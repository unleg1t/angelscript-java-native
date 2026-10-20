package org.angelscript;

/**
 * Statistics about the state of the script engine's garbage collector.
 */
public final class GCStatistics {
    public final int currentSize;
    public final int totalDestroyed;
    public final int totalDetected;
    public final int newObjects;
    public final int totalNewDestroyed;

    public GCStatistics(int currentSize, int totalDestroyed, int totalDetected, int newObjects,
                        int totalNewDestroyed) {
        this.currentSize = currentSize;
        this.totalDestroyed = totalDestroyed;
        this.totalDetected = totalDetected;
        this.newObjects = newObjects;
        this.totalNewDestroyed = totalNewDestroyed;
    }

    @Override
    public String toString() {
        return "GCStatistics{currentSize=" + currentSize + ", totalDestroyed=" + totalDestroyed
                + ", totalDetected=" + totalDetected + ", newObjects=" + newObjects
                + ", totalNewDestroyed=" + totalNewDestroyed + '}';
    }
}
