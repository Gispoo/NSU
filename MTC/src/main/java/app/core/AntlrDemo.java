package app.core;

import app.core.antlr.BasicInputLexer;
import app.core.antlr.BasicInputParser;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.Comparator;

public class AntlrDemo {
    public static void main(String[] args) throws Exception {
        String text =
                "INPUT \"Введите X:\"; X\n" +
                        "INPUT A,B,C\n" +
                        "INPT Z\n" +
                        "INPUT \"незакрытая строка; X\n";

        CharStream cs = CharStreams.fromString(text);

        BasicInputLexer lexer = new BasicInputLexer(cs);
        CollectingErrorListener el = new CollectingErrorListener();
        lexer.removeErrorListeners();
        lexer.addErrorListener(el);

        CommonTokenStream tokens = new CommonTokenStream(lexer);
        BasicInputParser parser = new BasicInputParser(tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(el);

        ParseTree tree = parser.program();

        el.getErrors().stream()
                .sorted(Comparator.comparingInt((CollectingErrorListener.Err e) -> e.line)
                        .thenComparingInt(e -> e.col))
                .forEach(System.out::println);

        if (el.getErrors().isEmpty()) {
            System.out.println("OK: разбор успешен");
        }
    }
}
