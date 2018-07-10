%{
// Dummy parser for scanner project.
//3456789112345678921234567893123456789412345678951234567896123456789712
#include <cassert>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%destructor { destroy ($$); } <>
%printer { astree::dump (yyoutput, $$); } <>

%initial-action {
    parser::root = new astree (ROOT, {0, 0, 0}, "<<ROOT>>");
}


%token  ROOT

%token TOK_VOID TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_NEWSTRING TOK_TYPEID
%token TOK_ORD TOK_CHR TOK_ROOT TOK_FIELD

%token TOK_PARAM TOK_PARAMLIST TOK_FUNCTION TOK_DECLID
%token TOK_RETURNVOID TOK_VARDECL TOK_INDEX TOK_PS

%right TOK_IF TOK_ELSE
%right '='
%left TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left '+' '-'
%left '*' '/' '%'
%right '^'
%right TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR TOK_NEW
%left '[' '.' '('
%nonassoc TOK_PS

%start start

%%

/*program : program token | ;
token   : '(' | ')' | '[' | ']' | '{' | '}' | ';' | ',' | '.'
        | '=' | '+' | '-' | '*' | '/' | '%' | '!'
        | TOK_VOID | TOK_CHAR | TOK_INT | TOK_STRING
        | TOK_IF | TOK_ELSE | TOK_WHILE | TOK_RETURN | TOK_STRUCT
        | TOK_NULL | TOK_NEW | TOK_ARRAY
        | TOK_EQ | TOK_NE | TOK_LT | TOK_LE | TOK_GT | TOK_GE
        | TOK_IDENT | TOK_INTCON | TOK_CHARCON | TOK_STRINGCON
        | TOK_ORD | TOK_CHR | TOK_ROOT
        ;
*/
start: program {$$ = $1 = nullptr; }
     ;
program: program structdef  {$$ = $1->adopt ($2); }
       | program function   {$$ = $1->adopt ($2); }
       | program statement  {$$ = $1->adopt ($2); }
       | program error      {$$ = $1; }
       |                    {$$ = parser::root; }
       ;
structdef: fielddecls '}' {destroy ($2); $$ = $1; }
         ;
fielddecls: fielddecls fielddecl ';'    {destroy ($3);
                                         $$ = $1->adopt($2); }
          | TOK_STRUCT TOK_IDENT '{'    {destroy($3);
                                         $2->sym($2, TOK_TYPEID);
                                         $$ = $1->adopt($2); }
          ;
fielddecl: basetype TOK_ARRAY TOK_IDENT {$3->sym($3, TOK_FIELD);
                                         $$ = $2->adopt($1, $3); }
         | basetype TOK_IDENT           {$2->sym($2, TOK_FIELD);
                                         $$ = $1->adopt($2); }
         ;
basetype: TOK_VOID      {$$ = $1; }
        | TOK_INT       {$$ = $1; }
        | TOK_STRING    {$$ = $1; }
        | TOK_TYPEID    {$$ = $1; }
        | TOK_CHAR      {$$ = $1; }
        | TOK_IDENT     {$$ = $1; }
        ;
function: identdecl paramlist ')' block     {destroy($3);
                                    astree* func = new astree
                                    (TOK_FUNCTION, $$->lloc, "");
                      $$ = func->adopt($1)->adopt($2)->adopt($4); }
paramlist: paramlist ',' identdecl          {destroy($2);
                                             $$ = $1->adopt($3); }
         | '(' identdecl     {$$ = $1->adopt_sym($2, TOK_PARAM); }
         | '('               {$$ = $1->sym($1,TOK_PARAM); }
         ;
identdecl: basetype TOK_ARRAY TOK_IDENT   {$3->sym($3, TOK_DECLID);
                                           $$ = $2->adopt($1, $3); }
         | basetype TOK_IDENT             {$2->sym($2, TOK_DECLID);
                                           $$ = $1->adopt($2); }
         ;
block: statements '}'       {destroy($2);
                             $$ = $1->sym($1, TOK_BLOCK); }
     | ';'                  {$$ = $1->sym($1, TOK_BLOCK); }
     ;
statements: statements statement {$$ = $1->adopt($2); }
          | '{'                  {$$ = $1; }
          ;
statement: block        {$$ = $1; }
         | vardecl      {$$ = $1; }
         | while        {$$ = $1; }
         | ifelse       {$$ = $1; }
         | return       {$$ = $1; }
         | expr';'      {destroy($2); $$ = $1; }
         ;
vardecl: identdecl '=' expr';'  {destroy($4);
                    $$ = $2->adopt_sym($1, TOK_VARDECL)->adopt($3); }
       ;
while: TOK_WHILE '(' expr ')' statement {destroy($2, $4);
                                         $$ = $1->adopt($3, $5); }
     ;
ifelse: TOK_IF '(' expr ')' statement TOK_ELSE statement
                                        {destroy($2, $4);
                                         destroy($6);
                            $$ = $1->adopt_sym($3, TOK_IFELSE)
                                   ->adopt($5)->adopt($7); }
      | TOK_IF '(' expr ')' statement %prec TOK_IF
                                        {destroy($2, $4);
                                         $$ = $1->adopt($3, $5); }
      ;
return: TOK_RETURN expr';'  {destroy($3);
                             $$ = $1->adopt($2); }
      |TOK_RETURN ';'       {destroy($2);
                             $$ = $1->sym($1, TOK_RETURNVOID); }
      ;
expr: expr '+' expr             {$$ = $2->adopt($1, $3); }
    | expr '-' expr             {$$ = $2->adopt($1, $3); }
    | expr '*' expr             {$$ = $2->adopt($1, $3); }
    | expr '/' expr             {$$ = $2->adopt($1, $3); }
    | expr '%' expr             {$$ = $2->adopt($1, $3); }
    | expr '=' expr             {$$ = $2->adopt($1, $3); }
    | expr '^' expr             {$$ = $2->adopt($1, $3); }
    | expr TOK_EQ expr          {$$ = $2->adopt($1, $3); }
    | expr TOK_NE expr          {$$ = $2->adopt($1, $3); }
    | expr TOK_LE expr          {$$ = $2->adopt($1, $3); }
    | expr TOK_LT expr          {$$ = $2->adopt($1, $3); }
    | expr TOK_GE expr          {$$ = $2->adopt($1, $3); }
    | expr TOK_GT expr          {$$ = $2->adopt($1, $3); }
    | '+' expr %prec TOK_POS    {$$ = $1->adopt_sym($2, TOK_POS); }
    | '-' expr %prec TOK_NEG    {$$ = $1->adopt_sym($2, TOK_NEG); }
    | '!' expr                  {$$ = $1->adopt($2); }
    | TOK_ORD expr              {$$ = $1->adopt($2); }
    | TOK_CHR expr              {$$ = $1->adopt($2); }
    | allocator                 {$$ = $1; }
    | call                      {$$ = $1; }
    | '(' expr ')' %prec TOK_PS {destroy($1, $3);
                                 $$ = $2; }
    | variable                  {$$ = $1; }
    | constant                  {$$ = $1; }
    ;
allocator: TOK_NEW TOK_IDENT '('')'          {destroy($3, $4);
                                              $2->sym($2, TOK_TYPEID);
                                              $$ = $1->adopt($2); }
         | TOK_NEW TOK_STRING '(' expr ')'   {destroy($3, $5);
                                            $2->sym($2, TOK_NEWSTRING);
                                              $$ = $1->adopt($2, $4); }
         | TOK_NEW basetype '[' expr ']'     {destroy($3, $5);
                                              $2->sym($2, TOK_ARRAY);
                                              $$ = $1->adopt($2, $4); }
         ;
call: exprs ')'                 {destroy($2);
                                 $$ = $1; }
    ;
exprs: exprs ',' expr           {destroy($2);
                                 $$ = $1->adopt($3); }
     | TOK_IDENT '(' expr
                        {$$ = $2->adopt_sym($1, TOK_CALL)->adopt($3); }
     | TOK_IDENT '('            {$$ = $2->adopt_sym($1, TOK_CALL); }
     ;
variable: TOK_IDENT             {$$ = $1; }
        | expr '[' expr ']'     {destroy($4);
                        $$ = $2->adopt_sym($1, TOK_INDEX)->adopt($3); }
        | expr '.' expr
                        {$$ = $2->adopt_sym($1, TOK_FIELD)->adopt($3); }
        ;
constant: TOK_INTCON            {$$ = $1; }
        | TOK_CHARCON           {$$ = $1; }
        | TOK_STRINGCON         {$$ = $1; }
        | TOK_NULL              {$$ = $1; }
        ;
%%


const char *parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

/*
static void* yycalloc (size_t size) {
   void* result = calloc (1, size);
   assert (result != nullptr);
   return result;
}
*/
