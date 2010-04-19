#include <string.h>
#include "es.h"
#include "y.tab.h"

Sexp *mk_S(Atom* ap, Pair* pp)
{
  Sexp *sp;

  sp = (Sexp *) emalloc(sizeof(Sexp));
  if (ap){
    sp->type = 0;
    sp->u.atom = ap;
  } else if (pp){
    sp->type = 1;
    sp->u.pair = pp;
  } else {
    error("unknown material", (Sexp*) 0);
  }
  return sp;
}

Sexp *mk_nullcar()
{
  Sexp *sp;

  sp = (Sexp *) emalloc(sizeof(Sexp));
  sp->type = -1;
  return sp;
}


Sexp *mk_num(double num)
{
  Atom *ap;

  ap = (Atom *) emalloc(sizeof(Atom));
  ap->type = 0;
  ap->u.num = num;
  return mk_S(ap, 0);
}

Sexp *mk_symbol(char* symbol)
{
  Atom *ap;

  ap = (Atom *) emalloc(sizeof(Atom));
  ap->u.sym = (char*)emalloc(strlen(symbol)+1);
  strcpy(ap->u.sym, symbol);
  ap->type = 1;
  return mk_S(ap, 0);
}

Sexp *mk_oblist(Sexp* sp1, Sexp* sp2)
{
  Pair *pp;

  if (sp1 != (Sexp*)0 && sp1->type == -1)
    return (Sexp*) 0;

  pp = (Pair *) emalloc(sizeof(Pair));
  pp->car = sp1;
  pp->cdr = sp2;
  return mk_S(0, pp);
}

