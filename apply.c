#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include "es.h"
#include "y.tab.h"

Sexp* apply_primitives(Sexp *proc, List args)
{
  Atom* ap = proc->u.atom;
  char* procstr = ap->u.sym;

  if (!strcmp(procstr, "car"))
    return car(args);
  else if (!strcmp(procstr, "cdr"))
    return cdr(args);
  else if (!strcmp(procstr, "cons"))
    return cons(args);

  else if (!strcmp(procstr, "+"))
    return calc(args, ADD);
  else if (!strcmp(procstr, "-"))
    return calc(args, SUB);
  else if (!strcmp(procstr, "*"))
    return calc(args, MUL);
  else if (!strcmp(procstr, "/"))
    return calc(args, DIV);

  else if (!strcmp(procstr, "<"))
    return order(args, LT);
  else if (!strcmp(procstr, ">"))
    return order(args, GT);
  else if (!strcmp(procstr, "="))
    return order(args, EQ);
  else if (!strcmp(procstr, "<="))
    return order(args, LTEQ);
  else if (!strcmp(procstr, ">="))
    return order(args, GTEQ);

  else 
    readerror("unknown primitive procedure", procstr);
}

Sexp* apply(Sexp* sp, List args)
{
  if (sp->type == 2)
    return apply_primitives(sp, args);
  else if(sp->type == 3){
    Proc* procp = sp->u.proc;
    Sexp* firstbody = *(procp->body);

    Env* newenv = (Env*)emalloc(sizeof(Env));
    newenv = extend_env(procp->params, args, procp->env);

    Sexp* applyed = eval(firstbody, newenv);
    
    return applyed;
  } else 
    readerror("unknown procedure", (sp->u.atom)->u.sym);
}

Sexp* car(List args)
{
  if (*(args+1) == NULL){
    Sexp* head = *args;
    if (head->type == 0)
      return head;
    if (head->type == 1){
      Pair* pp;
      pp = head->u.pair;
      return pp->car;
    }
  }
  readerror("Invalid argument to car. ");
}

Sexp* cdr(List args)
{  
  if (*(args+1) == NULL){
    Sexp* head = *args;
    if (head->type == 1){
      Pair* pp;
      pp = head->u.pair;
      return pp->cdr;
    }
  }
  readerror("Invalid argument to cdr. ");
}

Sexp* cons(List args)
{
  if (*(args+2) == NULL)
    return mk_oblist(*args, *(args+1));
  else
    readerror("Invalid argument to cons. ");
}

Sexp* calc(List args, Calc sym)
{
  if (*(args+2) == NULL){
    Sexp *sp1 = *args, *sp2 = *(args+1);
    Atom *ap1 = sp1->u.atom, *ap2 = sp2->u.atom;
    if (ap1->type != 0 || ap2->type != 0)
      readerror("arguments to calc must be number", "");
    double x1 = ap1->u.num, x2 = ap2->u.num, ans;

    switch (sym) {
    case ADD: ans = x1 + x2; break;
    case SUB: ans = x1 - x2; break;
    case MUL: ans = x1 * x2; break;
    case DIV: ans = x1 / x2; break;
    }
    return mk_num(ans);
  }
  else
    readerror("invalid argument to calc ", sym);
}

Sexp* order(List args, Order sym)
{
  if (*(args+2) == NULL){
    Sexp *sp1 = *args, *sp2 = *(args+1);
    Atom *ap1 = sp1->u.atom, *ap2 = sp2->u.atom;
    if (ap1->type != 0 || ap2->type != 0)
      readerror("arguments to order must be number", "");
    double x1 = ap1->u.num, x2 = ap2->u.num;
    char* torf;
  
    switch (sym) {
    case LT: torf = (x1 < x2) ? "#t" : "#f"; break;
    case GT: torf = (x1 > x2) ? "#t" : "#f"; break;
    case EQ: torf = (x1 == x2) ? "#t" : "#f"; break;
    case LTEQ: torf = (x1 <= x2) ? "#t" : "#f"; break;
    case GTEQ: torf = (x1 >= x2) ? "#t" : "#f"; break;
    }
    return eval(mk_symbol(torf), global_env);
  }
  else
    readerror("invalid argument to order ", sym);
}
