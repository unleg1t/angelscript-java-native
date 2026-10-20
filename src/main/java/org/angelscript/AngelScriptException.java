package org.angelscript;

/**
 * Thrown when a script raises an uncaught exception.
 */
public class AngelScriptException extends RuntimeException {

    private static final long serialVersionUID = 1L;

    private final int lineNumber;
    private final int column;
    private final String section;
    private final String functionDeclaration;

    public AngelScriptException(String message) {
        this(message, -1, -1, null, null);
    }

    public AngelScriptException(String message, int lineNumber, int column, String section,
                                String functionDeclaration) {
        super(message);
        this.lineNumber = lineNumber;
        this.column = column;
        this.section = section;
        this.functionDeclaration = functionDeclaration;
    }

    /** The line in the script section where the exception was raised, or -1 if unknown. */
    public int getLineNumber() {
        return lineNumber;
    }

    /** The column where the exception was raised, or -1 if unknown. */
    public int getColumn() {
        return column;
    }

    /** The name of the script section where the exception was raised, or null. */
    public String getSection() {
        return section;
    }

    /** The declaration of the function in which the exception was raised, or null. */
    public String getFunctionDeclaration() {
        return functionDeclaration;
    }
}
