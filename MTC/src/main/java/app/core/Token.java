package app.core;

public class Token {
    public final TokenKind kind;
    public final String lexeme;
    public final int line;
    public final int col;

    public Token(TokenKind kind, String lexeme, int line, int col) {
        this.kind = kind;
        this.lexeme = lexeme;
        this.line = line;
        this.col = col;
    }

    @Override public String toString() {
        return kind + "('" + lexeme + "' @" + line + ":" + col + ")";
    }
}