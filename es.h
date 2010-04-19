typedef struct Atom{
  short type;
  union {
    double num; /* type = 0 */
    char  *sym; /* type = 1 */
  } u;
} Atom;

typedef struct Pair {
  struct Sexp *car;
  struct Sexp *cdr;
} Pair;

typedef struct Sexp {
  /* 
     type = 0: normal atom
     type = 1: normal pair
     type = 2: primitive procedure (pair)
     type = 3: compound procedure
   */
  short type;
  union {
    struct Atom *atom; /* type = 0 */
    struct Pair *pair; /* type = 1,2 */
    struct Proc *proc; /* type = 3 */
  } u;
} Sexp;

typedef struct Env {
  struct Env *enclosing;
  struct Frame *frames;
} Env;

typedef struct Frame{
  struct Sexp *var;
  struct Sexp *val;
  struct Frame *next;
} Frame;

typedef struct Symbols{
  struct Atom *sym;
  struct Symbols *next;
} Symbols;

Env* global_env;
Symbols* symbol_table;

typedef Sexp** List;
typedef enum Calc {DIV, SUB, MUL, ADD} Calc;
typedef enum Order {LT, GT, EQ, LTEQ, GTEQ} Order;

typedef struct Proc {
  List params;
  List body;
  struct Env *env;
} Proc;


Sexp* car(List);
Sexp* cdr(List);
Sexp* cons(List);
Sexp* calc(List, Calc);
Sexp* order(List, Order);

Sexp *mk_oblist(Sexp*, Sexp*), *mk_S(Atom*, Pair*);
Sexp *mk_num(double), *mk_symbol(char*), *mk_nullcar();
Sexp *apply(Sexp*, List), *eval(Sexp*, Env*);
Sexp* apply_primitives(Sexp*, List);
void* atof_withtest(char[]);
void extract_args(Pair*, List);

int caris(Sexp*, char*);
int eqvar(Sexp*, Sexp*);
int eq(Sexp*, Sexp*);

void evlis(List, List, Env*);
Sexp* eval_definition(List, Env*);
Sexp* eval_eq(List, Env*);
Sexp* text_of_quotation(List);
Sexp* make_closure(List, Env*);
Sexp* eval_if(List, Env*);

Env* extend_env(List, List, Env*);
void mk_frame(Sexp*, Sexp*, Frame*);
void mk_frames(List, List, Frame*);

void define_variable(Sexp*, Sexp*, Env*);
void scan_define(Sexp*, Sexp*, Frame*);
Sexp* lookup_variable_value(Sexp*, Env*);
Sexp* scan_lookup(Sexp*, Env*, Frame*);

Sexp* intern_symbol(char*);
void scan_symbol(char*, Symbols*, Atom**);
int lookup_symbol(char*, Symbols*, Atom**);
Atom* symalloc(char*, Symbols*);

char* emalloc(unsigned);
void printl(List);


/* flags */
int lineno, inParen, waitingAtom, waitingCdr, needLparen, needRparen, waitingLNull;
int ibuf[100];
int *processingCdr;



