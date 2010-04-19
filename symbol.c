#include <stdio.h>
#include <string.h>
#include "es.h"

Sexp *intern_symbol(char* symbol)
{
  Atom** interned = (Atom**)emalloc(sizeof(Atom*));
  scan_symbol(symbol, symbol_table, interned);
  return mk_S(*interned, 0);
}

int lookup_symbol(char* symbol, Symbols* symtable, Atom** gap)
{
  if (symtable->sym == NULL){
    return 0;
  } else {
    Atom* ap = symtable->sym;
    if (!strcmp(ap->u.sym, symbol)){
      *gap = ap;
      return 1;
    } else if (symtable->next == NULL){
      return 0;
    } else {
      return lookup_symbol(symbol, symtable->next, gap);
    }
  }
}

void scan_symbol(char* symbol, Symbols* symtable, Atom** gap)
{
  if (symtable->sym == NULL){
    *gap = symalloc(symbol, symtable);
  } else {
    Atom* ap = symtable->sym;
    if (!strcmp(ap->u.sym, symbol)){
      *gap = ap;
    } else if (symtable->next == NULL){
      Symbols* newsymtbl;
      newsymtbl = (Symbols*)emalloc(sizeof(Symbols));
      symtable->next = newsymtbl;
      *gap = symalloc(symbol, symtable->next);
    } else {
      scan_symbol(symbol, symtable->next, gap);
    }
  }
}

Atom *symalloc(char* symbol, Symbols* symtable)
{
  Atom* ap;
  ap = (Atom*)emalloc(sizeof(Atom));
  ap->type = 1;
  ap->u.sym = (char*)emalloc(strlen(symbol)+1);
  strcpy(ap->u.sym, symbol);

  symtable->sym = ap;
  symtable->next = NULL;

  /* printf("newly interns symbol: %s\n", ap->u.sym); */
  return ap;
} 
