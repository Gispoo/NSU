package app.core;

import java.util.ArrayList;
import java.util.List;

public class Lexer {
    private final String src;
    private int i = 0;
    private int line = 1;
    private int col = 1;

    public Lexer(String src) { this.src = src != null ? src : ""; }

    private char peek(int k) {
        int j = i + k;
        if (j >= src.length()) return '\0';
        return src.charAt(j);
    }

    private char peek() { return peek(0); }

    private void advance() {
        if (i < src.length()) {
            char ch = src.charAt(i++);
            if (ch == '\n') { line++; col = 1; }
            else { col++; }
        }
    }

    private void add(List<Token> out, TokenKind kind, String lex, int l, int c) {
        out.add(new Token(kind, lex, l, c));
    }

    private static boolean isAlpha(char ch) {
        return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    }

    private static boolean isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }

    private static boolean isAlnum(char ch) {
        return isAlpha(ch) || isDigit(ch);
    }

    public List<Token> lex() {
        List<Token> tokens = new ArrayList<>();
        while (i < src.length()) {
            char ch = peek();
            if (ch == ' ' || ch == '\t' || ch == '\r') { advance(); continue; }

            if (ch == '\'') {
                // апостроф — комментарий до конца строки
                while (i < src.length() && peek() != '\n') advance();
                continue;
            }

            if (ch == 'R' && peek(1) == 'E' && peek(2) == 'M'
                    && !Character.isLetterOrDigit(peek(3))) {
                while (i < src.length() && peek() != '\n') {
                    advance();
                }
                continue;
            }

            if (ch == '\n') { add(tokens, TokenKind.EOL, "\\n", line, col); advance(); continue; }
            if (ch == ',') { add(tokens, TokenKind.COMMA, ",", line, col); advance(); continue; }
            if (ch == ';') { add(tokens, TokenKind.SEMI, ";", line, col); advance(); continue; }

            if (ch == '"') {
                int l = line, c = col;
                StringBuilder sb = new StringBuilder();
                sb.append('"'); advance();
                while (i < src.length() && peek() != '"' && peek() != '\n') {
                    sb.append(peek()); advance();
                }
                if (peek() == '"') { sb.append('"'); advance(); add(tokens, TokenKind.STRING, sb.toString(), l, c); }
                else { add(tokens, TokenKind.ERROR, "Незакрытый строковый литерал", l, c); }
                continue;
            }

            if (isAlpha(ch)) {
                int l = line, c = col;
                StringBuilder sb = new StringBuilder();
                sb.append(ch);
                advance();
                while (isAlnum(peek())) {
                    sb.append(peek());
                    advance();
                }
                String lex = sb.toString();
                // регистрозависимо: только "INPUT" — ключевое слово
                if (lex.equals("INPUT")) {
                    add(tokens, TokenKind.INPUT, lex, l, c);
                } else {
                    add(tokens, TokenKind.ID, lex, l, c);
                }
                continue;
            }


// unknown char
            add(tokens, TokenKind.ERROR, "Неожиданный символ: " + ch, line, col);
            advance();
        }
        if (tokens.isEmpty() || tokens.get(tokens.size()-1).kind != TokenKind.EOL) {
            add(tokens, TokenKind.EOL, "\\n", line, col);
        }
        add(tokens, TokenKind.EOF, "", line, col);
        return tokens;
    }
}