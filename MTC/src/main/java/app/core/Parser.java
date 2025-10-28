package app.core;

import java.util.ArrayList;
import java.util.List;

public class Parser {
    private final List<Token> tokens;
    private int i = 0;
    private final List<ParseError> errors = new ArrayList<>();

    public Parser(List<Token> tokens) { this.tokens = tokens; }

    private Token cur() { return tokens.get(i); }
    private Token next() { if (i < tokens.size()-1) i++; return cur(); }

    private boolean match(TokenKind kind, String msgIfFail) {
        if (cur().kind == kind) { next(); return true; }
        error(cur(), msgIfFail != null ? msgIfFail : ("ожидалось " + kind + ", найдено " + cur().kind));
        return false;
    }

    private void error(Token t, String msg) { errors.add(new ParseError(t.line, t.col, msg)); }

    private void sync(TokenKind... kinds) {
        outer: while (cur().kind != TokenKind.EOF && cur().kind != TokenKind.EOL) {
            for (TokenKind k : kinds) if (cur().kind == k) break outer;
            next();
        }
    }

    public List<ParseError> parseProgram() {
        while (cur().kind != TokenKind.EOF) {
            parseZ();
            if (cur().kind == TokenKind.EOL) next();
            else if (cur().kind != TokenKind.EOF) {
                error(cur(), "ожидался конец строки");
                sync(TokenKind.EOL);
                if (cur().kind == TokenKind.EOL) next();
            }
        }
        return errors;
    }

    // Z → INPUT H VarList EOL
    private void parseZ() {
        if (cur().kind == TokenKind.EOL) return; // пустая строка
        if (cur().kind != TokenKind.INPUT) {
            error(cur(), "ожидалось ключевое слово INPUT");
            sync(TokenKind.EOL);
            return;
        }
        match(TokenKind.INPUT, null);
        parseH();
        parseVarList();
    }

    // H → STRING ',' | STRING ';' | ε
    private void parseH() {
        if (cur().kind == TokenKind.STRING) {
            match(TokenKind.STRING, null);
            if (cur().kind == TokenKind.COMMA) match(TokenKind.COMMA, null);
            else if (cur().kind == TokenKind.SEMI) match(TokenKind.SEMI, null);
            else {
                error(cur(), "после подсказки ожидались ',' или ';'");
                if (cur().kind != TokenKind.ID) sync(TokenKind.ID);
            }
        }
    }

    // VarList → Var T
    private void parseVarList() {
        if (cur().kind != TokenKind.ID) {
            error(cur(), "ожидался идентификатор переменной");
            sync(TokenKind.ID, TokenKind.EOL);
            if (cur().kind != TokenKind.ID) return;
        }
        parseVar();
        parseT();
    }

    // T → ',' Var T | ε (loop)
    private void parseT() {
        while (cur().kind == TokenKind.COMMA) {
            match(TokenKind.COMMA, null);
            if (cur().kind != TokenKind.ID) {
                error(cur(), "после ',' ожидался идентификатор");
                sync(TokenKind.ID, TokenKind.EOL);
                if (cur().kind != TokenKind.ID) break;
            }
            parseVar();
        }
    }

    // Var → ID
    private void parseVar() { match(TokenKind.ID, "ожидался идентификатор переменной"); }
}