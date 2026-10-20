# angelscript-java-native

A cross platform native bridge that lets Java applications use the [AngelScript](https://www.angelcode.com/angelscript/)
scripting engine. The native library bundles the AngelScript core together with **all** of its
add-ons and is prebuilt for **Linux**, **Windows** and **macOS** (x86_64 and arm64).

The Java API mirrors the AngelScript C++ API one to one, so everything that can be done with
AngelScript from C++ can be done from Java as well.

* AngelScript version: **2.38.0**
* Java version: **11** or newer
* Native libraries: `linux-x86_64`, `linux-aarch64`, `windows-x86_64`, `macos-x86_64`,
  `macos-aarch64`

## Quick start

```java
import org.angelscript.*;

try (AngelScriptEngine engine = AngelScriptEngine.create()) {
    ScriptModule module = engine.getModule("main", ModuleFlags.CREATE_IF_NOT_EXISTS);
    module.addScriptSection("main", "int add(int a, int b) { return a + b; }");
    module.build();

    ScriptFunction add = module.getFunctionByDecl("int add(int, int)");
    try (ScriptContext context = engine.createContext()) {
        context.prepare(add);
        context.setArgDWord(0, 20);
        context.setArgDWord(1, 22);
        context.execute();
        System.out.println(context.getReturnDWord());   // 42
    }
}
```

## Using the library

The native library is loaded automatically. It is looked up in this order:

1. the system property `angelscript.native.path` (a file or a directory),
2. the environment variable `ANGELSCRIPT_NATIVE_PATH`,
3. `/native/<platform>/<library>` inside the jar (this is how the prebuilt binaries are shipped),
4. `prebuilt/<platform>/<library>` relative to the working directory.

```java
System.setProperty("angelscript.native.path", "/opt/angelscript/lib");
```

Maven:

```xml
<dependency>
    <groupId>org.angelscript</groupId>
    <artifactId>angelscript-java-native</artifactId>
    <version>1.0.0</version>
</dependency>
```

## API overview

| Java class | AngelScript counterpart |
| --- | --- |
| `AngelScriptEngine` | `asIScriptEngine` |
| `ScriptModule` | `asIScriptModule` |
| `ScriptContext` | `asIScriptContext` |
| `ScriptObject` | `asIScriptObject` |
| `TypeInfo` | `asITypeInfo` |
| `ScriptFunction` | `asIScriptFunction` |
| `ScriptGeneric` | `asIScriptGeneric` |
| `BinaryStream` | `asIBinaryStream` |
| `StringFactory` | `asIStringFactory` |
| `LockableSharedBool` | `asILockableSharedBool` |

The registration methods return a non negative identifier on success and a negative error code on
failure, exactly like the AngelScript C++ API. The error codes are available in `ReturnCode`.

### Add-ons

All AngelScript add-ons are compiled into the native library and can be registered on an engine:

```java
Addons.registerStdString(engine);
Addons.registerScriptArray(engine);
Addons.registerScriptDictionary(engine);
Addons.registerScriptAny(engine);
Addons.registerScriptMath(engine);
Addons.registerScriptDateTime(engine);
Addons.registerScriptHandle(engine);
Addons.registerScriptGrid(engine);
Addons.registerScriptWeakRef(engine);
Addons.registerScriptFileSystem(engine);
Addons.registerExceptionRoutines(engine);
Addons.registerThreadSupport(engine);
Addons.registerCoRoutineSupport(engine);
```

### Calling Java from the script

A Java function can be registered as a global function or as a method of an application type. The
callback receives AngelScript's generic calling convention context, which gives access to every
argument and to the return value:

```java
engine.registerGlobalFunction("int twice(int value)", generic ->
        generic.setReturnDWord(generic.getArgDWord(0) * 2));
```

### Calling the script from Java

Every AngelScript type can be registered as an object type. The public instance methods of the
class become methods of the AngelScript type:

```java
JavaType greeter = engine.registerJavaType(Greeter.class, "Greeter");

ScriptModule module = engine.getModule("main", ModuleFlags.CREATE_IF_NOT_EXISTS);
module.addScriptSection("main", "int useGreeter() { Greeter g; return g.add(20, 22); }");
module.build();
```

The following parameter and return types are mapped automatically:

| Java | AngelScript |
| --- | --- |
| `boolean` | `bool` |
| `byte` | `int8` |
| `short` | `int16` |
| `int` | `int` |
| `long` | `int64` |
| `char` | `uint16` |
| `float` | `float` |
| `double` | `double` |
| `java.lang.String` | `string` (requires the string add-on) |
| another registered Java type | the registered AngelScript type |

Instances can be handed to the script engine with `JavaType.wrap(Object)` and taken back with
`JavaType.unwrap(long)`.

### Compiler messages

```java
engine.setMessageCallback((section, row, col, type, message) ->
        System.err.printf("%s (%d,%d): %s%n", type, row, col, message));
```

### Global properties

Global properties are registered with the address of the variable. Since Java has no way of
taking the address of an object field, `NativeMemory` is used to allocate the storage:

```java
long address = NativeMemory.allocateInt(123);
engine.registerGlobalProperty("int g_value", address);
...
NativeMemory.setInt(address, 456);
NativeMemory.free(address);
```

## Building from source

```bash
./build.sh          # native library, classes, tests and jar
./build.sh native   # only the native library for the host platform
./build.sh java     # only the Java classes
./build.sh test     # build and run the tests
./build.sh jar      # build and package the jar
```

The native library is built with CMake and requires a JDK (the `JAVA_HOME` environment variable
must point at it, otherwise the JNI headers cannot be found).

```bash
cmake -S native -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native
```

The prebuilt libraries live in `prebuilt/<platform>/`. They can be rebuilt for every platform
with the cross compilation script, which uses the [zig](https://ziglang.org) toolchain to produce
the Windows and macOS binaries on a Linux host:

```bash
./scripts/build-cross.sh
```

The GitHub workflow in `.github/workflows/build.yml` rebuilds them for every supported platform
and packages them into the jar.

## Project layout

```
native/                  the JNI bridge and the CMake build
  include/asjava.h       shared declarations
  src/                   one translation unit per part of the API
src/main/java/           the Java API
src/test/java/           the test suite
prebuilt/                the prebuilt native libraries
sdk/                     the AngelScript SDK (core and add-ons)
```

## License

The AngelScript SDK is distributed under the zlib license by Andreas Jonsson. See
`sdk/angelscript/` for the original license text.
