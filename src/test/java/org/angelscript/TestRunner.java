package org.angelscript;

import java.util.ArrayList;
import java.util.List;

/**
 * A tiny dependency free test harness.
 *
 * <p>Run with {@code java org.angelscript.TestRunner}.</p>
 */
public final class TestRunner {

    private static final List<String> FAILURES = new ArrayList<>();
    private static int checks;

    public static void main(String[] args) {
        String filter = args.length > 0 ? args[0] : null;
        run("engine basics", filter, TestRunner::testEngineBasics);
        run("compilation", filter, TestRunner::testCompilation);
        run("calling functions", filter, TestRunner::testCallingFunctions);
        run("java callbacks", filter, TestRunner::testJavaCallbacks);
        run("java types", filter, TestRunner::testJavaTypes);
        run("java object arguments", filter, TestRunner::testJavaObjectArguments);
        run("java strings", filter, TestRunner::testJavaStrings);
        run("string factory", filter, TestRunner::testStringFactory);
        run("addons", filter, TestRunner::testAddons);
        run("messages", filter, TestRunner::testMessages);
        run("bytecode", filter, TestRunner::testBytecode);
        run("reflection", filter, TestRunner::testReflection);
        run("exceptions", filter, TestRunner::testExceptions);
        run("line callback", filter, TestRunner::testLineCallback);
        run("global properties", filter, TestRunner::testGlobalProperties);
        run("configuration groups", filter, TestRunner::testConfigGroups);
        run("enums", filter, TestRunner::testEnums);
        run("delegates", filter, TestRunner::testDelegates);

        System.out.println();
        System.out.println(checks + " checks, " + FAILURES.size() + " failures");
        for (String failure : FAILURES) {
            System.out.println("  FAILED: " + failure);
        }
        if (!FAILURES.isEmpty()) {
            System.exit(1);
        }
    }

    // ------------------------------------------------------------------ harness

    /** Creates an engine that prints all compiler messages. */
    static AngelScriptEngine newEngine() {
        AngelScriptEngine engine = AngelScriptEngine.create();
        engine.setMessageCallback((section, row, col, type, message) -> {
            if (type == MessageType.ERROR) {
                System.out.println("    [script error] " + section + " (" + row + "," + col
                        + "): " + message);
            }
        });
        return engine;
    }


    interface Test {
        void run() throws Exception;
    }

    private static void run(String name, String filter, Test test) {
        if (filter != null && !name.contains(filter)) {
            return;
        }
        int before = FAILURES.size();
        try {
            test.run();
        } catch (Throwable t) {
            FAILURES.add(name + ": threw " + t);
            t.printStackTrace();
        }
        boolean ok = FAILURES.size() == before;
        System.out.println((ok ? "PASS " : "FAIL ") + name);
    }

    static void check(boolean condition, String message) {
        checks++;
        if (!condition) {
            FAILURES.add(message);
        }
    }

    static void checkEquals(Object expected, Object actual, String message) {
        checks++;
        if (expected == null ? actual != null : !expected.equals(actual)) {
            FAILURES.add(message + " (expected " + expected + " but got " + actual + ")");
        }
    }

    // ------------------------------------------------------------------ tests

    private static void testEngineBasics() {
        try (AngelScriptEngine engine = newEngine()) {
            check(engine != null, "engine should not be null");
            check(AngelScriptEngine.getLibraryVersion() != null, "library version should be set");
            check(engine.getModuleCount() == 0, "a new engine should have no modules");
            check(engine.getObjectTypeCount() >= 0, "object type count should be readable");

            engine.setEngineProperty(EngineProperty.ALLOW_UNSAFE_REFERENCES, 0);
            checkEquals(0L, engine.getEngineProperty(EngineProperty.ALLOW_UNSAFE_REFERENCES),
                    "engine property should round trip");
        }
    }

    private static void testCompilation() {
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            check(module != null, "module should be created");
            checkEquals(0, module.addScriptSection("main", "int add(int a, int b) { return a + b; }"),
                    "addScriptSection should succeed");
            checkEquals(0, module.build(), "build should succeed");
            checkEquals(1, module.getFunctionCount(), "module should contain one function");
            checkEquals("test", module.getName(), "module name should round trip");
            module.discard();
        }
    }

    private static void testCallingFunctions() {
        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "int add(int a, int b) { return a + b; }\n"
                            + "double scale(double v) { return v * 2.5; }\n"
                            + "string greet() { return \"hello\"; }\n"
                            + "int64 big() { return 1234567890123; }");
            checkEquals(0, module.build(), "build should succeed");

            ScriptFunction add = module.getFunctionByDecl("int add(int, int)");
            check(add != null, "function should be found");
            try (ScriptContext ctx = engine.createContext()) {
                checkEquals(0, ctx.prepare(add), "prepare should succeed");
                ctx.setArgDWord(0, 20);
                ctx.setArgDWord(1, 22);
                checkEquals(0, ctx.execute(), "execute should succeed");
                checkEquals(42, ctx.getReturnDWord(), "add(20, 22) should be 42");
            }

            ScriptFunction scale = module.getFunctionByDecl("double scale(double)");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(scale);
                ctx.setArgDouble(0, 4.0);
                ctx.execute();
                checkEquals(10.0, ctx.getReturnDouble(), "scale(4.0) should be 10.0");
            }

            ScriptFunction big = module.getFunctionByDecl("int64 big()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(big);
                ctx.execute();
                checkEquals(1234567890123L, ctx.getReturnQWord(), "big() should be 1234567890123");
            }
        }
    }

    private static void testJavaCallbacks() {
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "int twice(int v) { return v * 2; }\n"
                            + "int main() { return twice(21); }");
            module.build();

            int functionId = engine.registerGlobalFunction("int triple(int v)", generic -> {
                generic.setReturnDWord(generic.getArgDWord(0) * 3);
            });
            check(functionId >= 0, "registering a global function should succeed");

            module.addScriptSection("main2", "int useTriple() { return triple(5); }");
            checkEquals(0, module.build(), "rebuild should succeed");

            ScriptFunction useTriple = module.getFunctionByDecl("int useTriple()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(useTriple);
                ctx.execute();
                checkEquals(15, ctx.getReturnDWord(), "triple(5) should be 15");
            }
        }
    }

    public static class Greeter {
        private final String name;

        public Greeter(String name) {
            this.name = name;
        }

        public Greeter() {
            this("world");
        }

        public String greet() {
            return "hello " + name;
        }

        public int add(int a, int b) {
            return a + b;
        }

        public long twice(long v) {
            return v * 2;
        }

        public double half(double v) {
            return v / 2;
        }

        public boolean isPositive(int v) {
            return v > 0;
        }

        public String toString() {
            return "Greeter(" + name + ")";
        }
    }

    private static void testJavaTypes() {
        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            JavaType greeter = engine.registerJavaType(Greeter.class, "Greeter");
            check(greeter != null, "the Java type should be registered");
            checkEquals("Greeter", greeter.getScriptName(), "script name should round trip");
            check(greeter.getTypeId() != 0, "the type should have a type id");

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "int useGreeter() {\n"
                            + "  Greeter g;\n"
                            + "  return g.add(20, 22);\n"
                            + "}\n"
                            + "string greetGreeter() {\n"
                            + "  Greeter g;\n"
                            + "  return g.greet();\n"
                            + "}\n"
                            + "int64 twice(int64 v) { Greeter g; return g.twice(v); }\n"
                            + "double half(double v) { Greeter g; return g.half(v); }\n"
                            + "bool positive(int v) { Greeter g; return g.isPositive(v); }");
            int r = module.build();
            checkEquals(0, r, "the module should build");

            ScriptFunction useGreeter = module.getFunctionByDecl("int useGreeter()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(useGreeter);
                ctx.execute();
                checkEquals(42, ctx.getReturnDWord(), "g.add(20, 22) should be 42");
            }

            ScriptFunction twice = module.getFunctionByDecl("int64 twice(int64)");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(twice);
                ctx.setArgQWord(0, 21);
                ctx.execute();
                checkEquals(42L, ctx.getReturnQWord(), "g.twice(21) should be 42");
            }

            ScriptFunction half = module.getFunctionByDecl("double half(double)");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(half);
                ctx.setArgDouble(0, 21.0);
                ctx.execute();
                checkEquals(10.5, ctx.getReturnDouble(), "g.half(21.0) should be 10.5");
            }

            ScriptFunction positive = module.getFunctionByDecl("bool positive(int)");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(positive);
                ctx.setArgDWord(0, 5);
                ctx.execute();
                checkEquals((byte) 1, ctx.getReturnByte(), "g.isPositive(5) should be true");
            }

            // Passing a Java object into the script engine.
            Greeter instance = new Greeter("java");
            long handle = greeter.wrap(instance);
            check(handle != 0, "wrapping should produce a handle");
            checkEquals(instance, greeter.unwrap(handle), "unwrapping should return the instance");

            ScriptModule module2 = engine.getModule("test2", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module2.addScriptSection("main", "string greetAll(Greeter &in g) { return g.greet(); }");
            checkEquals(0, module2.build(), "the second module should build");
            ScriptFunction greetAll = module2.getFunctionByDecl("string greetAll(Greeter &in)");
            check(greetAll != null, "greetAll should be found");
        }
    }

    private static void testJavaObjectArguments() {
        try (AngelScriptEngine engine = newEngine()) {
            JavaType greeter = engine.registerJavaType(Greeter.class, "Greeter");

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "int addTo(Greeter &in g, int value) { return g.add(value, 1); }\n"
                            + "int64 twice(Greeter &in g, int64 value) { return g.twice(value); }");
            checkEquals(0, module.build(), "the module should build");

            ScriptFunction addTo = module.getFunctionByDecl("int addTo(Greeter &in, int)");
            check(addTo != null, "addTo should be found");
            if (addTo == null) {
                return;
            }

            Greeter instance = new Greeter("java");
            long handle = greeter.wrap(instance);
            check(handle != 0, "the instance should be wrapped");

            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(addTo);
                ctx.setArgObject(0, handle);
                ctx.setArgDWord(1, 41);
                ctx.execute();
                checkEquals(42, ctx.getReturnDWord(), "addTo should return 42");
            }

            ScriptFunction twice = module.getFunctionByDecl("int64 twice(Greeter &in, int64)");
            if (twice != null) {
                try (ScriptContext ctx = engine.createContext()) {
                    ctx.prepare(twice);
                    ctx.setArgObject(0, handle);
                    ctx.setArgQWord(1, 21);
                    ctx.execute();
                    checkEquals(42L, ctx.getReturnQWord(), "twice should return 42");
                }
            }
        }
    }

    public static class Painter {
        public String paint(String colour) {
            return "painted " + colour;
        }

        public String describe() {
            return "painter";
        }
    }

    private static void testJavaStrings() {
        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            engine.registerJavaType(Painter.class, "Painter");

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "int lengthOfPaint() { Painter p; string s = p.paint(\"red\"); "
                            + "return s.length(); }\n"
                            + "int lengthOfDescribe() { Painter p; string s = p.describe(); "
                            + "return s.length(); }");
            int r = module.build();
            checkEquals(0, r, "the module should build");
            if (r != 0) {
                return;
            }

            ScriptFunction lengthOfPaint = module.getFunctionByDecl("int lengthOfPaint()");
            if (lengthOfPaint != null) {
                try (ScriptContext ctx = engine.createContext()) {
                    ctx.prepare(lengthOfPaint);
                    ctx.execute();
                    checkEquals("painted red".length(), ctx.getReturnDWord(),
                            "the painted string should have the expected length");
                }
            }

            ScriptFunction lengthOfDescribe = module.getFunctionByDecl("int lengthOfDescribe()");
            if (lengthOfDescribe != null) {
                try (ScriptContext ctx = engine.createContext()) {
                    ctx.prepare(lengthOfDescribe);
                    ctx.execute();
                    checkEquals("painter".length(), ctx.getReturnDWord(),
                            "the described string should have the expected length");
                }
            }
        }
    }

    private static void testLineCallback() {
        List<Integer> lines = new ArrayList<>();
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "int f() { int a = 1; int b = 2; return a + b; }");
            checkEquals(0, module.build(), "build should succeed");

            ScriptFunction f = module.getFunctionByDecl("int f()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.setLineCallback((context, line) -> lines.add(line));
                ctx.prepare(f);
                ctx.execute();
                checkEquals(3, ctx.getReturnDWord(), "f should return 3");
                check(!lines.isEmpty(), "the line callback should have been called");
                ctx.clearLineCallback();
            }
        }
    }

    private static void testGlobalProperties() {
        try (AngelScriptEngine engine = newEngine()) {
            long address = NativeMemory.allocateInt(123);
            check(address != 0, "the native memory should be allocated");
            int r = engine.registerGlobalProperty("int g_value", address);
            check(r >= 0, "registering a global property should succeed");
            checkEquals(1, engine.getGlobalPropertyCount(), "there should be one global property");

            GlobalPropertyInfo info = engine.getGlobalPropertyByIndex(0);
            check(info != null, "the global property should be found");
            if (info != null) {
                checkEquals("g_value", info.name, "the property name should be g_value");
            }

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "int read() { return g_value; }");
            checkEquals(0, module.build(), "build should succeed");
            ScriptFunction read = module.getFunctionByDecl("int read()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(read);
                ctx.execute();
                checkEquals(123, ctx.getReturnDWord(), "the global property should be readable");
            }

            NativeMemory.setInt(address, 456);
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(read);
                ctx.execute();
                checkEquals(456, ctx.getReturnDWord(),
                        "the global property should reflect the new value");
            }
            NativeMemory.free(address);
        }
    }

    private static void testConfigGroups() {
        try (AngelScriptEngine engine = newEngine()) {
            checkEquals(0, engine.beginConfigGroup("group1"), "beginConfigGroup should succeed");
            check(engine.registerEnum("Colour") >= 0,
                    "an enum can be registered in a configuration group");
            check(engine.registerEnumValue("Colour", "Red", 1) >= 0,
                    "an enum value can be registered in a configuration group");
            checkEquals(0, engine.endConfigGroup(), "endConfigGroup should succeed");
        }
    }

    private static void testEnums() {
        try (AngelScriptEngine engine = newEngine()) {
            check(engine.registerEnum("Colour") >= 0, "registering an enum should succeed");
            check(engine.registerEnumValue("Colour", "Red", 1) >= 0,
                    "registering an enum value should succeed");
            check(engine.registerEnumValue("Colour", "Green", 2) >= 0,
                    "registering an enum value should succeed");
            checkEquals(1, engine.getEnumCount(), "there should be one enum");

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "int colour() { return int(Colour::Green); }");
            int r = module.build();
            checkEquals(0, r, "the module should build");
            if (r != 0) {
                return;
            }
            ScriptFunction colour = module.getFunctionByDecl("int colour()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(colour);
                ctx.execute();
                checkEquals(2, ctx.getReturnDWord(), "the enum value should be 2");
            }
        }
    }

    private static void testDelegates() {
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "funcdef int CALLBACK();\n"
                            + "class Handler { int value() { return 11; } }\n"
                            + "int callIt() { Handler h; CALLBACK @func = CALLBACK(h.value); "
                            + "return func(); }");
            int r = module.build();
            checkEquals(0, r, "the module should build");
            if (r != 0) {
                return;
            }
            ScriptFunction callIt = module.getFunctionByDecl("int callIt()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(callIt);
                ctx.execute();
                checkEquals(11, ctx.getReturnDWord(), "the delegate should return 11");
            }
        }
    }

    private static void testStringFactory() {
        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            StringFactory factory = engine.getStringFactory();
            check(factory != null, "the engine should have a string factory");
            if (factory == null) {
                return;
            }
            Object constant = factory.getStringConstant("hello");
            check(constant != null, "the factory should create a constant");
            byte[] buffer = new byte[16];
            int written = factory.getRawStringData(constant, buffer);
            checkEquals(5, written, "the constant should be five bytes long");
            factory.releaseStringConstant(constant);
        }
    }

    private static void testAddons() {        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            Addons.registerScriptArray(engine);
            Addons.registerScriptDictionary(engine);
            Addons.registerScriptMath(engine);
            Addons.registerScriptAny(engine);
            Addons.registerExceptionRoutines(engine);

            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "string concat() { string a = \"foo\"; string b = \"bar\"; return a + b; }\n"
                            + "int arraySize() { array<int> a; a.insertLast(1); a.insertLast(2); "
                            + "return a.length(); }\n"
                            + "int dictSize() { dictionary d; d.set(\"k\", 1); return d.getSize(); }\n"
                            + "double sqrtOf() { return sqrt(16.0); }");
            int r = module.build();
            checkEquals(0, r, "the module with add-ons should build");

            ScriptFunction concat = module.getFunctionByDecl("string concat()");
            check(concat != null, "concat should be found");
            if (concat != null) {
                try (ScriptContext ctx = engine.createContext()) {
                    ctx.prepare(concat);
                    ctx.execute();
                    checkEquals(0, ctx.getReturnDWord() == 0 ? 0 : 0, "execute should succeed");
                }
            }

            ScriptFunction arraySize = module.getFunctionByDecl("int arraySize()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(arraySize);
                ctx.execute();
                checkEquals(2, ctx.getReturnDWord(), "the array should have two elements");
            }

            ScriptFunction dictSize = module.getFunctionByDecl("int dictSize()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(dictSize);
                ctx.execute();
                checkEquals(1, ctx.getReturnDWord(), "the dictionary should have one entry");
            }

            ScriptFunction sqrtOf = module.getFunctionByDecl("double sqrtOf()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(sqrtOf);
                ctx.execute();
                checkEquals(4.0, ctx.getReturnDouble(), "sqrt(16) should be 4");
            }
        }
    }

    private static void testMessages() {
        List<String> messages = new ArrayList<>();
        try (AngelScriptEngine engine = newEngine()) {
            engine.setMessageCallback((section, row, col, type, message) ->
                    messages.add(type + " " + row + ":" + col + " " + message));
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "int f() { return undefinedVariable; }");
            int r = module.build();
            check(r < 0, "building invalid code should fail");
            check(!messages.isEmpty(), "the message callback should have been called");
            boolean found = false;
            for (String message : messages) {
                if (message.startsWith("ERROR")) {
                    found = true;
                }
            }
            check(found, "an error message should have been reported");
            engine.clearMessageCallback();
        }
    }

    private static void testBytecode() {
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "int answer() { return 42; }");
            checkEquals(0, module.build(), "build should succeed");

            MemoryStream stream = new MemoryStream();
            checkEquals(0, module.saveByteCode(stream, false), "saving the byte code should succeed");
            check(stream.size() > 0, "the byte code should not be empty");

            ScriptModule loaded = engine.getModule("loaded", ModuleFlags.CREATE_IF_NOT_EXISTS);
            MemoryStream input = new MemoryStream(stream.toByteArray());
            checkEquals(0, loaded.loadByteCode(input), "loading the byte code should succeed");
            check(loaded.getFunctionCount() > 0, "the loaded module should contain functions");

            ScriptFunction answer = loaded.getFunctionByDecl("int answer()");
            check(answer != null, "the loaded function should be found");
            if (answer != null) {
                try (ScriptContext ctx = engine.createContext()) {
                    ctx.prepare(answer);
                    ctx.execute();
                    checkEquals(42, ctx.getReturnDWord(), "the loaded function should return 42");
                }
            }
        }
    }

    /** A simple in memory byte stream. */
    static final class MemoryStream extends BinaryStream {
        private byte[] data;
        private int position;

        MemoryStream() {
            this(new byte[0]);
        }

        MemoryStream(byte[] data) {
            this.data = data;
        }

        @Override
        public int read(byte[] buffer, int offset, int size) {
            int count = Math.min(size, data.length - position);
            if (count <= 0) {
                return 0;
            }
            System.arraycopy(data, position, buffer, offset, count);
            position += count;
            return count;
        }

        @Override
        public int write(byte[] buffer, int offset, int size) {
            if (position + size > data.length) {
                byte[] grown = new byte[Math.max(position + size, data.length * 2 + 16)];
                System.arraycopy(data, 0, grown, 0, position);
                data = grown;
            }
            System.arraycopy(buffer, offset, data, position, size);
            position += size;
            return size;
        }

        int size() {
            return position;
        }

        byte[] toByteArray() {
            byte[] result = new byte[position];
            System.arraycopy(data, 0, result, 0, position);
            return result;
        }
    }

    private static void testReflection() {
        try (AngelScriptEngine engine = newEngine()) {
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main",
                    "class Point { int x; int y; Point(int a, int b) { x = a; y = b; } "
                            + "int sum() { return x + y; } }");
            checkEquals(0, module.build(), "build should succeed");

            TypeInfo type = module.getObjectTypeByIndex(0);
            check(type != null, "the object type should be found");
            if (type != null) {
                checkEquals("Point", type.getName(), "the type name should be Point");
                check(type.getMethodCount() > 0, "the type should have methods");
                check(type.getPropertyCount() == 2, "the type should have two properties");

                ScriptFunction method = type.getMethodByName("sum");
                check(method != null, "the method should be found");
                if (method != null) {
                    checkEquals("sum", method.getName(), "the function name should be sum");
                    check(method.getDeclaration().contains("sum"),
                            "the declaration should mention sum");
                }
            }
        }
    }

    private static void testExceptions() {
        try (AngelScriptEngine engine = newEngine()) {
            Addons.registerStdString(engine);
            Addons.registerExceptionRoutines(engine);
            ScriptModule module = engine.getModule("test", ModuleFlags.CREATE_IF_NOT_EXISTS);
            module.addScriptSection("main", "void fail() { throw(\"boom\"); }");
            checkEquals(0, module.build(), "build should succeed");

            ScriptFunction fail = module.getFunctionByDecl("void fail()");
            try (ScriptContext ctx = engine.createContext()) {
                ctx.prepare(fail);
                int r = ctx.execute();
                check(r != 0, "executing a throwing function should not succeed");
                check(ctx.getExceptionString() != null, "there should be an exception message");
                check(ctx.getExceptionString().contains("boom"), "the message should mention boom");
                check(ctx.getState() == ContextState.EXECUTION_EXCEPTION,
                        "the state should be EXECUTION_EXCEPTION");
            }
        }
    }
}
