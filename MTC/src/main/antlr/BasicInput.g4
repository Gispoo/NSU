grammar BasicInput;

// ===== Parser rules =====

program     : (line)* EOF ;
line        : inputStmt EOL ;
inputStmt   : INPUT hint? varList ;
hint        : STRING (COMMA | SEMI) ;
varList     : ID (COMMA ID)* ;

// ===== Lexer rules =====

// Регистр учитываем: только INPUT (верхний регистр) — ключевое слово
INPUT   : 'INPUT' ;

COMMA   : ',' ;
SEMI    : ';' ;

// Конец строки (важно для "одна строка = один оператор")
EOL     : '\r'? '\n' ;

// Идентификатор: буква, затем буквы/цифры (без $,%,&)
ID      : [A-Za-z] [A-Za-z0-9]* ;

// Строка в кавычках (без экранирования)
STRING  : '"' (~["\r\n])* '"' ;

// Комментарии: REM ... (только REM в верхнем регистре), и апостроф
REM_COMMENT : 'REM' ~[\r\n]* -> skip ;
APOS_COMMENT: '\''  ~[\r\n]* -> skip ;

// Пробелы/табуляции игнорируем
WS      : [ \t]+ -> skip ;
