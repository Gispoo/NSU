package app.util;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

public final class TextFiles {
    private TextFiles() {}
    public static String readString(Path p) {
        try { return Files.readString(p, StandardCharsets.UTF_8); }
        catch (IOException e) { return ""; }
    }
    public static String readString(Path p, String fallback) {
        try { return Files.readString(p, StandardCharsets.UTF_8); }
        catch (IOException e) { return fallback; }
    }
    public static void writeString(Path p, String s) {
        try { Files.createDirectories(p.getParent()); Files.writeString(p, s, StandardCharsets.UTF_8); }
        catch (IOException ignored) {}
    }
}