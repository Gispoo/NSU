package app.core;

import app.core.antlr.BasicInputLexer;
import app.core.antlr.BasicInputParser;
import org.antlr.v4.runtime.*;

import java.util.Comparator;
import java.util.List;

public final class AntlrInputChecker {

    public static String check(String text) {
        CollectingErrorListener el = new CollectingErrorListener();

        CharStream cs = CharStreams.fromString(text);

        BasicInputLexer lexer = new BasicInputLexer(cs);
        lexer.removeErrorListeners();
        lexer.addErrorListener(el);

        CommonTokenStream tokens = new CommonTokenStream(lexer);

        BasicInputParser parser = new BasicInputParser(tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(el);

        parser.program(); // разбор всего текста

        List<CollectingErrorListener.Err> errors = el.getErrors();
        if (errors.isEmpty()) {
            return "OK: разбор успешен, ошибок нет.";
        }

        errors.sort(Comparator
                .comparingInt((CollectingErrorListener.Err e) -> e.line)
                .thenComparingInt(e -> e.col));

        StringBuilder sb = new StringBuilder();
        for (var e : errors) sb.append(e).append('\n');
        return sb.toString();
    }

    private AntlrInputChecker() {}
}
