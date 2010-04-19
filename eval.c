#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include "es.h"
#include "y.tab.h"

Sexp* eval(Sexp* sp, Env* env)
{  
  if (sp->type == 0){       /* self evaluating or variable */
    Atom* ap = sp->u.atom;
    if (ap->type == 0)
      return sp;
    else if (ap->type == 1){
      Atom** interned = (Atom**)emalloc(sizeof(Atom*));
      if (lookup_symbol(ap->u.sym, symbol_table, interned) == 1){
	Sexp* found = lookup_variable_value(mk_S(*interned, 0), env);
	return found;
      }
      else
	return lookup_variable_value(sp, NULL);
    } else {
      readerror("illegal object -- eval");
    }
  } else if (sp->type == 1){
    Pair* pp = sp->u.pair;
    List argls, evaledls;
    argls = (List) emalloc(sizeof(List));
    extract_args(sp->u.pair, argls);
    if (caris(sp, "define")){
      return eval_definition(argls, env);
    }
    else if (caris(sp, "eq?")){
      return eval_eq(argls, env);
    }
    else if (caris(sp, "quote")){
      return text_of_quotation(argls);
    }
    else if (caris(sp, "lambda")){
      return make_closure(argls, env);
    }
    else if (caris(sp, "if")){
      return eval_if(argls, env);
    }
    else {
      evaledls = (List) emalloc(sizeof(List));
      evlis(argls, evaledls, env);
      return apply(eval(pp->car, env), evaledls);
    }
  } else {
    readerror("eval error");
  }
}

void extract_args(Pair* pp, List ls)
{
  Sexp* sp = pp->cdr;
  if (sp == (Sexp *) 0)
    *ls = NULL;
  else
    ptol(sp->u.pair, ls);
}

void evlis(List ls1, List ls2, Env* env)
{
  while (*ls1 != NULL) {
    *ls2++ = eval(*ls1++, env);
  }
}

int length(List args)
{
  int i = 0;
  while (*args != NULL){
    i++;
    args++;
  }
  return i;
}


Sexp* eval_definition(List args, Env* env)
{
  if (length(args) == 2 && 
      (*args)->type != 1 && 
      ((*args)->u.atom)->type == 1){
    define_variable(*args, eval(*(args+1), env), env);
    return *args;
  }
  else{
    printf("got %d args\n", length(args));
    readerror("Invalid argument", "-- define");
  }
}

Sexp* eval_eq(List args, Env* env)
{
  if (eval(*args, env) == (Sexp*)0 && 
      eval(*(args+1), env) == (Sexp*)0){
    return eval(mk_symbol("#t"), env);
  }
  if (eval(*args, env) == (Sexp*)0 ||
      eval(*(args+1), env) == (Sexp*)0){
    return eval(mk_symbol("#f"), env);
  }
  if (eq(eval(*args, env), eval(*(args+1), env)))
    return eval(mk_symbol("#t"), env);
  else
    return eval(mk_symbol("#f"), env);
}

Sexp* text_of_quotation(List args)
{
  if (*args != (Sexp*)0 &&
      (*args)->type == 0 && 
      ((*args)->u.atom)->type == 1)
    return intern_symbol(((*args)->u.atom)->u.sym);
  else
    return *args;
}

void extract_body(List argls, List ls)
{
  while (*argls != NULL){
    *ls++ = *argls++;
  }    
  *ls = NULL;
}

void intern_params(List params, List interned)
{
  while (*params != NULL){
    *interned++ = intern_symbol(((*params++)->u.atom)->u.sym);
  }
  *interned = NULL;
}

Sexp* make_closure(List args, Env* ep)
{
  List paramls = (List)emalloc(sizeof(List));
  List interned_params = (List)emalloc(sizeof(List));
  ptol((*args++)->u.pair, paramls);
  intern_params(paramls, interned_params);

  List bs = (List)emalloc(sizeof(List));
  extract_body(args, bs);

  Proc* procp = (Proc*)emalloc(sizeof(Proc));
  procp->params = interned_params;
  procp->body = bs;
  procp->env = ep;

  Sexp* sp = (Sexp*)emalloc(sizeof(Sexp));
  sp->type = 3;
  sp->u.proc = procp;

  return sp;
}

Sexp* eval_if(List args, Env* ep)
{
  if (length(args) < 3)
    readerror("malloformed if", " eval_if");
  if (eq(eval(*args, ep), eval(mk_symbol("#t"), ep)))
    return eval(*(args+1), ep);
  else if (length(args) > 2)
    return eval(*(args+2), ep);
  else
    return eval(mk_symbol("#f"), ep);
}

int caris(Sexp* sp, char* str)
{
  char* fname = "-- caris";
  if (sp->type != 1)
    readerror("sp is not pair",fname);
  Pair* pp = sp->u.pair;
  Sexp* carp = pp->car;
  if (carp->type != 0)
    return 0;
  Atom* ap = carp->u.atom;
  if (ap->type == 0){
    prints(sp);
    readerror("Cannot apply to a number", "");
  }
  char* carstr = ap->u.sym;

  if (!strcmp(carstr, str))
    return 1;
  else
    return 0;
}

