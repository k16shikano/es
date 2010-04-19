%{
  #include "es.h"
  #define YYSTYPE Sexp*
  %}
%token ATOM
%token DOT
%token LPAREN
%token RPAREN
%%
list:             { prompt(lineno); }
| list '\n'
| list sexp '\n'  { prints(eval($2, global_env)); prompt(lineno); }
;

sexp: ATOM
| LPAREN sexp DOT sexp RPAREN  { $$ = mk_oblist($2, $4); }
;

%%
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
jmp_buf begin;
char *progname;

main(char argc, char *argv[])
{
  lineno = 0;
  progname = argv[0];

  symbol_table = (Symbols*)emalloc(sizeof(Symbols));
  symbol_table->sym = NULL;
  symbol_table->next = NULL;

  initenv();
 

  setjmp(begin);
  yyparse();
}

yylex()
{
  char c;

  if (needLparen > 0){
    inParen++; needLparen--; needRparen++;
    *processingCdr++ = waitingCdr; 
    waitingAtom = 1; waitingCdr = 0;
    return LPAREN;
  }
  while (isspace((c=getchar()))){
    if (c == '\n') lineno++;
    if (inParen)
      ;
    else
      if (c == '\n') return c;
  }
  if (c == EOF)
    return 0;
  if (c == '('){
    if (waitingAtom){
      inParen++;
      *processingCdr++ = waitingCdr; 
      waitingCdr = 0; waitingLNull = 1;
      return LPAREN;
    }
    ungetc(c, stdin);
    needLparen++;
    waitingAtom = 1; waitingCdr = 1;
    return DOT;
  }
  if (c == ')'){
    if (waitingLNull){
      ungetc(c, stdin);
      waitingAtom = 0; waitingCdr = 1; waitingLNull = 0;
      yylval = mk_nullcar();
      return ATOM;
    }
    if (waitingCdr){
      ungetc(c, stdin);
      if (waitingAtom){
	waitingAtom = 0; waitingCdr = 0;
	yylval = (Sexp*) 0;
	return ATOM;
      }
      waitingAtom = 1; waitingCdr = 1;
      return DOT;
    }
    if (needRparen > 0 && *(processingCdr-1)){
      ungetc(c, stdin);
      inParen--; needRparen--;
      waitingAtom = 0; waitingCdr = !*--processingCdr;
      return RPAREN;
    }
    inParen--;
    waitingAtom = 0; waitingCdr = !*--processingCdr;
    return RPAREN;
  }
  if (c == '.'){
    char cnext;
    cnext = getchar();
    if (isspace(cnext)){
      waitingAtom = 1; waitingCdr = 1;
      return DOT;
    }
    if (cnext == '('){
      ungetc(cnext, stdin);
      waitingAtom = 1; waitingCdr = 1;
      return DOT;
    }
    ungetc(cnext, stdin);
  }
  if (isdigit(c) || c == '.' || c == '-'){
    if (waitingAtom){
      char sbuf[100];
      double* numaddr;
      getstring(c, sbuf);
      waitingAtom = 0; waitingCdr = !waitingCdr; waitingLNull = 0;
      if ((numaddr = atof_withtest(sbuf)) != NULL){
	yylval = mk_num(*numaddr);
	return ATOM;
      }
      yylval = mk_symbol(sbuf);
      return ATOM;
    }
    ungetc(c, stdin);
    needLparen++;
    waitingAtom = 1; waitingCdr = 1;
    return DOT;
  }
  if (!isspace(c)){
    if (waitingAtom){
      char sbuf[100];
      getstring(c, sbuf);
      waitingAtom = 0; waitingCdr = !waitingCdr; waitingLNull = 0;
      yylval = mk_symbol(sbuf);
      return ATOM;
    }
    ungetc(c, stdin);
    needLparen++;
    waitingAtom = 1; waitingCdr = 1;
    return DOT;
  }
  return c;
}

yyerror(char* s)
{
  warning(s, (char*) 0);
  longjmp(begin, 0);
}

readerror(char* s, char* t)
{
  warning(s, t);
  longjmp(begin, 0);
}

warning(char *s, char* t)
{
  fprintf(stderr, "%s; %s", progname, s);
  if (t) 
    fprintf(stderr, " %s", t);
  fprintf(stderr, " at es%d\n", lineno-1);
}
