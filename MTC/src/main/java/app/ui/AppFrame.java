package app.ui;

import app.core.*;
import app.util.TextFiles;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class AppFrame extends JFrame {
    public static final String APP_TITLE = "BASIC INPUT Parser (Java)";

    private final JTextArea editor = new JTextArea();
    private final JTextArea output = new JTextArea();
    private File currentFile = null;
    private int fontSize = 30;
    private boolean dirty = false;

    private final Path baseDir = Paths.get(".").toAbsolutePath().normalize();
    private final Path assetsDir = baseDir.resolve("assets");
    private final Path testsDir = baseDir.resolve("tests");
    private final Path autosaveDir = baseDir.resolve("autosave");
    private final Path autosavePath = autosaveDir.resolve("last_session.txt");

    public AppFrame() {
        super(APP_TITLE);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        setSize(1000, 700);
        setLocationRelativeTo(null);
        buildUI();
        bindEvents();
        tryRestoreAutosaveOrLoadTests();
    }

    private void buildUI() {
        setLayout(new BorderLayout());
        setJMenuBar(buildMenuBar());

        editor.setFont(new Font("Consolas", Font.PLAIN, fontSize));
        editor.setLineWrap(false);
        editor.getDocument().addDocumentListener((SimpleDocumentListener) e -> dirty = true);

        output.setEditable(false);
        output.setFont(new Font("Consolas", Font.PLAIN, fontSize));

        JScrollPane editorScroll = new JScrollPane(editor);
        editorScroll.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        editorScroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);

        JPanel bottom = new JPanel(new BorderLayout());
        JLabel label = new JLabel("Результаты / Ошибки:");
        label.setFont(new Font("SansSerif", Font.BOLD, 20));
        bottom.add(label, BorderLayout.NORTH);
        bottom.add(new JScrollPane(output), BorderLayout.CENTER);

        JSplitPane split = new JSplitPane(JSplitPane.VERTICAL_SPLIT, editorScroll, bottom);
        split.setResizeWeight(0.7);
        add(split, BorderLayout.CENTER);

        addWindowListener(new java.awt.event.WindowAdapter() {
            @Override public void windowClosing(java.awt.event.WindowEvent e) {
                onExit();
            }
        });
    }

    private JMenuBar buildMenuBar() {
        JMenuBar bar = new JMenuBar();
        Font menuFont = new Font("SansSerif", Font.BOLD, 20); // bigger menu font

        JMenu file = new JMenu("Файл");
        file.setFont(menuFont);
        JMenuItem miNew = new JMenuItem(new AbstractAction("Создать") {
            public void actionPerformed(ActionEvent e) { newFile(); }
        });
        JMenuItem miOpen = new JMenuItem(new AbstractAction("Открыть…") {
            public void actionPerformed(ActionEvent e) { openFile(); }
        });
        JMenuItem miOpenTests = new JMenuItem(new AbstractAction("Открыть тесты…") {
            public void actionPerformed(ActionEvent e) { openTests(); }
        });
        JMenuItem miSave = new JMenuItem(new AbstractAction("Сохранить") {
            public void actionPerformed(ActionEvent e) { saveFile(); }
        });
        JMenuItem miSaveAs = new JMenuItem(new AbstractAction("Сохранить как…") {
            public void actionPerformed(ActionEvent e) { saveFileAs(); }
        });
        JMenuItem miExit = new JMenuItem(new AbstractAction("Выход") {
            public void actionPerformed(ActionEvent e) { onExit(); }
        });
        for (JMenuItem item : new JMenuItem[]{miNew, miOpen, miOpenTests, miSave, miSaveAs, miExit}) {
            item.setFont(menuFont);
        }
        file.add(miNew); file.add(miOpen); file.add(miOpenTests); file.add(miSave); file.add(miSaveAs); file.addSeparator(); file.add(miExit);
        bar.add(file);

        JMenu run = new JMenu("Пуск"); run.setFont(menuFont);
        JMenuItem miParse = new JMenuItem(new AbstractAction("Разобрать (F5)") {
            public void actionPerformed(ActionEvent e) { runParse(); }
        });
        miParse.setFont(menuFont);
        run.add(miParse);
        bar.add(run);

        JMenu view = new JMenu("Вид"); view.setFont(menuFont);
        JMenuItem miBigger = new JMenuItem(new AbstractAction("Крупнее шрифт") {
            public void actionPerformed(ActionEvent e) { changeFont(+1); }
        });
        JMenuItem miSmaller = new JMenuItem(new AbstractAction("Мельче шрифт") {
            public void actionPerformed(ActionEvent e) { changeFont(-1); }
        });
        miBigger.setFont(menuFont); miSmaller.setFont(menuFont);
        view.add(miBigger); view.add(miSmaller);
        bar.add(view);

        JMenu text = new JMenu("Текст"); text.setFont(menuFont);
        JMenuItem miReport = new JMenuItem(new AbstractAction("ТЕКСТ (отчёт)") {
            public void actionPerformed(ActionEvent e) { showReport(); }
        });
        miReport.setFont(menuFont);
        text.add(miReport);
        bar.add(text);

        JMenu help = new JMenu("Справка"); help.setFont(menuFont);
        JMenuItem miHelp = new JMenuItem(new AbstractAction("Справка / Примеры") {
            public void actionPerformed(ActionEvent e) { showHelp(); }
        });
        miHelp.setFont(menuFont);
        help.add(miHelp);
        bar.add(help);

        return bar;
    }

    private void bindEvents() {
// Extra bindings for numpad +/-
        editor.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_ADD, Toolkit.getDefaultToolkit().getMenuShortcutKeyMaskEx()), "fontPlus");
        editor.getActionMap().put("fontPlus", new AbstractAction() { public void actionPerformed(ActionEvent e) { changeFont(+1); } });
        editor.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_SUBTRACT, Toolkit.getDefaultToolkit().getMenuShortcutKeyMaskEx()), "fontMinus");
        editor.getActionMap().put("fontMinus", new AbstractAction() { public void actionPerformed(ActionEvent e) { changeFont(-1); } });
    }

    private void changeFont(int delta) {
        fontSize = Math.max(8, Math.min(40, fontSize + delta));
        editor.setFont(editor.getFont().deriveFont((float) fontSize));
        output.setFont(output.getFont().deriveFont((float) fontSize));
    }

    private void newFile() {
        if (!confirmDiscard()) return;
        editor.setText("");
        currentFile = null;
        dirty = false;
        setTitle(APP_TITLE + " — Новый файл");
    }

    private void openFile() {
        if (!confirmDiscard()) return;
        JFileChooser ch = new JFileChooser();
        ch.setFileFilter(new FileNameExtensionFilter("Text files", "txt"));
        if (ch.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = ch.getSelectedFile();
            editor.setText(TextFiles.readString(f.toPath()));
            currentFile = f;
            dirty = false;
            setTitle(APP_TITLE + " — Открыт: " + f.getName());
        }
    }

    private void openTests() {
        Path p = testsDir.resolve("tests_input.txt");
        if (Files.exists(p)) {
            if (!confirmDiscard()) return;
            editor.setText(TextFiles.readString(p));
            currentFile = p.toFile();
            dirty = false;
            setTitle(APP_TITLE + " — Открыты тесты: " + p);
        } else {
            JOptionPane.showMessageDialog(this, "Файл tests/tests_input.txt не найден.");
        }
    }

    private void saveFile() {
        if (currentFile == null) { saveFileAs(); return; }
        TextFiles.writeString(currentFile.toPath(), editor.getText());
        dirty = false;
    }

    private void saveFileAs() {
        JFileChooser ch = new JFileChooser();
        ch.setFileFilter(new FileNameExtensionFilter("Text files", "txt"));
        if (ch.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            currentFile = ch.getSelectedFile();
            saveFile();
        }
    }

    private boolean confirmDiscard() {
        if (!dirty) return true;
        int r = JOptionPane.showConfirmDialog(this, "Сохранить изменения?", APP_TITLE, JOptionPane.YES_NO_CANCEL_OPTION);
        if (r == JOptionPane.CANCEL_OPTION) return false;
        if (r == JOptionPane.YES_OPTION) saveFile();
        return true;
    }

    private void onExit() {
// autosave dirty buffer
        try {
            Files.createDirectories(autosaveDir);
            if (dirty) TextFiles.writeString(autosavePath, editor.getText());
        } catch (Exception ignored) {}
        if (!confirmDiscard()) return;
        dispose();
    }

    private void tryRestoreAutosaveOrLoadTests() {
        try {
            if (Files.exists(autosavePath)) {
                int r = JOptionPane.showConfirmDialog(this, "Обнаружена несохранённая сессия. Восстановить?", APP_TITLE, JOptionPane.YES_NO_OPTION);
                if (r == JOptionPane.YES_OPTION) {
                    editor.setText(TextFiles.readString(autosavePath));
                    setTitle(APP_TITLE + " — Восстановленная сессия");
                    dirty = true;
                    return;
                }
            }
        } catch (Exception ignored) {}
// preload tests if available
        Path p = testsDir.resolve("tests_input.txt");
        if (Files.exists(p)) {
            editor.setText(TextFiles.readString(p));
            currentFile = p.toFile();
            setTitle(APP_TITLE + " — Открыты тесты: " + p);
            dirty = false;
        } else {
// sample
            editor.setText("INPUT \"Введите X:\"; X\nINPUT A, B, C%\nINPT X\nINPUT \"X\" X\n");
        }
    }

    private void runParse() {
        output.setText("");
        String src = editor.getText();
        Lexer lx = new Lexer(src);
        var tokens = lx.lex();
// lexical errors first
        for (Token t : tokens) {
            if (t.kind == TokenKind.ERROR) {
                output.append(String.format("строка %d, позиция %d: %s\n", t.line, t.col, t.lexeme));
            }
        }
        Parser p = new Parser(tokens);
        var errors = p.parseProgram();
        if (errors.isEmpty() && tokens.stream().noneMatch(t -> t.kind == TokenKind.ERROR)) {
            output.append("OK: синтаксических ошибок не найдено\n");
        } else {
            for (ParseError e : errors) output.append(e.toString() + "\n");
        }
    }

    private void showHelp() {
        String text = TextFiles.readString(assetsDir.resolve("help.txt"),
                "Справка не найдена (assets/help.txt)\n\n" +
                        "Синтаксис: INPUT [\"подсказка\"] (','|';') идентификаторы\n" +
                        "Идентификатор: буква, затем буквы/цифры, опционально '$' или '%'.\n");
        JOptionPane.showMessageDialog(this, new JScrollPane(new JTextArea(text)), "Справка / Примеры", JOptionPane.INFORMATION_MESSAGE);
    }

    private void showReport() {
        String text = TextFiles.readString(assetsDir.resolve("report.txt"), defaultReport());
        JTextArea ta = new JTextArea(text); ta.setLineWrap(true); ta.setWrapStyleWord(true);
        ta.setFont(new Font("Serif", Font.PLAIN, 14));
        JOptionPane.showMessageDialog(this, new JScrollPane(ta), "Отчёт — ТЕКСТ", JOptionPane.PLAIN_MESSAGE);
    }


    private String defaultReport() {
        return "" +
                "Титульный лист\nBASIC INPUT Parser (практическая работа)\n\n" +
                "Постановка задачи\nЦель: разработать парсер оператора INPUT языка BASIC, включая грамматику, выбор метода анализа, реализацию с GUI и тесты.\n\n" +
                "Язык и грамматика\nТокены: INPUT, STRING, ID, ',', ';', EOL, EOF\n" +
                "Грамматика (стартовый символ Z):\n Z → INPUT H VarList EOL\n H → STRING ',' | STRING ';' | ε\n VarList → Var T\n T → ',' Var T | ε\n Var → ID\n\n" +
                "Классификация: КС-грамматика (Тип-2 по Хомскому).\n\n" +
                "FIRST/FOLLOW (доказательство LL(1))\nFIRST(Z)={INPUT}\nFIRST(H)={STRING,ε}\nFIRST(VarList)={ID}\nFIRST(T)={',',ε}\nFIRST(Var)={ID}\nFOLLOW(Z)={$}\nFOLLOW(H)={ID}\nFOLLOW(VarList)={EOL}\nFOLLOW(T)={EOL}\nFOLLOW(Var)={',',EOL}\nВывод: для H нужен lookahead на один символ (','|';'), грамматика LL(1).\n\n" +
                "Метод анализа\nНисходящий LL(1), рекурсивный спуск.\n\n" +
                "Нейтрализация ошибок\nПаник-режим: пропускаем до ID/EOL/EOF, выдаём сообщение с позицией.\n";
    }
}