package org.angelscript;

/**
 * Receives a notification for every executed line of script code.
 *
 * <p>Registered through {@link ScriptContext#setLineCallback(LineListener)}.</p>
 */
@FunctionalInterface
public interface LineListener {
    /**
     * @param context    the context that is executing
     * @param lineNumber the 1 based line number that is about to be executed
     */
    void onLine(ScriptContext context, int lineNumber);
}
