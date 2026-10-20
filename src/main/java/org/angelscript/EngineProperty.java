package org.angelscript;

/**
 * Engine properties, mirroring AngelScript's {@code asEEngineProp}.
 *
 * <p>Values are identical to the native enum so they can be passed straight through JNI.</p>
 */
public enum EngineProperty {
    ALLOW_UNSAFE_REFERENCES(1),
    OPTIMIZE_BYTECODE(2),
    COPY_SCRIPT_SECTIONS(3),
    MAX_STACK_SIZE(4),
    USE_CHARACTER_LITERALS(5),
    ALLOW_MULTILINE_STRINGS(6),
    ALLOW_IMPLICIT_HANDLE_TYPES(7),
    BUILD_WITHOUT_LINE_CUES(8),
    INIT_GLOBAL_VARS_AFTER_BUILD(9),
    REQUIRE_ENUM_SCOPE(10),
    SCRIPT_SCANNER(11),
    INCLUDE_JIT_INSTRUCTIONS(12),
    STRING_ENCODING(13),
    PROPERTY_ACCESSOR_MODE(14),
    EXPAND_DEF_ARRAY_TO_TMPL(15),
    AUTO_GARBAGE_COLLECT(16),
    DISALLOW_GLOBAL_VARS(17),
    ALWAYS_IMPL_DEFAULT_CONSTRUCT(18),
    COMPILER_WARNINGS(19),
    DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE(20),
    ALTER_SYNTAX_NAMED_ARGS(21),
    DISABLE_INTEGER_DIVISION(22),
    DISALLOW_EMPTY_LIST_ELEMENTS(23),
    PRIVATE_PROP_AS_PROTECTED(24),
    ALLOW_UNICODE_IDENTIFIERS(25),
    HEREDOC_TRIM_MODE(26),
    MAX_NESTED_CALLS(27),
    GENERIC_CALL_MODE(28),
    INIT_STACK_SIZE(29),
    INIT_CALL_STACK_SIZE(30),
    MAX_CALL_STACK_SIZE(31),
    IGNORE_DUPLICATE_SHARED_INTF(32),
    NO_DEBUG_OUTPUT(33),
    DISABLE_SCRIPT_CLASS_GC(34),
    JIT_INTERFACE_VERSION(35),
    ALWAYS_IMPL_DEFAULT_COPY(36),
    ALWAYS_IMPL_DEFAULT_COPY_CONSTRUCT(37),
    MEMBER_INIT_MODE(38),
    BOOL_CONVERSION_MODE(39),
    FOREACH_SUPPORT(40);

    private final int value;

    EngineProperty(int value) {
        this.value = value;
    }

    /** The raw native value. */
    public int value() {
        return value;
    }

    /** Looks up a property by its raw native value. */
    public static EngineProperty fromValue(int value) {
        for (EngineProperty p : values()) {
            if (p.value == value) {
                return p;
            }
        }
        throw new IllegalArgumentException("Unknown engine property: " + value);
    }
}
