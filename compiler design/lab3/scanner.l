%{
    #include "auxlib.h"
    #include "lyutils.h"
    #include "astree.h"
    
    #define YY_USER_ACTION  { lexer::advance(); }

    
%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
INTEGER         ({DIGIT}+)
MANTISSA        ({INTEGER}\.?{DIGIT}*|\.{INTEGER})
EXPONENT        ([Ee][+-]?{INTEGER})
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"if"            { return yylval_token (TOK_IF); }
"else"          { return yylval_token (TOK_ELSE); }
"while"         { return yylval_token (TOK_WHILE); }
"return"        { return yylval_token (TOK_RETURN); }
"struct"        { return yylval_token (TOK_STRUCT); }
"null"          { return yylval_token (TOK_NULL); }
"new"           { return yylval_token (TOK_NEW); }
"void"          { return yylval_token (TOK_VOID); }
"char"          { return yylval_token (TOK_CHAR); }
"int"           { return yylval_token (TOK_INT); }
"string"        { return yylval_token (TOK_STRING); }
"ord"           { return yylval_token (TOK_ORD); }
"chr"           { return yylval_token (TOK_CHR); }
"root"          { return yylval_token (TOK_ROOT); }

{IDENT}         { return yylval_token (TOK_IDENT); }
{INTEGER}       { return yylval_token (TOK_INTCON); }
{NUMBER}        { return yylval_token (TOK_INTCON); }
{CHAR}          { return yylval_token (TOK_CHARCON); }
{STRING}        { return yylval_token (TOK_STRINGCON); }

"[]"            { return yylval_token (TOK_ARRAY); }
"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }
"<="            { return yylval_token (TOK_LE); }
"<"             { return yylval_token (TOK_LT); }
">="            { return yylval_token (TOK_GE); }
">"             { return yylval_token (TOK_GT); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"^"             { return yylval_token ('^'); }
"!"             { return yylval_token ('!'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
";"             { return yylval_token (';'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }


.               { lexer::badchar (*yytext); }

%%
