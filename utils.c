#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include <stdlib.h>
#include "es.h"
#include "y.tab.h"

void prints(Sexp *sp)
{
  if (sp == (Sexp *) 0){
    printf("null");
  } else if (sp->type == 0){
    Atom *ap;
    ap = sp->u.atom;
    if (ap->type == 0)
      printf("%f", ap->u.num);
    else if (ap->type == 1)
      printf("%s", ap->u.sym);
  } else if (sp->type == 1){
    Pair *pp;
    pp = sp->u.pair;
    printf("(");
    prints(pp->car);
    printf(" . ");
    prints(pp->cdr);
    printf(")");
  } else if (sp->type == 2) {
    Atom *ap;
    ap = sp->u.atom;
    printf("#<subr %s> ", ap->u.sym);
  } else if (sp->type == 3) {
    printf("#<closure");
    printf(" ");
    printl((sp->u.proc)->params);
    printf(">");
  } else
    ;
}

void ptol(Pair* pp, List ls)
{
  Sexp* cdrs = pp->cdr;

  *ls++ = pp->car;

  while (cdrs != (Sexp*) 0){
    if (cdrs->type == 0){
      *ls++ = cdrs;
      break;
    } else if (cdrs->type == 1){
      pp = cdrs->u.pair;
      *ls++ = pp->car;
      cdrs = pp->cdr;
    } else {
      error("illegal pair");
    }
  }
  *ls = NULL;
  //free(pp);
}

void printl(List ls)
{
  if (*ls == NULL)
    readerror("cannot print", " list");
  Sexp* sp;
  printf("(");
  while ((sp = *ls++) != NULL){
    if (sp == (Sexp *) 0){
      printf("()");
    } else if (sp->type == 0){
      printf(" ");
      prints(sp);
      printf(" ");
    } else if (sp->type == 1){
      Pair* pp = sp->u.pair;
      if (pp->cdr == (Sexp *) 0){
	printf("(");
	prints(pp->car);
	printf(")");
      }
      else {
	List subls;
	subls = (List) emalloc(sizeof(List));
	ptol(pp, subls);
	printl(subls);
      }
    } else
      ;
  }
  printf(")");
}

void printls(Sexp* sp)
{
  List ls;
  ls = (List) emalloc(sizeof(List));
  ptol(sp->u.pair, ls);
  printl(ls);
}


void getstring(char c, char* str)
{
  char *p = str;
  do {
    *p++ = c;
  } while ((c=getchar()) != EOF && !isspace(c) && c!=')' && c!='(');
  ungetc(c, stdin);
  *p = '\0';
}

void *atof_withtest(char s[])
{
  double val, power, ans;
  int i, sign;

  for (i = 0; isspace(s[i]); i++)
    ;
  sign = (s[i] == '-') ? -1 : 1;
  if (s[i] == '+' || s[i] == '-')
    i++;
  if (s[i] == '\0')
    return NULL;
  for (val = 0.0; isdigit(s[i]); i++)
    val = 10.0 * val + (s[i] - '0');
  if (s[i] == '.')
    i++;
  else if (s[i] != '\0'){
    lineno++;
    readerror("unexpected symbol");
  }
  for (power = 1.0; isdigit(s[i]); i++){
    val = 10.0 * val + (s[i] - '0');
    power *= 10.0;
  }
  ans = sign * val / power;
  return &ans;
}

void initflags()
{
  inParen = 0;
  waitingAtom = 1;
  waitingCdr = 0;
  needLparen = 0;
  needRparen = 0;
  processingCdr = ibuf;
}

void prompt(int line)
{
  initflags();
  printf("\nes%d> ", line);
}

char *emalloc(unsigned n)
{
  char *p;

  p = (char*) malloc(n);
  if (p == 0)
    error("out of memory", (char *) 0);
  return p;
}
