package app.ui;

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;

/**
 * Поле нумерации строк для JTextArea, ставится в rowHeader JScrollPane.
 */
public class LineNumberView extends JComponent {

    private final JTextArea textArea;
    private final int padding = 8;

    public LineNumberView(JTextArea textArea) {
        this.textArea = textArea;
        setFont(textArea.getFont());

        // Перерисовываем номера при изменении текста
        textArea.getDocument().addDocumentListener(new DocumentListener() {
            @Override public void insertUpdate(DocumentEvent e) { repaint(); revalidate(); }
            @Override public void removeUpdate(DocumentEvent e) { repaint(); revalidate(); }
            @Override public void changedUpdate(DocumentEvent e) { repaint(); revalidate(); }
        });

        // И при изменении размера
        textArea.addComponentListener(new java.awt.event.ComponentAdapter() {
            @Override public void componentResized(java.awt.event.ComponentEvent e) {
                repaint();
                revalidate();
            }
        });
    }

    @Override
    public Dimension getPreferredSize() {
        int lineCount = Math.max(1, textArea.getLineCount());
        FontMetrics fm = getFontMetrics(getFont());
        int digits = String.valueOf(lineCount).length();
        int width = padding * 2 + fm.charWidth('0') * digits;
        return new Dimension(width, textArea.getHeight());
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        FontMetrics fm = getFontMetrics(getFont());
        int lineHeight = fm.getHeight();
        int ascent = fm.getAscent();

        int lineCount = textArea.getLineCount();
        int y = ascent;

        // фон
        g.setColor(new Color(240, 240, 240));
        g.fillRect(0, 0, getWidth(), getHeight());

        g.setColor(Color.GRAY);
        for (int line = 1; line <= lineCount; line++) {
            String num = String.valueOf(line);
            int strWidth = fm.stringWidth(num);
            int x = getWidth() - padding - strWidth;
            g.drawString(num, x, y);
            y += lineHeight;
        }
    }
}
