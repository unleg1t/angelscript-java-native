package org.angelscript;

/**
 * A function implemented in Java that can be registered with the script engine.
 *
 * <p>The callback is invoked using AngelScript's generic calling convention, which gives access to
 * every argument of the call as well as the ability to set the return value.</p>
 *
 * @see AngelScriptEngine#registerGlobalFunction(String, ScriptCallback)
 * @see AngelScriptEngine#registerObjectMethod(String, String, ScriptCallback)
 */
@FunctionalInterface
public interface ScriptCallback {
    /**
     * @param generic the call context of the currently executing script function
     */
    void invoke(ScriptGeneric generic);
}
