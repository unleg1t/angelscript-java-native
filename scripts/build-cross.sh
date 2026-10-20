#!/usr/bin/env bash
#
# Builds the prebuilt native libraries for every supported platform.
#
# The Windows and macOS libraries are cross compiled with the zig toolchain, which bundles the
# needed C and C++ runtime libraries. The Linux libraries are built with the native compiler.
#
# Usage:
#   ./scripts/build-cross.sh            build every platform
#   ./scripts/build-cross.sh macos      build only the macOS libraries
#
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CACHE="${ANGELSCRIPT_CROSS_CACHE:-$ROOT/build/cross}"
ZIG_VERSION="${ZIG_VERSION:-0.13.0}"
ZIG_DIR="$CACHE/zig-linux-x86_64-$ZIG_VERSION"
ZIG="$ZIG_DIR/zig"

mkdir -p "$CACHE"

if [ ! -x "$ZIG" ]; then
    echo "==> Downloading zig $ZIG_VERSION"
    curl -sL -o "$CACHE/zig.tar.xz" \
        "https://ziglang.org/download/$ZIG_VERSION/zig-linux-x86_64-$ZIG_VERSION.tar.xz"
    tar xf "$CACHE/zig.tar.xz" -C "$CACHE"
fi

TOOLS="$CACHE/tools"
mkdir -p "$TOOLS/bin"
export PATH="$TOOLS/bin:$PATH"

# A stub for the tool that CMake looks for when targeting macOS. It is only used to detect the
# platform and is never invoked during the build.
cat > "$TOOLS/bin/install_name_tool" <<'EOF'
#!/bin/sh
exit 0
EOF
chmod +x "$TOOLS/bin/install_name_tool"

# The wrappers strip the flags that CMake adds for the target platform but that zig does not
# understand, and force the correct target.
make_wrapper() {
    local name="$1" driver="$2" target="$3"
    cat > "$TOOLS/$name" <<EOF
#!/bin/bash
args=()
skip=0
for a in "\$@"; do
    if [ "\$skip" = "1" ]; then skip=0; continue; fi
    if [ "\$a" = "-arch" ] || [ "\$a" = "-target" ]; then skip=1; continue; fi
    case "\$a" in --target=*) continue;; esac
    args+=("\$a")
done
exec $ZIG $driver -target $target "\${args[@]}"
EOF
    chmod +x "$TOOLS/$name"
}

make_wrapper zig-cc-aarch64-macos cc aarch64-macos
make_wrapper zig-cxx-aarch64-macos c++ aarch64-macos
make_wrapper zig-cc-x86_64-macos cc x86_64-macos
make_wrapper zig-cxx-x86_64-macos c++ x86_64-macos
make_wrapper zig-cc-x86_64-windows cc x86_64-windows-gnu
make_wrapper zig-cxx-x86_64-windows c++ x86_64-windows-gnu
make_wrapper zig-cc-aarch64-linux cc aarch64-linux-gnu
make_wrapper zig-cxx-aarch64-linux c++ aarch64-linux-gnu

cat > "$TOOLS/zig-ar-darwin" <<EOF
#!/bin/bash
exec $ZIG ar --format=darwin "\$@"
EOF
cat > "$TOOLS/zig-ar-gnu" <<EOF
#!/bin/bash
exec $ZIG ar --format=gnu "\$@"
EOF
cat > "$TOOLS/zig-ranlib" <<EOF
#!/bin/bash
exec $ZIG ranlib "\$@"
EOF
chmod +x "$TOOLS/zig-ar-darwin" "$TOOLS/zig-ar-gnu" "$TOOLS/zig-ranlib"

write_toolchain() {
    local file="$1" system="$2" processor="$3" cc="$4" cxx="$5" ar="$6"
    cat > "$TOOLS/$file" <<EOF
set(CMAKE_SYSTEM_NAME $system)
set(CMAKE_SYSTEM_PROCESSOR $processor)
set(CMAKE_C_COMPILER $TOOLS/$cc)
set(CMAKE_CXX_COMPILER $TOOLS/$cxx)
set(CMAKE_AR $TOOLS/$ar CACHE FILEPATH "")
set(CMAKE_RANLIB $TOOLS/zig-ranlib CACHE FILEPATH "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
}

write_toolchain darwin-aarch64.cmake Darwin arm64 zig-cc-aarch64-macos zig-cxx-aarch64-macos zig-ar-darwin
write_toolchain darwin-x86_64.cmake Darwin x86_64 zig-cc-x86_64-macos zig-cxx-x86_64-macos zig-ar-darwin
write_toolchain windows-x86_64.cmake Windows x86_64 zig-cc-x86_64-windows zig-cxx-x86_64-windows zig-ar-gnu
write_toolchain linux-aarch64.cmake Linux aarch64 zig-cc-aarch64-linux zig-cxx-aarch64-linux zig-ar-gnu

build() {
    local platform="$1" toolchain="$2"
    echo "==> Building $platform"
    cmake -S "$ROOT/native" -B "$ROOT/build/cross/$platform" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLS/$toolchain" \
        -DCMAKE_BUILD_TYPE=Release \
        -DAS_JNI_INSTALL_DIR="$ROOT/prebuilt/$platform"
    cmake --build "$ROOT/build/cross/$platform" -j "$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"
    strip_library "$ROOT/prebuilt/$platform"
}

# Removes the debug information. The system strip cannot handle every architecture, so the
# objcopy of the zig toolchain is used as a fallback.
strip_library() {
    local directory="$1"
    for library in "$directory"/*; do
        [ -f "$library" ] || continue
        if ! strip --strip-unneeded "$library" 2>/dev/null; then
            "$ZIG" objcopy --strip-all "$library" "$library.stripped" 2>/dev/null &&
                mv "$library.stripped" "$library" || true
        fi
    done
}

TARGETS="${1:-all}"

if [ "$TARGETS" = "all" ] || [ "$TARGETS" = "macos" ]; then
    build macos-aarch64 darwin-aarch64.cmake
    build macos-x86_64 darwin-x86_64.cmake
fi

if [ "$TARGETS" = "all" ] || [ "$TARGETS" = "windows" ]; then
    build windows-x86_64 windows-x86_64.cmake
fi

if [ "$TARGETS" = "all" ] || [ "$TARGETS" = "linux" ]; then
    build linux-aarch64 linux-aarch64.cmake
fi

echo "==> Done"
ls -l "$ROOT/prebuilt"/*/
