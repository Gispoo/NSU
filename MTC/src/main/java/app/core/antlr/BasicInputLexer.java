package app.core.antlr;

// Generated from BasicInput.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class BasicInputLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.13.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		INPUT=1, COMMA=2, SEMI=3, EOL=4, ID=5, STRING=6, REM_COMMENT=7, APOS_COMMENT=8, 
		WS=9;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"INPUT", "COMMA", "SEMI", "EOL", "ID", "STRING", "REM_COMMENT", "APOS_COMMENT", 
			"WS"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'INPUT'", "','", "';'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "INPUT", "COMMA", "SEMI", "EOL", "ID", "STRING", "REM_COMMENT", 
			"APOS_COMMENT", "WS"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public BasicInputLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "BasicInput.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\u0004\u0000\tN\u0006\uffff\uffff\u0002\u0000\u0007\u0000\u0002\u0001"+
		"\u0007\u0001\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004"+
		"\u0007\u0004\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002\u0007"+
		"\u0007\u0007\u0002\b\u0007\b\u0001\u0000\u0001\u0000\u0001\u0000\u0001"+
		"\u0000\u0001\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0002\u0001"+
		"\u0002\u0001\u0003\u0003\u0003\u001f\b\u0003\u0001\u0003\u0001\u0003\u0001"+
		"\u0004\u0001\u0004\u0005\u0004%\b\u0004\n\u0004\f\u0004(\t\u0004\u0001"+
		"\u0005\u0001\u0005\u0005\u0005,\b\u0005\n\u0005\f\u0005/\t\u0005\u0001"+
		"\u0005\u0001\u0005\u0001\u0006\u0001\u0006\u0001\u0006\u0001\u0006\u0001"+
		"\u0006\u0005\u00068\b\u0006\n\u0006\f\u0006;\t\u0006\u0001\u0006\u0001"+
		"\u0006\u0001\u0007\u0001\u0007\u0005\u0007A\b\u0007\n\u0007\f\u0007D\t"+
		"\u0007\u0001\u0007\u0001\u0007\u0001\b\u0004\bI\b\b\u000b\b\f\bJ\u0001"+
		"\b\u0001\b\u0000\u0000\t\u0001\u0001\u0003\u0002\u0005\u0003\u0007\u0004"+
		"\t\u0005\u000b\u0006\r\u0007\u000f\b\u0011\t\u0001\u0000\u0005\u0002\u0000"+
		"AZaz\u0003\u000009AZaz\u0003\u0000\n\n\r\r\"\"\u0002\u0000\n\n\r\r\u0002"+
		"\u0000\t\t  S\u0000\u0001\u0001\u0000\u0000\u0000\u0000\u0003\u0001\u0000"+
		"\u0000\u0000\u0000\u0005\u0001\u0000\u0000\u0000\u0000\u0007\u0001\u0000"+
		"\u0000\u0000\u0000\t\u0001\u0000\u0000\u0000\u0000\u000b\u0001\u0000\u0000"+
		"\u0000\u0000\r\u0001\u0000\u0000\u0000\u0000\u000f\u0001\u0000\u0000\u0000"+
		"\u0000\u0011\u0001\u0000\u0000\u0000\u0001\u0013\u0001\u0000\u0000\u0000"+
		"\u0003\u0019\u0001\u0000\u0000\u0000\u0005\u001b\u0001\u0000\u0000\u0000"+
		"\u0007\u001e\u0001\u0000\u0000\u0000\t\"\u0001\u0000\u0000\u0000\u000b"+
		")\u0001\u0000\u0000\u0000\r2\u0001\u0000\u0000\u0000\u000f>\u0001\u0000"+
		"\u0000\u0000\u0011H\u0001\u0000\u0000\u0000\u0013\u0014\u0005I\u0000\u0000"+
		"\u0014\u0015\u0005N\u0000\u0000\u0015\u0016\u0005P\u0000\u0000\u0016\u0017"+
		"\u0005U\u0000\u0000\u0017\u0018\u0005T\u0000\u0000\u0018\u0002\u0001\u0000"+
		"\u0000\u0000\u0019\u001a\u0005,\u0000\u0000\u001a\u0004\u0001\u0000\u0000"+
		"\u0000\u001b\u001c\u0005;\u0000\u0000\u001c\u0006\u0001\u0000\u0000\u0000"+
		"\u001d\u001f\u0005\r\u0000\u0000\u001e\u001d\u0001\u0000\u0000\u0000\u001e"+
		"\u001f\u0001\u0000\u0000\u0000\u001f \u0001\u0000\u0000\u0000 !\u0005"+
		"\n\u0000\u0000!\b\u0001\u0000\u0000\u0000\"&\u0007\u0000\u0000\u0000#"+
		"%\u0007\u0001\u0000\u0000$#\u0001\u0000\u0000\u0000%(\u0001\u0000\u0000"+
		"\u0000&$\u0001\u0000\u0000\u0000&\'\u0001\u0000\u0000\u0000\'\n\u0001"+
		"\u0000\u0000\u0000(&\u0001\u0000\u0000\u0000)-\u0005\"\u0000\u0000*,\b"+
		"\u0002\u0000\u0000+*\u0001\u0000\u0000\u0000,/\u0001\u0000\u0000\u0000"+
		"-+\u0001\u0000\u0000\u0000-.\u0001\u0000\u0000\u0000.0\u0001\u0000\u0000"+
		"\u0000/-\u0001\u0000\u0000\u000001\u0005\"\u0000\u00001\f\u0001\u0000"+
		"\u0000\u000023\u0005R\u0000\u000034\u0005E\u0000\u000045\u0005M\u0000"+
		"\u000059\u0001\u0000\u0000\u000068\b\u0003\u0000\u000076\u0001\u0000\u0000"+
		"\u00008;\u0001\u0000\u0000\u000097\u0001\u0000\u0000\u00009:\u0001\u0000"+
		"\u0000\u0000:<\u0001\u0000\u0000\u0000;9\u0001\u0000\u0000\u0000<=\u0006"+
		"\u0006\u0000\u0000=\u000e\u0001\u0000\u0000\u0000>B\u0005\'\u0000\u0000"+
		"?A\b\u0003\u0000\u0000@?\u0001\u0000\u0000\u0000AD\u0001\u0000\u0000\u0000"+
		"B@\u0001\u0000\u0000\u0000BC\u0001\u0000\u0000\u0000CE\u0001\u0000\u0000"+
		"\u0000DB\u0001\u0000\u0000\u0000EF\u0006\u0007\u0000\u0000F\u0010\u0001"+
		"\u0000\u0000\u0000GI\u0007\u0004\u0000\u0000HG\u0001\u0000\u0000\u0000"+
		"IJ\u0001\u0000\u0000\u0000JH\u0001\u0000\u0000\u0000JK\u0001\u0000\u0000"+
		"\u0000KL\u0001\u0000\u0000\u0000LM\u0006\b\u0000\u0000M\u0012\u0001\u0000"+
		"\u0000\u0000\u0007\u0000\u001e&-9BJ\u0001\u0006\u0000\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}