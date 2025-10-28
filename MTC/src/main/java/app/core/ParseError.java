package app.core;

public class ParseError {
    public final int line, col; public final String message;
    public ParseError(int line, int col, String message) { this.line = line; this.col = col; this.message = message; }
    @Override public String toString() { return "строка " + line + ", позиция " + col + ": " + message; }
}