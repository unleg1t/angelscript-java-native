package org.angelscript;

/**
 * Receives a notification when a script exception is raised.
 *
 * <p>Registered through {@link ScriptContext#setExceptionCallback(ExceptionListener)}.</p>
 */
@FunctionalInterface
public interface ExceptionListener {
    /**
     * @param context the context in which the exception occurred
     */
    void onException(ScriptContext context);
}
