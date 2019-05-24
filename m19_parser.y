%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                   i;	/* integer value */
  double                d;    /* double value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  
  basic_type      *type;

  m19::function_declaration_node *funcDecl;
  m19::initial_section_node *initSec;
  m19::final_section_node *finSec;
  m19::block_node *block;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tPRINTLN tAND tOR tBREAK tCONTINUE tRETURN tINIT tFINAL

%nonassoc tIF
%nonassoc tELSE
%right '='
%left tOR
%left tAND
%nonassoc '~'
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc ":"
%nonassoc "["

%type <node> decl funcDef section varDecl stmt
%type <sequence> list module sections args optBlockVarDecls optStmts blockVarDecls varDecls stmts exprs params
%type <expression> expr literal
%type <lvalue> lval
%type <i> qualifier
%type <type> type
%type <funcDecl> funcDecl
%type <initSec> initialSec
%type <finSec> finalSec
%type <block> block
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

module 	: list { compiler->ast($1); }
	      ;

list : decl	     { $$ = new cdk::sequence_node(LINE, $1); }
	| list decl { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;

decl : funcDecl        { $$ = $1; }
     | funcDef         { $$ = $1; }
	| varDecl ';'  { $$ = $1;}
     ;

funcDecl : type tIDENTIFIER qualifier '(' args ')' {$$ = new m19::function_declaration_node(LINE, $3, $1, nullptr, *$2, $5);}
         | type tIDENTIFIER qualifier '(' ')'      {$$ = new m19::function_declaration_node(LINE, $3, $1, nullptr, *$2, new cdk::sequence_node(LINE));}
         | '!' tIDENTIFIER qualifier '(' args ')' {$$ = new m19::function_declaration_node(LINE, $3, *$2, $5);}
         | '!' tIDENTIFIER qualifier '(' ')'      {$$ = new m19::function_declaration_node(LINE, $3, *$2, new cdk::sequence_node(LINE));}
         | type tIDENTIFIER qualifier '(' args ')' '=' literal  {$$ = new m19::function_declaration_node(LINE, $3, $1, $8, *$2, $5);}
         | type tIDENTIFIER qualifier '(' ')' '=' literal       {$$ = new m19::function_declaration_node(LINE, $3, $1, $7, *$2, new cdk::sequence_node(LINE));}
       /*  | '!' tIDENTIFIER qualifier '(' args ')' '=' literal  {$$ = new m19::function_declaration_node(LINE, $3, new basic_type(0, basic_type::TYPE_VOID), $8, *$2, $5);} */
         | '!' tIDENTIFIER qualifier '(' ')' '=' literal       {$$ = new m19::function_declaration_node(LINE, $3, new basic_type(0, basic_type::TYPE_VOID), $7, *$2, new cdk::sequence_node(LINE));}
         ;

funcDef: funcDecl initialSec sections finalSec 
       {$$ = new m19::function_definition_node(LINE, $1, $3, $2, $4);}
        | funcDecl initialSec sections         
       {$$ = new m19::function_definition_node(LINE, $1, $3, $2, nullptr);}
        | funcDecl sections finalSec          
       {$$ = new m19::function_definition_node(LINE, $1, $2, nullptr, $3);}
        | funcDecl initialSec finalSec       
       {$$ = new m19::function_definition_node(LINE, $1, nullptr, $2, $3);}
        | funcDecl sections                 
       {$$ = new m19::function_definition_node(LINE, $1, $2, nullptr, nullptr);}
        | funcDecl initialSec                 
       {$$ = new m19::function_definition_node(LINE, $1, nullptr, $2, nullptr);}
        | funcDecl finalSec                 
       {$$ = new m19::function_definition_node(LINE, $1, nullptr, nullptr, $2);}
       ;

initialSec: tINIT block               {$$ = new m19::initial_section_node(LINE, $2);}
          ;

finalSec: tFINAL block                {$$ = new m19::final_section_node(LINE, $2);}
        ;

sections: sections section            {$$ = new cdk::sequence_node(LINE, $2, $1);}
        | section                     {$$ = new cdk::sequence_node(LINE, $1);}
        ;

section: '[' expr ']' block           {$$ = new m19::section_node(LINE, $2, $4, false);}
       | '(' expr ')' block           {$$ = new m19::section_node(LINE, $2, $4, true);}
       | '[' ']' block                {$$ = new m19::section_node(LINE, new cdk::integer_node(LINE, 1), $3, false);}
       | '(' ')' block                {$$ = new m19::section_node(LINE, new cdk::integer_node(LINE, 1), $3, true);}
       | block                        {$$ = new m19::section_node(LINE, new cdk::integer_node(LINE, 1), $1, true);}
       ;

block: '{' optBlockVarDecls optStmts '}'   {$$ = new m19::block_node(LINE, $2, $3);}
     ;

optBlockVarDecls: /*no varDecls*/             {$$ = new cdk::sequence_node(LINE);}
		 | blockVarDecls                    {$$ = $1;}
	 				;
blockVarDecls: varDecl ';'                    {$$ = new cdk::sequence_node(LINE, $1);}
	     | blockVarDecls varDecl ';'      {$$ = new cdk::sequence_node(LINE, $2, $1);}
	;

optStmts: /*no stmts*/                  {$$ = new cdk::sequence_node(LINE);}
	   | stmts                         {$$ = $1;}
	   ;

varDecls: varDecls ',' varDecl              {$$ = new cdk::sequence_node(LINE, $3, $1);}
	   | varDecl                       {$$ = new cdk::sequence_node(LINE, $1);}
	   ;

varDecl: type tIDENTIFIER qualifier                  {$$ = new m19::variable_declaration_node(LINE, $3, $1, *$2, nullptr);}
       | type tIDENTIFIER qualifier '=' expr         {$$ = new m19::variable_declaration_node(LINE, $3, $1, *$2, $5);}
       ;

qualifier:  '!'                         { $$ = 1; } //qualifier rules: 1 is public
		| '?'                         { $$ = 2; } //                 2 is extern
          |                             { $$ = 0; } //                 0 is private
          ;

type : '#'                              { $$ = new basic_type(4, basic_type::TYPE_INT); }
	| '%'                              { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
	| '$'                              { $$ = new basic_type(4, basic_type::TYPE_STRING); }
     | '<' type '>'                     { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
/*	| tINIT type '>' '>'               { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(4,basic_type::TYPE_POINTER); $$->_subtype->_subtype = $2; }
	| tINIT type tFINAL                { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(4,basic_type::TYPE_POINTER); $$->_subtype->_subtype = $2;}
	| '<' '<' type tFINAL              { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(4,basic_type::TYPE_POINTER); $$->_subtype->_subtype = $3;}*/
     ;

args : varDecl                          {$$ = new cdk::sequence_node(LINE, $1);}
     | args ',' varDecl                 {$$ = new cdk::sequence_node(LINE, $3, $1);}
     ;

stmts: stmt                             { $$ = new cdk::sequence_node(LINE, $1);}
	| stmts stmt                   { $$ = new cdk::sequence_node(LINE, $2, $1);}
	;

stmt : expr ';'                                   { $$ = new m19::evaluation_node(LINE, $1); }
 	| expr '!'                                   { $$ = new m19::print_node(LINE, $1, false); }
 	| expr tPRINTLN                              { $$ = new m19::print_node(LINE, $1, true); }
 	| tCONTINUE                              { $$ = new m19::continue_node(LINE); }
 	| tBREAK                              { $$ = new m19::break_node(LINE); }
 		 | tRETURN                              { $$ = new m19::return_node(LINE); }
     | '[' exprs ';' exprs ';' exprs ']' stmt     { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
     | '[' ';' exprs ';' exprs ']' stmt     { $$ = new m19::for_node(LINE, new cdk::sequence_node(LINE), $3, $5, $7); }
     | '[' varDecls ';' exprs ';' exprs ']' stmt  { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
     | '[' expr ']' '#' stmt                      { $$ = new m19::if_node(LINE, $2, $5); }
     | '[' expr ']' '?' stmt ':' stmt %prec tELSE            { $$ = new m19::if_else_node(LINE, $2, $5, $7); }
     | '[' expr ']' '?' stmt  %prec tIF           { $$ = new m19::if_node(LINE, $2, $5); }
		 |  block                                { $$ = $1; }
     ;

exprs: expr                             { $$ = new cdk::sequence_node(LINE, $1);}
     | exprs ',' expr                       { $$ = new cdk::sequence_node(LINE, $3, $1);}
     ;

expr : literal                         { $$ = $1; }
     | tIDENTIFIER '(' params ')'       { $$ = new m19::function_call_node(LINE, *$1, $3); }
     | tIDENTIFIER '(' ')'              { $$ = new m19::function_call_node(LINE, *$1); }
     | '@' '(' params ')'               { $$ = new m19::function_call_node(LINE, "@", $3); }
     | '@' '(' ')'                      { $$ = new m19::function_call_node(LINE, "@"); }
     | '-' expr %prec tUNARY            { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY            { $$ = new m19::identity_node(LINE, $2); }
     | '~' expr                         { $$ = new cdk::not_node(LINE, $2); }
     | expr '+' expr	                { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	                { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	                { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	                { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	                { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr	                { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	                { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr                    { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                    { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	                { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	                { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tAND expr	                { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr	                { $$ = new cdk::or_node(LINE, $1, $3); }
     | '[' expr ']'                     { $$ = new m19::alloc_node(LINE, $2); }
     | '(' expr ')'                     { $$ = $2; }
     | lval                             { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
     | lval '=' expr                    { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | '@' '=' expr                     { $$ = new cdk::assignment_node(LINE, new cdk::variable_node(LINE,new std::string("@")), $3); }
     | '@'                              { $$ = new m19::read_node(LINE); }
     | lval '?'                         { $$ = new m19::address_of_node(LINE, $1); }
     | '@' '?'                         { $$ = new m19::address_of_node(LINE, new cdk::variable_node(LINE,new std::string("@"))); }
     ;

params: expr                        { $$ = new cdk::sequence_node(LINE, $1);}
      | params ',' expr                 { $$ = new cdk::sequence_node(LINE, $3, $1);}
      ;

lval : tIDENTIFIER                      { $$ = new cdk::variable_node(LINE, $1); }
     | '(' expr ')' '[' expr ']'                { $$ = new m19::index_node(LINE, $2, $5);}
     | tIDENTIFIER '[' expr ']'                { $$ = new m19::index_node(LINE, new cdk::rvalue_node(LINE, new cdk::variable_node(LINE, $1)), $3);}
     | '@' '[' expr ']'                { $$ = new m19::index_node(LINE, new cdk::rvalue_node(LINE, new cdk::variable_node(LINE, new std::string("@"))), $3);}
     ;

literal: tINTEGER                       { $$ = new cdk::integer_node(LINE, $1); }
       | string                        { $$ = new cdk::string_node(LINE, $1); }
       | tDOUBLE                        { $$ = new cdk::double_node(LINE, $1); }
       ;

string: tSTRING                         { $$ = new std::string(*$1); delete $1;}
   | string tSTRING                  { $$ = new std::string(*$1 + *$2); delete $1; delete $2;}
   ;

%%
