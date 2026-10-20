#!/usr/bin/env bash
#
# Builds the AngelScript JNI bridge for the host platform, compiles the Java sources and
# packages everything into a jar.
#
# Usage:
#   ./build.sh                 build the native library, the classes and the jar
#   ./build.sh native          only build the native library
#   ./build.sh java            only compile the Java sources
#   ./build.sh test            build everything and run the tests
#   ./build.sh jar             build everything and create the jar
#
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT/build"
CLASSES_DIR="$BUILD_DIR/classes"
TEST_CLASSES_DIR="$BUILD_DIR/test-classes"
RESOURCES_DIR="$BUILD_DIR/resources"
JAR_FILE="$BUILD_DIR/angelscript-java-native.jar"

if [ -n "${JAVA_HOME:-}" ]; then
    JAVAC="$JAVA_HOME/bin/javac"
    JAVA="$JAVA_HOME/bin/java"
    JAR="$JAVA_HOME/bin/jar"
else
    JAVAC="javac"
    JAVA="java"
    JAR="jar"
    # Derive JAVA_HOME from the location of the compiler so that the native build can find the
    # JNI headers.
    javac_path="$(command -v javac || true)"
    if [ -n "$javac_path" ]; then
        resolved="$(readlink -f "$javac_path" 2>/dev/null || echo "$javac_path")"
        JAVA_HOME="$(dirname "$(dirname "$resolved")")"
        export JAVA_HOME
    fi
fi

# The platform identifier used by the Java loader and the prebuilt directory layout.
detect_platform() {
    local os arch
    case "$(uname -s)" in
        Linux) os="linux" ;;
        Darwin) os="macos" ;;
        MINGW* | MSYS* | CYGWIN*) os="windows" ;;
        *) os="$(uname -s | tr '[:upper:]' '[:lower:]')" ;;
    esac
    case "$(uname -m)" in
        x86_64 | amd64) arch="x86_64" ;;
        aarch64 | arm64) arch="aarch64" ;;
        armv7l | armv7) arch="armv7" ;;
        i386 | i686) arch="x86" ;;
        *) arch="$(uname -m)" ;;
    esac
    echo "$os-$arch"
}

PLATFORM="$(detect_platform)"

build_native() {
    echo "==> Building the native library for $PLATFORM"
    cmake -S "$ROOT/native" -B "$BUILD_DIR/native-$PLATFORM" \
        -DCMAKE_BUILD_TYPE=Release \
        -DAS_JNI_INSTALL_DIR="$ROOT/prebuilt/$PLATFORM"
    cmake --build "$BUILD_DIR/native-$PLATFORM" --config Release -j "$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"

    # Remove the debug information from the library that was just built.
    local library
    case "$PLATFORM" in
        windows-*) library="$ROOT/prebuilt/$PLATFORM/angelscript-jni.dll" ;;
        macos-*) library="$ROOT/prebuilt/$PLATFORM/libangelscript-jni.dylib" ;;
        *) library="$ROOT/prebuilt/$PLATFORM/libangelscript-jni.so" ;;
    esac
    if [ -f "$library" ]; then
        strip --strip-unneeded "$library" 2>/dev/null || true
    fi
}

build_java() {
    echo "==> Compiling the Java sources"
    rm -rf "$CLASSES_DIR"
    mkdir -p "$CLASSES_DIR"
    find "$ROOT/src/main/java" -name '*.java' > "$BUILD_DIR/sources.txt"
    "$JAVAC" -Xlint:all -d "$CLASSES_DIR" @"$BUILD_DIR/sources.txt"

    echo "==> Copying the prebuilt native libraries"
    rm -rf "$RESOURCES_DIR"
    mkdir -p "$RESOURCES_DIR/native"
    for dir in "$ROOT"/prebuilt/*/; do
        [ -d "$dir" ] || continue
        name="$(basename "$dir")"
        mkdir -p "$RESOURCES_DIR/native/$name"
        cp "$dir"* "$RESOURCES_DIR/native/$name/" 2>/dev/null || true
    done
    cp -r "$CLASSES_DIR"/. "$RESOURCES_DIR/"
}

build_tests() {
    echo "==> Compiling the tests"
    rm -rf "$TEST_CLASSES_DIR"
    mkdir -p "$TEST_CLASSES_DIR"
    find "$ROOT/src/test/java" -name '*.java' > "$BUILD_DIR/test-sources.txt"
    "$JAVAC" -cp "$CLASSES_DIR" -d "$TEST_CLASSES_DIR" @"$BUILD_DIR/test-sources.txt"
}

run_tests() {
    echo "==> Running the tests"
    "$JAVA" -cp "$CLASSES_DIR:$TEST_CLASSES_DIR" \
        -Dangelscript.native.path="$ROOT/prebuilt/$PLATFORM" \
        org.angelscript.TestRunner
}

build_jar() {
    echo "==> Packaging the jar"
    mkdir -p "$BUILD_DIR"
    "$JAR" --create --file "$JAR_FILE" -C "$RESOURCES_DIR" .
    echo "    $JAR_FILE"
}

mkdir -p "$BUILD_DIR"

case "${1:-all}" in
    native)
        build_native
        ;;
    java)
        build_java
        ;;
    test)
        build_native
        build_java
        build_tests
        run_tests
        ;;
    jar)
        build_native
        build_java
        build_jar
        ;;
    all)
        build_native
        build_java
        build_tests
        run_tests
        build_jar
        ;;
    *)
        echo "Unknown target '${1}'. Use native, java, test, jar or all." >&2
        exit 1
        ;;
esac
