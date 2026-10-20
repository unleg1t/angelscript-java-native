package org.angelscript;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Locale;

/**
 * Loads the native AngelScript bridge library.
 *
 * <p>The library is looked up in the following order:</p>
 * <ol>
 *   <li>the system property {@code angelscript.native.path} (a file or a directory)</li>
 *   <li>the environment variable {@code ANGELSCRIPT_NATIVE_PATH}</li>
 *   <li>the directory {@code <jar>/native/<platform>} extracted from the class path</li>
 *   <li>the directory {@code prebuilt/<platform>} relative to the working directory</li>
 * </ol>
 */
public final class Native {

    /** System property that overrides the location of the native library. */
    public static final String PATH_PROPERTY = "angelscript.native.path";

    /** Environment variable that overrides the location of the native library. */
    public static final String PATH_ENV = "ANGELSCRIPT_NATIVE_PATH";

    private static boolean loaded;
    private static String loadedFrom;

    private Native() {
    }

    /**
     * Loads the native library if it has not been loaded yet.
     *
     * @throws UnsatisfiedLinkError if the library cannot be found or loaded
     */
    public static synchronized void load() {
        if (loaded) {
            return;
        }
        String platform = platform();
        String fileName = libraryFileName(platform);

        UnsatisfiedLinkError failure = null;

        String override = System.getProperty(PATH_PROPERTY);
        if (override == null || override.isEmpty()) {
            override = System.getenv(PATH_ENV);
        }
        if (override != null && !override.isEmpty()) {
            try {
                loadFrom(new File(override), fileName);
                loaded = true;
                loadedFrom = override;
                return;
            } catch (UnsatisfiedLinkError e) {
                failure = e;
            }
        }

        // Inside a jar: /native/<platform>/<library>
        try {
            InputStream in = Native.class.getResourceAsStream("/native/" + platform + "/" + fileName);
            if (in != null) {
                Path tmp = Files.createTempDirectory("angelscript-native");
                tmp.toFile().deleteOnExit();
                Path target = tmp.resolve(fileName);
                try (OutputStream out = new FileOutputStream(target.toFile())) {
                    copy(in, out);
                }
                target.toFile().deleteOnExit();
                tmp.toFile().deleteOnExit();
                System.load(target.toAbsolutePath().toString());
                loaded = true;
                loadedFrom = target.toAbsolutePath().toString();
                return;
            }
        } catch (IOException | UnsatisfiedLinkError e) {
            failure = e instanceof UnsatisfiedLinkError ? (UnsatisfiedLinkError) e
                    : new UnsatisfiedLinkError(e.getMessage());
        }

        // Development checkout: prebuilt/<platform>/<library>
        File local = new File("prebuilt", platform);
        if (local.isDirectory()) {
            try {
                loadFrom(local, fileName);
                loaded = true;
                loadedFrom = local.getAbsolutePath();
                return;
            } catch (UnsatisfiedLinkError e) {
                failure = e;
            }
        }

        if (failure != null) {
            throw failure;
        }
        throw new UnsatisfiedLinkError(
                "Could not find the native AngelScript bridge for platform '" + platform
                        + "'. Expected file '" + fileName + "'. Set the system property '"
                        + PATH_PROPERTY + "' to the directory containing it.");
    }

    /** @return the path the native library was loaded from, or null if it was not loaded yet. */
    public static synchronized String getLoadedFrom() {
        return loadedFrom;
    }

    /** @return the platform identifier used to locate the native library. */
    public static String platform() {
        String os = System.getProperty("os.name", "").toLowerCase(Locale.ROOT);
        String arch = System.getProperty("os.arch", "").toLowerCase(Locale.ROOT);

        String osName;
        if (os.contains("win")) {
            osName = "windows";
        } else if (os.contains("mac") || os.contains("darwin")) {
            osName = "macos";
        } else if (os.contains("nux") || os.contains("nix") || os.contains("aix")
                || os.contains("bsd") || os.contains("sunos") || os.contains("solaris")) {
            osName = "linux";
        } else {
            osName = os.replaceAll("[^a-z0-9]+", "");
        }

        String archName;
        if (arch.equals("amd64") || arch.equals("x86_64") || arch.equals("x64")) {
            archName = "x86_64";
        } else if (arch.equals("aarch64") || arch.equals("arm64")) {
            archName = "aarch64";
        } else if (arch.startsWith("arm")) {
            archName = "armv7";
        } else if (arch.equals("x86") || arch.equals("i386") || arch.equals("i486")
                || arch.equals("i586") || arch.equals("i686")) {
            archName = "x86";
        } else {
            archName = arch.replaceAll("[^a-z0-9]+", "");
        }

        return osName + "-" + archName;
    }

    private static String libraryFileName(String platform) {
        if (platform.startsWith("windows")) {
            return "angelscript-jni.dll";
        }
        if (platform.startsWith("macos")) {
            return "libangelscript-jni.dylib";
        }
        return "libangelscript-jni.so";
    }

    private static void loadFrom(File dirOrFile, String fileName) {
        File file = dirOrFile.isDirectory() ? new File(dirOrFile, fileName) : dirOrFile;
        if (!file.isFile()) {
            throw new UnsatisfiedLinkError("Native library not found: " + file.getAbsolutePath());
        }
        System.load(file.getAbsolutePath());
    }

    private static void copy(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[16 * 1024];
        int read;
        while ((read = in.read(buffer)) > 0) {
            out.write(buffer, 0, read);
        }
    }
}
