package org.angelscript;

/**
 * Return codes, mirroring AngelScript's {@code asERetCodes}.
 */
public final class ReturnCode {

    private ReturnCode() {
    }

    public static final int SUCCESS = 0;
    public static final int ERROR = -1;
    public static final int CONTEXT_ACTIVE = -2;
    public static final int CONTEXT_NOT_FINISHED = -3;
    public static final int CONTEXT_NOT_PREPARED = -4;
    public static final int INVALID_ARG = -5;
    public static final int NO_FUNCTION = -6;
    public static final int NOT_SUPPORTED = -7;
    public static final int INVALID_NAME = -8;
    public static final int NAME_TAKEN = -9;
    public static final int INVALID_DECLARATION = -10;
    public static final int INVALID_OBJECT = -11;
    public static final int INVALID_TYPE = -12;
    public static final int ALREADY_REGISTERED = -13;
    public static final int MULTIPLE_FUNCTIONS = -14;
    public static final int NO_MODULE = -15;
    public static final int NO_GLOBAL_VAR = -16;
    public static final int INVALID_CONFIGURATION = -17;
    public static final int INVALID_INTERFACE = -18;
    public static final int CANT_BIND_ALL_FUNCTIONS = -19;
    public static final int LOWER_ARRAY_DIMENSION_NOT_REGISTERED = -20;
    public static final int WRONG_CONFIG_GROUP = -21;
    public static final int CONFIG_GROUP_IS_IN_USE = -22;
    public static final int ILLEGAL_BEHAVIOUR_FOR_TYPE = -23;
    public static final int WRONG_CALLING_CONV = -24;
    public static final int BUILD_IN_PROGRESS = -25;
    public static final int INIT_GLOBAL_VARS_FAILED = -26;
    public static final int OUT_OF_MEMORY = -27;
    public static final int MODULE_IS_IN_USE = -28;

    /**
     * Checks whether a value returned by one of the registration methods is an error code.
     *
     * <p>The AngelScript registration functions return a non-negative identifier on success and a
     * negative error code on failure.</p>
     */
    public static boolean isError(int result) {
        return result < 0;
    }

    /** Human readable name of a return code, using AngelScript's own wording. */
    public static String name(int code) {        switch (code) {
            case SUCCESS: return "asSUCCESS";
            case ERROR: return "asERROR";
            case CONTEXT_ACTIVE: return "asCONTEXT_ACTIVE";
            case CONTEXT_NOT_FINISHED: return "asCONTEXT_NOT_FINISHED";
            case CONTEXT_NOT_PREPARED: return "asCONTEXT_NOT_PREPARED";
            case INVALID_ARG: return "asINVALID_ARG";
            case NO_FUNCTION: return "asNO_FUNCTION";
            case NOT_SUPPORTED: return "asNOT_SUPPORTED";
            case INVALID_NAME: return "asINVALID_NAME";
            case NAME_TAKEN: return "asNAME_TAKEN";
            case INVALID_DECLARATION: return "asINVALID_DECLARATION";
            case INVALID_OBJECT: return "asINVALID_OBJECT";
            case INVALID_TYPE: return "asINVALID_TYPE";
            case ALREADY_REGISTERED: return "asALREADY_REGISTERED";
            case MULTIPLE_FUNCTIONS: return "asMULTIPLE_FUNCTIONS";
            case NO_MODULE: return "asNO_MODULE";
            case NO_GLOBAL_VAR: return "asNO_GLOBAL_VAR";
            case INVALID_CONFIGURATION: return "asINVALID_CONFIGURATION";
            case INVALID_INTERFACE: return "asINVALID_INTERFACE";
            case CANT_BIND_ALL_FUNCTIONS: return "asCANT_BIND_ALL_FUNCTIONS";
            case LOWER_ARRAY_DIMENSION_NOT_REGISTERED: return "asLOWER_ARRAY_DIMENSION_NOT_REGISTERED";
            case WRONG_CONFIG_GROUP: return "asWRONG_CONFIG_GROUP";
            case CONFIG_GROUP_IS_IN_USE: return "asCONFIG_GROUP_IS_IN_USE";
            case ILLEGAL_BEHAVIOUR_FOR_TYPE: return "asILLEGAL_BEHAVIOUR_FOR_TYPE";
            case WRONG_CALLING_CONV: return "asWRONG_CALLING_CONV";
            case BUILD_IN_PROGRESS: return "asBUILD_IN_PROGRESS";
            case INIT_GLOBAL_VARS_FAILED: return "asINIT_GLOBAL_VARS_FAILED";
            case OUT_OF_MEMORY: return "asOUT_OF_MEMORY";
            case MODULE_IS_IN_USE: return "asMODULE_IS_IN_USE";
            default: return "asUNKNOWN(" + code + ")";
        }
    }
}
