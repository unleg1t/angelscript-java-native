cmake_minimum_required(VERSION 3.16)

project(angelscript_jni LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(AS_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sdk" CACHE PATH "Path to the AngelScript SDK")
set(AS_SOURCE_DIR "${AS_SDK_DIR}/angelscript/source")
set(AS_INCLUDE_DIR "${AS_SDK_DIR}/angelscript/include")
set(AS_ADDON_DIR "${AS_SDK_DIR}/add_on")

if(NOT EXISTS "${AS_INCLUDE_DIR}/angelscript.h")
    message(FATAL_ERROR "AngelScript SDK not found at ${AS_SDK_DIR}")
endif()

# ---------------------------------------------------------------------------
# The AngelScript core
# ---------------------------------------------------------------------------

set(ANGELSCRIPT_SOURCE
    ${AS_SOURCE_DIR}/as_atomic.cpp
    ${AS_SOURCE_DIR}/as_builder.cpp
    ${AS_SOURCE_DIR}/as_bytecode.cpp
    ${AS_SOURCE_DIR}/as_callfunc.cpp
    ${AS_SOURCE_DIR}/as_callfunc_mips.cpp
    ${AS_SOURCE_DIR}/as_callfunc_x86.cpp
    ${AS_SOURCE_DIR}/as_callfunc_x64_gcc.cpp
    ${AS_SOURCE_DIR}/as_callfunc_x64_msvc.cpp
    ${AS_SOURCE_DIR}/as_callfunc_x64_mingw.cpp
    ${AS_SOURCE_DIR}/as_compiler.cpp
    ${AS_SOURCE_DIR}/as_configgroup.cpp
    ${AS_SOURCE_DIR}/as_context.cpp
    ${AS_SOURCE_DIR}/as_datatype.cpp
    ${AS_SOURCE_DIR}/as_gc.cpp
    ${AS_SOURCE_DIR}/as_generic.cpp
    ${AS_SOURCE_DIR}/as_globalproperty.cpp
    ${AS_SOURCE_DIR}/as_memory.cpp
    ${AS_SOURCE_DIR}/as_module.cpp
    ${AS_SOURCE_DIR}/as_objecttype.cpp
    ${AS_SOURCE_DIR}/as_outputbuffer.cpp
    ${AS_SOURCE_DIR}/as_parser.cpp
    ${AS_SOURCE_DIR}/as_restore.cpp
    ${AS_SOURCE_DIR}/as_scriptcode.cpp
    ${AS_SOURCE_DIR}/as_scriptengine.cpp
    ${AS_SOURCE_DIR}/as_scriptfunction.cpp
    ${AS_SOURCE_DIR}/as_scriptnode.cpp
    ${AS_SOURCE_DIR}/as_scriptobject.cpp
    ${AS_SOURCE_DIR}/as_string.cpp
    ${AS_SOURCE_DIR}/as_string_util.cpp
    ${AS_SOURCE_DIR}/as_thread.cpp
    ${AS_SOURCE_DIR}/as_tokenizer.cpp
    ${AS_SOURCE_DIR}/as_typeinfo.cpp
    ${AS_SOURCE_DIR}/as_variablescope.cpp
)

set(ARCHFLAGS "${CMAKE_SYSTEM_PROCESSOR}")
if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        set(ARCHFLAGS "aarch64")
    endif()
endif()

if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    enable_language(ASM_MASM)
    if(CMAKE_ASM_MASM_COMPILER_WORKS)
        list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_x64_msvc_asm.asm)
    else()
        message(FATAL_ERROR "MSVC x86_64 target requires a working assembler")
    endif()
endif()

if(ARCHFLAGS MATCHES "^arm64" OR ARCHFLAGS MATCHES "^aarch64")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_arm64.cpp)
        if(APPLE)
            list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_arm64_xcode.S)
        else()
            list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_arm64_gcc.S)
        endif()
    else()
        message(FATAL_ERROR "ARM64 target requires a working assembler")
    endif()
endif()

if(ARCHFLAGS MATCHES "^arm" AND NOT ARCHFLAGS MATCHES "^a?arch64")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_arm.cpp
                                          ${AS_SOURCE_DIR}/as_callfunc_arm_gcc.S)
        set_property(SOURCE ${AS_SOURCE_DIR}/as_callfunc_arm_gcc.S APPEND PROPERTY
                     COMPILE_FLAGS " -Wa,-mimplicit-it=always")
    else()
        message(FATAL_ERROR "ARM target requires a working assembler")
    endif()
endif()

if(ARCHFLAGS MATCHES "^riscv64")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_riscv64.cpp
                                          ${AS_SOURCE_DIR}/as_callfunc_riscv64_gcc.S)
    else()
        message(FATAL_ERROR "RISC-V 64 target requires a working assembler")
    endif()
endif()

if(ARCHFLAGS MATCHES "^e2k")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        list(APPEND ANGELSCRIPT_SOURCE ${AS_SOURCE_DIR}/as_callfunc_e2k.cpp
                                          ${AS_SOURCE_DIR}/as_callfunc_e2k.S)
    else()
        message(FATAL_ERROR "E2K target requires a working assembler")
    endif()
endif()

find_package(Threads)

if(APPLE AND CMAKE_CROSSCOMPILING AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # The LLVM integrated assembler does not accept the CFI directives that the x86_64 call
    # trampolines emit from their inline assembly. They are only needed for stack unwinding
    # through the trampoline, which the script engine never does.
    set_source_files_properties(${AS_SOURCE_DIR}/as_callfunc_x64_gcc.cpp
                                PROPERTIES COMPILE_OPTIONS "-U__OPTIMIZE__")
endif()

add_library(angelscript STATIC ${ANGELSCRIPT_SOURCE})
target_include_directories(angelscript PUBLIC ${AS_INCLUDE_DIR})
if(Threads_FOUND)
    target_link_libraries(angelscript PUBLIC Threads::Threads)
endif()
if(MSVC)
    target_compile_definitions(angelscript PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(angelscript PRIVATE /MP)
endif()
if(UNIX AND NOT APPLE)
    target_compile_options(angelscript PRIVATE -fPIC)
endif()

# ---------------------------------------------------------------------------
# The AngelScript add-ons
# ---------------------------------------------------------------------------

set(ADDON_SOURCE
    ${AS_ADDON_DIR}/scriptstdstring/scriptstdstring.cpp
    ${AS_ADDON_DIR}/scriptarray/scriptarray.cpp
    ${AS_ADDON_DIR}/scriptdictionary/scriptdictionary.cpp
    ${AS_ADDON_DIR}/scriptany/scriptany.cpp
    ${AS_ADDON_DIR}/scriptmath/scriptmath.cpp
    ${AS_ADDON_DIR}/scriptmath/scriptmathcomplex.cpp
    ${AS_ADDON_DIR}/datetime/datetime.cpp
    ${AS_ADDON_DIR}/scripthandle/scripthandle.cpp
    ${AS_ADDON_DIR}/scriptbuilder/scriptbuilder.cpp
    ${AS_ADDON_DIR}/scriptgrid/scriptgrid.cpp
    ${AS_ADDON_DIR}/scripthelper/scripthelper.cpp
    ${AS_ADDON_DIR}/serializer/serializer.cpp
    ${AS_ADDON_DIR}/weakref/weakref.cpp
    ${AS_ADDON_DIR}/contextmgr/contextmgr.cpp
    ${AS_ADDON_DIR}/debugger/debugger.cpp
)

# The file system add-on needs the Windows SDK headers, which are not always available when
# cross compiling for Windows.
if(WIN32)
    find_file(WINDOWS_SDK_HEADER Windows.h
              HINTS "$ENV{WindowsSdkDir}/Include" "$ENV{WindowsSdkDir}/Include/10.0.22000.0/um"
              "$ENV{WindowsSdkDir}/Include/10.0.22621.0/um" "$ENV{WindowsSdkDir}/Include/10.0.26100.0/um"
              NO_CMAKE_FIND_ROOT_PATH)
    if(NOT WINDOWS_SDK_HEADER)
        file(GLOB WINDOWS_SDK_CANDIDATES "$ENV{WindowsSdkDir}/Include/*/um" "C:/Program Files (x86)/Windows Kits/*/Include/*/um")
        foreach(candidate ${WINDOWS_SDK_CANDIDATES})
            if(EXISTS "${candidate}/Windows.h")
                set(WINDOWS_SDK_HEADER "${candidate}/Windows.h")
                break()
            endif()
        endforeach()
    endif()
    if(WINDOWS_SDK_HEADER)
        list(APPEND ADDON_SOURCE ${AS_ADDON_DIR}/scriptfile/scriptfilesystem.cpp)
        set(AS_JNI_FILE_SYSTEM ON)
    else()
        message(STATUS "Windows SDK headers not found, the file system add-on is not included")
    endif()
else()
    list(APPEND ADDON_SOURCE ${AS_ADDON_DIR}/scriptfile/scriptfilesystem.cpp)
    set(AS_JNI_FILE_SYSTEM ON)
endif()

add_library(angelscript_addons STATIC ${ADDON_SOURCE})
target_include_directories(angelscript_addons PUBLIC ${AS_INCLUDE_DIR} ${AS_ADDON_DIR})
target_link_libraries(angelscript_addons PUBLIC angelscript)
if(UNIX AND NOT APPLE)
    target_compile_options(angelscript_addons PRIVATE -fPIC)
endif()

# ---------------------------------------------------------------------------
# The JNI bridge
# ---------------------------------------------------------------------------

# Locate the JNI headers. The JVM library itself is not needed, the JNI functions are resolved by
# the virtual machine at runtime.
find_path(JNI_HEADER_DIR jni.h
          HINTS $ENV{JAVA_HOME}/include $ENV{JDK_HOME}/include
          PATHS /usr/lib/jvm/default/include /usr/lib/jvm/java/include
          PATH_SUFFIXES include
          NO_CMAKE_FIND_ROOT_PATH)
if(NOT JNI_HEADER_DIR)
    file(GLOB JNI_JDK_CANDIDATES "/usr/lib/jvm/*/include" "/usr/lib/jvm/*/*/include"
         "/usr/java/*/include" "/opt/java/*/include" "/Library/Java/JavaVirtualMachines/*/Contents/Home/include")
    foreach(candidate ${JNI_JDK_CANDIDATES})
        if(EXISTS "${candidate}/jni.h")
            set(JNI_HEADER_DIR "${candidate}")
            break()
        endif()
    endforeach()
endif()
if(NOT JNI_HEADER_DIR)
    message(FATAL_ERROR "Could not find the JNI headers. Set JAVA_HOME to your JDK.")
endif()

if(WIN32)
    set(JNI_PLATFORM_DIR "win32")
elseif(APPLE)
    set(JNI_PLATFORM_DIR "darwin")
else()
    set(JNI_PLATFORM_DIR "linux")
endif()

set(JNI_INCLUDE_DIRS ${JNI_HEADER_DIR})

if(EXISTS "${JNI_HEADER_DIR}/${JNI_PLATFORM_DIR}/jni_md.h")
    list(APPEND JNI_INCLUDE_DIRS "${JNI_HEADER_DIR}/${JNI_PLATFORM_DIR}")
else()
    # When cross compiling the JDK of the host system does not contain the headers of the target
    # platform. Generate the small platform specific header instead.
    set(JNI_MD_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/jni_md_${JNI_PLATFORM_DIR}")
    file(WRITE "${JNI_MD_INCLUDE_DIR}/jni_md.h"
"#ifndef _JAVASOFT_JNI_MD_H_\n"
"#define _JAVASOFT_JNI_MD_H_\n")
    if(WIN32)
        file(APPEND "${JNI_MD_INCLUDE_DIR}/jni_md.h"
"#define JNIEXPORT __declspec(dllexport)\n"
"#define JNIIMPORT __declspec(dllimport)\n"
"#define JNICALL __stdcall\n"
"typedef long jint;\n"
"typedef __int64 jlong;\n"
"typedef signed char jbyte;\n")
    elseif(APPLE)
        file(APPEND "${JNI_MD_INCLUDE_DIR}/jni_md.h"
"#define JNIEXPORT __attribute__((visibility(\"default\")))\n"
"#define JNIIMPORT __attribute__((visibility(\"default\")))\n"
"#define JNICALL\n"
"typedef int jint;\n"
"typedef long long jlong;\n"
"typedef signed char jbyte;\n")
    else()
        file(APPEND "${JNI_MD_INCLUDE_DIR}/jni_md.h"
"#define JNIEXPORT __attribute__((visibility(\"default\")))\n"
"#define JNIIMPORT __attribute__((visibility(\"default\")))\n"
"#define JNICALL\n"
"typedef int jint;\n"
"typedef long long jlong;\n"
"typedef signed char jbyte;\n")
    endif()
    file(APPEND "${JNI_MD_INCLUDE_DIR}/jni_md.h" "#endif\n")
    list(APPEND JNI_INCLUDE_DIRS ${JNI_MD_INCLUDE_DIR})
endif()

set(JNI_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_streams.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_addons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_javatypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asjava_memory.cpp
)

add_library(angelscript-jni SHARED ${JNI_SOURCE})
if(AS_JNI_FILE_SYSTEM)
    target_compile_definitions(angelscript-jni PRIVATE AS_JNI_WITH_FILE_SYSTEM)
endif()
target_include_directories(angelscript-jni PRIVATE ${JNI_INCLUDE_DIRS}
                                                ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                ${AS_INCLUDE_DIR}
                                                ${AS_ADDON_DIR})
target_link_libraries(angelscript-jni PRIVATE angelscript_addons angelscript)

if(WIN32)
    set_target_properties(angelscript-jni PROPERTIES PREFIX "" OUTPUT_NAME "angelscript-jni")
elseif(APPLE)
    set_target_properties(angelscript-jni PROPERTIES
        INSTALL_NAME "angelscript-jni"
        SUFFIX ".dylib")
else()
    set_target_properties(angelscript-jni PROPERTIES OUTPUT_NAME "angelscript-jni")
endif()

# Copy the freshly built library into the prebuilt directory of the current platform.
if(AS_JNI_INSTALL_DIR)
    add_custom_command(TARGET angelscript-jni POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${AS_JNI_INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:angelscript-jni> ${AS_JNI_INSTALL_DIR}
        COMMENT "Copying the native library to ${AS_JNI_INSTALL_DIR}")
endif()
