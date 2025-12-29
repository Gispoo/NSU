package app.core.antlr;

// Generated from BasicInput.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link BasicInputParser}.
 */
public interface BasicInputListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link BasicInputParser#program}.
	 * @param ctx the parse tree
	 */
	void enterProgram(BasicInputParser.ProgramContext ctx);
	/**
	 * Exit a parse tree produced by {@link BasicInputParser#program}.
	 * @param ctx the parse tree
	 */
	void exitProgram(BasicInputParser.ProgramContext ctx);
	/**
	 * Enter a parse tree produced by {@link BasicInputParser#line}.
	 * @param ctx the parse tree
	 */
	void enterLine(BasicInputParser.LineContext ctx);
	/**
	 * Exit a parse tree produced by {@link BasicInputParser#line}.
	 * @param ctx the parse tree
	 */
	void exitLine(BasicInputParser.LineContext ctx);
	/**
	 * Enter a parse tree produced by {@link BasicInputParser#inputStmt}.
	 * @param ctx the parse tree
	 */
	void enterInputStmt(BasicInputParser.InputStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link BasicInputParser#inputStmt}.
	 * @param ctx the parse tree
	 */
	void exitInputStmt(BasicInputParser.InputStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link BasicInputParser#hint}.
	 * @param ctx the parse tree
	 */
	void enterHint(BasicInputParser.HintContext ctx);
	/**
	 * Exit a parse tree produced by {@link BasicInputParser#hint}.
	 * @param ctx the parse tree
	 */
	void exitHint(BasicInputParser.HintContext ctx);
	/**
	 * Enter a parse tree produced by {@link BasicInputParser#varList}.
	 * @param ctx the parse tree
	 */
	void enterVarList(BasicInputParser.VarListContext ctx);
	/**
	 * Exit a parse tree produced by {@link BasicInputParser#varList}.
	 * @param ctx the parse tree
	 */
	void exitVarList(BasicInputParser.VarListContext ctx);
}