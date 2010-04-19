#include <stdio.h>
#include "es.h"

Env* extend_env(List vars, List vals, Env* base)
{
  Env* ep = (Env*) emalloc(sizeof(Env));
  ep->enclosing = base;

  Frame* fp = (Frame*)emalloc(sizeof(Frame));
  mk_frames(vars, vals, fp);
  ep->frames = fp;
 
  return ep;
}

void intern_list(char** strs, List ls)
{
  while (*strs != NULL){
    *ls++ = intern_symbol(*strs++);
  }
  *ls = NULL;
}

void objects_list(char** strs, List ls)
{
  while (*strs != NULL){
    *ls = mk_symbol(*strs++);
    (*ls)->type = 2;
    ls++;
  }
  *ls = NULL;
}

void initenv()
{
  Env* init_env = (Env*)emalloc(sizeof(Env));
  init_env->enclosing = NULL;
  init_env->frames = NULL;

  char* procstrs[] = {"car", "cdr", "cons", 
		      "+", "-", "*", "/", 
		      "<", ">", "=", "<=", ">=", NULL};
  int n = sizeof(procstrs)/sizeof(procstrs[0]);
  Sexp* pp_name[n];
  Sexp* pp_obj[n];
  intern_list(procstrs, pp_name);
  objects_list(procstrs, pp_obj);

  global_env = (Env*)emalloc(sizeof(Env));
  global_env =  extend_env(pp_name, pp_obj, init_env);

  define_variable(mk_symbol("#t"), mk_symbol("#tt"), global_env);
  define_variable(mk_symbol("#f"), mk_symbol("#ff"), global_env);

}


void mk_frame(Sexp* var, Sexp* val, Frame* fp)
{
  if (var->type != 0)
    readerror("You cannot define variable that is not Atom.");
  if (fp == NULL)
    readerror("No room for new definition.");

  fp->var = var;
  fp->val = val;
  fp->next = NULL;
}

void mk_frames(List vars, List vals, Frame* fp)
{
  mk_frame(*vars, *vals, fp);
/*   if (length(vars) > length(vals)) */
/*     readerror("too few arguments", "-- mk_frames"); */
/*   if (length(vars) < length(vals)) */
/*     readerror("too many arguments", "-- mk_frames"); */

  Sexp *var, *val;
  while ((var = *vars++) != NULL){
    scan_define(var, (val=*vals++), fp);
  }
}


void define_variable(Sexp* var, Sexp* val, Env* ep)
{
  Frame* fp;
  fp = ep->frames;

  char* varstr = (var->u.atom)->u.sym;
  Sexp* interned_var = intern_symbol(varstr); free(var);
  
  if (fp == NULL){
    Frame* newfp;
    newfp = (Frame*)emalloc(sizeof(Frame));
    mk_frame(interned_var, val, newfp);
    ep->frames = newfp;
  } else {
    scan_define(interned_var, val, fp);
  }
}

void scan_define(Sexp* var, Sexp* val, Frame* fp)
{
  if (eq(fp->var, var)){
    fp->val = val;
  } else if (fp->next == NULL){
    Frame* newfp;
    newfp = (Frame*)emalloc(sizeof(Frame));
    mk_frame(var, val, newfp);
    fp->next = newfp;
  } else {
    scan_define(var, val, fp->next);
  }
}

Sexp* lookup_variable_value(Sexp* var, Env* env)
{
  char* varstr = (var->u.atom)->u.sym;
  if (env == NULL){
    free(var);
    readerror("Unbound variable", varstr);
  }

  Frame* fp;
  if ((fp = env->frames) == NULL){
    free(var);
    readerror("Unbound variable", varstr);
  }
  
  return scan_lookup(var, env, fp);
}

Sexp* scan_lookup(Sexp* var, Env* env, Frame* fp)
{
  if (eq(fp->var, var)){
    return fp->val;
  } else if (fp->next == NULL){
    return lookup_variable_value(var, env->enclosing);
  } else {
    return scan_lookup(var, env, fp->next);
  }
}

int equal(Sexp* var1, Sexp* var2)
{
  if (var1->type != 0 || var2->type != 0)
    readerror("not atom -- eqvar");

  Atom* ap1 = var1->u.atom;
  Atom* ap2 = var2->u.atom;

  if (!strcmp(ap1->u.sym, ap2->u.sym))
    return 1;
  else 
    return 0;
}

int eq(Sexp* var1, Sexp* var2)
{
  Atom* ap1 = var1->u.atom;
  Atom* ap2 = var2->u.atom;

  /*
    printf("  ap1 = %s(%p); ", ap1->u.sym, ap1);
    printf("  ap2 = %s(%p)\n", ap2->u.sym, ap2);
  */

  if (ap1->type == 0 && ap2->type == 0){
    if (ap1->u.num == ap2->u.num)
      return 1;
    else
      return 0;
  } else if (ap1 == ap2)
    return 1;
  else
    return 0;
}







/* void readerror(char* str1, char* str2) */
/* { */
/*   printf(str1); */
/*   if (str2) */
/*     printf(str2); */
/*   printf("\n"); */
/* } */

void mk_ls(char** strs, List ls)
{
  while (*strs != NULL)
    *ls++ = mk_symbol(*strs++);
  *ls = NULL;
}

void test()
{
  Sexp* foo = mk_symbol("foo");
  Sexp* bar = mk_symbol("bar");
  Sexp* baz = mk_symbol("baz");
  Sexp* s1 = mk_num(123);
  Sexp* s2 = mk_num(456);
  Sexp* s3 = mk_num(789);

  char* procstrs[7] = {"car", "cdr", "cons", "foo", "bar", "baz", NULL};
  int n = sizeof(procstrs)/sizeof(procstrs[0]);
  Sexp* pp_name[n];
  Sexp* pp_obj[n];/* = {s1, s2, s3, s1, s2, s3, NULL}; */
  mk_ls(procstrs, pp_name);
  mk_ls(procstrs, pp_obj);

  printl(pp_name);
  printf("\n");
  printl(pp_obj);
  printf("\n");

  Frame* testfp;
  testfp = (Frame*)emalloc(sizeof(Frame));
  testfp->var = foo;
  testfp->val = s1;
  testfp->next = NULL;

  Env* testenv;
  testenv = (Env*)emalloc(sizeof(Env));
  testenv->enclosing = NULL;
  testenv->frames = testfp;

  Env* genv;
  genv = (Env*)emalloc(sizeof(Env));
  genv = extend_env(pp_name, pp_obj, testenv);
  
  printf("value of foo in testenv is ");
  prints(lookup_variable_value(bar, genv));

/*   define_variable(bar, s2, testenv); */

/*   printf("\nvalue of foo in testenv is now "); */
/*   prints(lookup_variable_value(bar, testenv)); */
/*   printf("\n"); */
}


/* main () */
/* { */
/*   test(); */
/* } */

