package app.core;

import org.antlr.v4.runtime.*;

import java.util.ArrayList;
import java.util.List;

public class CollectingErrorListener extends BaseErrorListener {
    public static class Err {
        public final int line, col;
        public final String msg;
        public Err(int line, int col, String msg) { this.line = line; this.col = col; this.msg = msg; }
        @Override public String toString() { return "Строка " + line + ", позиция " + col + ": " + msg; }
    }

    private final List<Err> errors = new ArrayList<>();
    public List<Err> getErrors() { return errors; }

    @Override
    public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol,
                            int line, int charPositionInLine, String msg, RecognitionException e) {
        errors.add(new Err(line, charPositionInLine + 1, msg));
    }
}
