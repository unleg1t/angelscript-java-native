package org.angelscript;

/**
 * Receives compiler messages produced while building a module.
 *
 * <p>Registered through {@link AngelScriptEngine#setMessageCallback(MessageListener)}.</p>
 */
@FunctionalInterface
public interface MessageListener {
    /**
     * @param section  the name of the script section the message refers to
     * @param row      the row (1 based) the message refers to
     * @param col      the column the message refers to
     * @param type     the severity of the message
     * @param message  the human readable message
     */
    void onMessage(String section, int row, int col, MessageType type, String message);
}
