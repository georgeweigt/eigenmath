/*
BSD 2-Clause License

Copyright (c) 2023, George Weigt
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <errno.h>

#define STACKSIZE 100000 // evaluation stack
#define FRAMESIZE 10000
#define BLOCKSIZE 10000
#define MAXBLOCKS 2000
#define BUCKETSIZE 100
#define STRBUFLEN 1000
#define MAXDIM 24

// MAXBLOCKS * BLOCKSIZE = 20,000,000 atoms

// MAXBLOCKS * BLOCKSIZE * sizeof (struct atom) = 480,000,000 bytes

// Symbolic expressions are built by linking structs of type "atom".
//
// For example, the expression "a b + c" is built like this:
//
//  _______      _______                                _______      _______
// |CONS   |    |CONS   |                              |CONS   |    |SYM    |
// |car cdr|--->|car cdr|----------------------------->|car cdr|--->|"nil"  |
// |_|_____|    |_|_____|                              |_|_____|    |_______|
//   |            |                                      |
//   |            |                                     _v_____
//   |            |                                    |SYM    |
//   |            |                                    |"c"    |
//   |            |                                    |_______|
//   |            |
//  _v_____      _v_____      _______      _______      _______
// |SYM    |    |CONS   |    |CONS   |    |CONS   |    |SYM    |
// |"add"  |    |car cdr|--->|car cdr|--->|car cdr|--->|"nil"  |
// |_______|    |_|_____|    |_|_____|    |_|_____|    |_______|
//                |            |            |
//               _v_____      _v_____      _v_____
//              |SYM    |    |SYM    |    |SYM    |
//              |"mul"  |    |"a"    |    |"b"    |
//              |_______|    |_______|    |_______|

struct atom {
	union {
		struct {
			struct atom *car;
			struct atom *cdr;
		} cons;
		struct {
			char *name;
			void (*func)(struct atom *);
		} ksym;
		struct {
			char *name;
			uint32_t index;
		} usym;
		struct {
			uint32_t *a; // rational number a over b
			uint32_t *b;
		} q;
		double d;
		char *str;
		struct tensor *tensor;
		struct atom *next;
	} u;
	uint8_t atomtype, tag, sign;
};

struct tensor {
	int ndim;
	int dim[MAXDIM];
	int nelem;
	struct atom *elem[1];
};

// atom types

#define FREEATOM	0
#define CONS		1
#define KSYM		2
#define USYM		3
#define RATIONAL	4
#define DOUBLE		5
#define STR		6
#define TENSOR		7

// symbol table

#define ABS		(0 * BUCKETSIZE + 0)
#define ADJ		(0 * BUCKETSIZE + 1)
#define AND		(0 * BUCKETSIZE + 2)
#define ARCCOS		(0 * BUCKETSIZE + 3)
#define ARCCOSH		(0 * BUCKETSIZE + 4)
#define ARCSIN		(0 * BUCKETSIZE + 5)
#define ARCSINH		(0 * BUCKETSIZE + 6)
#define ARCTAN		(0 * BUCKETSIZE + 7)
#define ARCTANH		(0 * BUCKETSIZE + 8)
#define ARG		(0 * BUCKETSIZE + 9)

#define BINDING		(1 * BUCKETSIZE + 0)

#define C_UPPER		(2 * BUCKETSIZE + 0)
#define C_LOWER		(2 * BUCKETSIZE + 1)
#define CEILING		(2 * BUCKETSIZE + 2)
#define CHECK		(2 * BUCKETSIZE + 3)
#define CIRCEXP		(2 * BUCKETSIZE + 4)
#define CLEAR		(2 * BUCKETSIZE + 5)
#define CLOCK		(2 * BUCKETSIZE + 6)
#define COFACTOR	(2 * BUCKETSIZE + 7)
#define CONJ		(2 * BUCKETSIZE + 8)
#define CONTRACT	(2 * BUCKETSIZE + 9)
#define COS		(2 * BUCKETSIZE + 10)
#define COSH		(2 * BUCKETSIZE + 11)

#define D_UPPER		(3 * BUCKETSIZE + 0)
#define D_LOWER		(3 * BUCKETSIZE + 1)
#define DEFINT		(3 * BUCKETSIZE + 2)
#define DENOMINATOR	(3 * BUCKETSIZE + 3)
#define DERIVATIVE	(3 * BUCKETSIZE + 4)
#define DET		(3 * BUCKETSIZE + 5)
#define DIM		(3 * BUCKETSIZE + 6)
#define DO		(3 * BUCKETSIZE + 7)
#define DOT		(3 * BUCKETSIZE + 8)
#define DRAW		(3 * BUCKETSIZE + 9)

#define EIGENVEC	(4 * BUCKETSIZE + 0)
#define ERF		(4 * BUCKETSIZE + 1)
#define ERFC		(4 * BUCKETSIZE + 2)
#define EVAL		(4 * BUCKETSIZE + 3)
#define EXIT		(4 * BUCKETSIZE + 4)
#define EXP		(4 * BUCKETSIZE + 5)
#define EXPCOS		(4 * BUCKETSIZE + 6)
#define EXPCOSH		(4 * BUCKETSIZE + 7)
#define EXPSIN		(4 * BUCKETSIZE + 8)
#define EXPSINH		(4 * BUCKETSIZE + 9)
#define EXPTAN		(4 * BUCKETSIZE + 10)
#define EXPTANH		(4 * BUCKETSIZE + 11)

#define FACTORIAL	(5 * BUCKETSIZE + 0)
#define FLOATF		(5 * BUCKETSIZE + 1)
#define FLOOR		(5 * BUCKETSIZE + 2)
#define FOR		(5 * BUCKETSIZE + 3)

#define H_UPPER		(7 * BUCKETSIZE + 0)
#define H_LOWER		(7 * BUCKETSIZE + 1)
#define HADAMARD	(7 * BUCKETSIZE + 2)

#define I_UPPER		(8 * BUCKETSIZE + 0)
#define I_LOWER		(8 * BUCKETSIZE + 1)
#define IMAG		(8 * BUCKETSIZE + 2)
#define INFIXFORM	(8 * BUCKETSIZE + 3)
#define INNER		(8 * BUCKETSIZE + 4)
#define INTEGRAL	(8 * BUCKETSIZE + 5)
#define INV		(8 * BUCKETSIZE + 6)

#define J_UPPER		(9 * BUCKETSIZE + 0)
#define J_LOWER		(9 * BUCKETSIZE + 1)

#define KRONECKER	(10 * BUCKETSIZE + 0)

#define LAST		(11 * BUCKETSIZE + 0)
#define LOG		(11 * BUCKETSIZE + 1)

#define MAG		(12 * BUCKETSIZE + 0)
#define MINOR		(12 * BUCKETSIZE + 1)
#define MINORMATRIX	(12 * BUCKETSIZE + 2)
#define MOD		(12 * BUCKETSIZE + 3)

#define NIL		(13 * BUCKETSIZE + 0)
#define NOEXPAND	(13 * BUCKETSIZE + 1)
#define NOT		(13 * BUCKETSIZE + 2)
#define NROOTS		(13 * BUCKETSIZE + 3)
#define NUMBER		(13 * BUCKETSIZE + 4)
#define NUMERATOR	(13 * BUCKETSIZE + 5)

#define OR		(14 * BUCKETSIZE + 0)
#define OUTER		(14 * BUCKETSIZE + 1)

#define P_UPPER		(15 * BUCKETSIZE + 0)
#define P_LOWER		(15 * BUCKETSIZE + 1)
#define PI		(15 * BUCKETSIZE + 2)
#define POLAR		(15 * BUCKETSIZE + 3)
#define PREFIXFORM	(15 * BUCKETSIZE + 4)
#define PRINT		(15 * BUCKETSIZE + 5)
#define PRODUCT		(15 * BUCKETSIZE + 6)

#define Q_UPPER		(16 * BUCKETSIZE + 0)
#define Q_LOWER		(16 * BUCKETSIZE + 1)
#define QUOTE		(16 * BUCKETSIZE + 2)

#define R_UPPER		(17 * BUCKETSIZE + 0)
#define R_LOWER		(17 * BUCKETSIZE + 1)
#define RANK		(17 * BUCKETSIZE + 2)
#define RATIONALIZE	(17 * BUCKETSIZE + 3)
#define REAL		(17 * BUCKETSIZE + 4)
#define RECTF		(17 * BUCKETSIZE + 5)
#define ROOTS		(17 * BUCKETSIZE + 6)
#define ROTATE		(17 * BUCKETSIZE + 7)
#define RUN		(17 * BUCKETSIZE + 8)

#define S_UPPER		(18 * BUCKETSIZE + 0)
#define S_LOWER		(18 * BUCKETSIZE + 1)
#define SGN		(18 * BUCKETSIZE + 2)
#define SIMPLIFY	(18 * BUCKETSIZE + 3)
#define SIN		(18 * BUCKETSIZE + 4)
#define SINH		(18 * BUCKETSIZE + 5)
#define SQRT		(18 * BUCKETSIZE + 6)
#define STATUS		(18 * BUCKETSIZE + 7)
#define STOP		(18 * BUCKETSIZE + 8)
#define SUBST		(18 * BUCKETSIZE + 9)
#define SUM		(18 * BUCKETSIZE + 10)

#define T_UPPER		(19 * BUCKETSIZE + 0)
#define T_LOWER		(19 * BUCKETSIZE + 1)
#define TAN		(19 * BUCKETSIZE + 2)
#define TANH		(19 * BUCKETSIZE + 3)
#define TAYLOR		(19 * BUCKETSIZE + 4)
#define TEST		(19 * BUCKETSIZE + 5)
#define TESTEQ		(19 * BUCKETSIZE + 6)
#define TESTGE		(19 * BUCKETSIZE + 7)
#define TESTGT		(19 * BUCKETSIZE + 8)
#define TESTLE		(19 * BUCKETSIZE + 9)
#define TESTLT		(19 * BUCKETSIZE + 10)
#define TRACE		(19 * BUCKETSIZE + 11)
#define TRANSPOSE	(19 * BUCKETSIZE + 12)
#define TTY		(19 * BUCKETSIZE + 13)

#define U_UPPER		(20 * BUCKETSIZE + 0)
#define U_LOWER		(20 * BUCKETSIZE + 1)
#define UNIT		(20 * BUCKETSIZE + 2)

#define V_UPPER		(21 * BUCKETSIZE + 0)
#define V_LOWER		(21 * BUCKETSIZE + 1)

#define W_UPPER		(22 * BUCKETSIZE + 0)
#define W_LOWER		(22 * BUCKETSIZE + 1)

#define X_UPPER		(23 * BUCKETSIZE + 0)
#define X_LOWER		(23 * BUCKETSIZE + 1)

#define Y_UPPER		(24 * BUCKETSIZE + 0)
#define Y_LOWER		(24 * BUCKETSIZE + 1)

#define Z_UPPER		(25 * BUCKETSIZE + 0)
#define Z_LOWER		(25 * BUCKETSIZE + 1)
#define ZERO		(25 * BUCKETSIZE + 2)

#define ADD		(26 * BUCKETSIZE + 0)
#define MULTIPLY	(26 * BUCKETSIZE + 1)
#define POWER		(26 * BUCKETSIZE + 2)
#define INDEX		(26 * BUCKETSIZE + 3)
#define SETQ		(26 * BUCKETSIZE + 4)
#define EXP1		(26 * BUCKETSIZE + 5)
#define SA		(26 * BUCKETSIZE + 6)
#define SB		(26 * BUCKETSIZE + 7)
#define SX		(26 * BUCKETSIZE + 8)
#define ARG1		(26 * BUCKETSIZE + 9)
#define ARG2		(26 * BUCKETSIZE + 10)
#define ARG3		(26 * BUCKETSIZE + 11)
#define ARG4		(26 * BUCKETSIZE + 12)
#define ARG5		(26 * BUCKETSIZE + 13)
#define ARG6		(26 * BUCKETSIZE + 14)
#define ARG7		(26 * BUCKETSIZE + 15)
#define ARG8		(26 * BUCKETSIZE + 16)
#define ARG9		(26 * BUCKETSIZE + 17)

#define symbol(x) symtab[x]
#define push_symbol(x) push(symbol(x))
#define iscons(p) ((p)->atomtype == CONS)
#define isrational(p) ((p)->atomtype == RATIONAL)
#define isdouble(p) ((p)->atomtype == DOUBLE)
#define isnum(p) (isrational(p) || isdouble(p))
#define isstr(p) ((p)->atomtype == STR)
#define istensor(p) ((p)->atomtype == TENSOR)
#define iskeyword(p) ((p)->atomtype == KSYM)
#define isusersymbol(p) ((p)->atomtype == USYM)
#define issymbol(p) (iskeyword(p) || isusersymbol(p))
#define equal(p1, p2) (cmp(p1, p2) == 0)
#define lessp(p1, p2) (cmp(p1, p2) < 0)

#define car(p) (iscons(p) ? (p)->u.cons.car : symbol(NIL))
#define cdr(p) (iscons(p) ? (p)->u.cons.cdr : symbol(NIL))
#define caar(p) car(car(p))
#define cadr(p) car(cdr(p))
#define cdar(p) cdr(car(p))
#define cddr(p) cdr(cdr(p))
#define caadr(p) car(car(cdr(p)))
#define caddr(p) car(cdr(cdr(p)))
#define cadar(p) car(cdr(car(p)))
#define cdadr(p) cdr(car(cdr(p)))
#define cddar(p) cdr(cdr(car(p)))
#define cdddr(p) cdr(cdr(cdr(p)))
#define caaddr(p) car(car(cdr(cdr(p))))
#define cadadr(p) car(cdr(car(cdr(p))))
#define caddar(p) car(cdr(cdr(car(p))))
#define cadddr(p) car(cdr(cdr(cdr(p))))
#define cdaddr(p) cdr(car(cdr(cdr(p))))
#define cddadr(p) cdr(cdr(car(cdr(p))))
#define cddddr(p) cdr(cdr(cdr(cdr(p))))
#define caddddr(p) car(cdr(cdr(cdr(cdr(p)))))
#define cadaddr(p) car(cdr(car(cdr(cdr(p)))))
#define cddaddr(p) cdr(cdr(car(cdr(cdr(p)))))
#define caddadr(p) car(cdr(cdr(car(cdr(p)))))
#define cdddaddr(p) cdr(cdr(cdr(car(cdr(cdr(p))))))
#define caddaddr(p) car(cdr(cdr(car(cdr(cdr(p))))))

#define MPLUS 0
#define MMINUS 1
#define MLENGTH(p) (((int *) (p))[-1])
#define MZERO(p) (MLENGTH(p) == 1 && (p)[0] == 0)
#define MEQUAL(p, n) (MLENGTH(p) == 1 && (p)[0] == (n))

#define BLACK 0
#define BLUE 1
#define RED 2

#define Trace fprintf(stderr, "%s %d\n", __func__, __LINE__);

extern struct atom *mem[MAXBLOCKS]; // an array of pointers
extern struct atom *free_list;
extern int tos; // top of stack
extern int tof; // top of frame
extern struct atom *stack[STACKSIZE];
extern struct atom *frame[FRAMESIZE];
extern struct atom *symtab[27 * BUCKETSIZE];
extern struct atom *binding[27 * BUCKETSIZE];
extern struct atom *usrfunc[27 * BUCKETSIZE];
extern struct atom *zero;
extern struct atom *one;
extern struct atom *minusone;
extern struct atom *imaginaryunit;
extern int eval_level;
extern int gc_level;
extern int expanding;
extern int drawing;
extern int nonstop;
extern int interrupt;
extern jmp_buf jmpbuf0;
extern jmp_buf jmpbuf1;
extern char *trace1;
extern char *trace2;
extern int alloc_count;
extern int block_count;
extern int free_count;
extern int gc_count;
extern int bignum_count;
extern int ksym_count;
extern int usym_count;
extern int string_count;
extern int tensor_count;
extern int max_eval_level;
extern int max_tos;
extern int max_tof;
extern char strbuf[];
extern char *outbuf;
extern int outbuf_index;
extern int outbuf_length;
struct atom * alloc_atom(void);
void alloc_block(void);
struct atom * alloc_vector(int nrow);
struct atom * alloc_matrix(int nrow, int ncol);
struct atom * alloc_tensor(int nelem);
struct atom * alloc_str(void);
void * alloc_mem(int n);
double mfloat(uint32_t *p);
void msetbit(uint32_t *x, uint32_t k);
void mclrbit(uint32_t *x, uint32_t k);
uint32_t * mscan(char *s);
char * mstr(uint32_t *u);
int mdivby1billion(uint32_t *u);
uint32_t * madd(uint32_t *u, uint32_t *v);
uint32_t * msub(uint32_t *u, uint32_t *v);
uint32_t * mmul(uint32_t *u, uint32_t *v);
uint32_t * mdiv(uint32_t *u, uint32_t *v);
uint32_t * mmod(uint32_t *u, uint32_t *v);
uint32_t * mpow(uint32_t *u, uint32_t *v);
void mshr(uint32_t *u);
int mcmp(uint32_t *u, uint32_t *v);
int meq(uint32_t *u, uint32_t *v);
uint32_t * mint(uint32_t n);
uint32_t * mnew(int n);
void mfree(uint32_t *u);
uint32_t * mcopy(uint32_t *u);
void mnorm(uint32_t *u);
uint32_t * mgcd(uint32_t *u, uint32_t *v);
uint32_t * mroot(uint32_t *a, uint32_t *n);
void list(int n);
void cons(void);
int lengthf(struct atom *p);
int findf(struct atom *p, struct atom *q);
void sort(int n);
int sort_func(const void *p1, const void *p2);
int cmp(struct atom *p1, struct atom *p2);
int cmp_numbers(struct atom *p1, struct atom *p2);
int cmp_rationals(struct atom *a, struct atom *b);
int cmp_tensors(struct atom *p1, struct atom *p2);
int find_denominator(struct atom *p);
int count_denominators(struct atom *p);
int count_numerators(struct atom *p);
void evalg(void);
void evalf(void);
void evalf_nib(struct atom *p1);
void evalp(void);
void eval_abs(struct atom *p1);
void absfunc(void);
void eval_add(struct atom *p1);
void add(void);
void add_terms(int n);
void flatten_terms(int h);
struct atom * combine_tensors(int h);
void add_tensors(void);
void combine_terms(int h);
int combine_terms_nib(int i, int j);
void sort_terms(int n);
int sort_terms_func(const void *q1, const void *q2);
int cmp_terms(struct atom *p1, struct atom *p2);
int simplify_terms(int h);
int isradicalterm(struct atom *p);
int isimaginaryterm(struct atom *p);
int isimaginaryfactor(struct atom *p);
void add_numbers(struct atom *p1, struct atom *p2);
void add_rationals(struct atom *p1, struct atom *p2);
void add_integers(struct atom *p1, struct atom *p2);
void subtract(void);
void eval_adj(struct atom *p1);
void adj(void);
void eval_and(struct atom *p1);
void eval_arccos(struct atom *p1);
void arccos(void);
void eval_arccosh(struct atom *p1);
void arccosh(void);
void eval_arcsin(struct atom *p1);
void arcsin(void);
void eval_arcsinh(struct atom *p1);
void arcsinh(void);
void eval_arctan(struct atom *p1);
void arctan(void);
void arctan_numbers(struct atom *X, struct atom *Y);
void eval_arctanh(struct atom *p1);
void arctanh(void);
void eval_arg(struct atom *p1);
void arg(void);
void arg1(void);
void eval_binding(struct atom *p1);
void eval_ceiling(struct atom *p1);
void ceilingfunc(void);
void eval_check(struct atom *p1);
void eval_circexp(struct atom *p1);
void circexp(void);
void circexp_subst(void);
void eval_clear(struct atom *p1);
void eval_clock(struct atom *p1);
void clockfunc(void);
void eval_cofactor(struct atom *p1);
void eval_conj(struct atom *p1);
void conjfunc(void);
void conjfunc_subst(void);
void eval_contract(struct atom *p1);
void contract(void);
void eval_cos(struct atom *p1);
void cosfunc(void);
void cosfunc_sum(struct atom *p1);
void eval_cosh(struct atom *p1);
void coshfunc(void);
void eval_defint(struct atom *p1);
void eval_denominator(struct atom *p1);
void denominator(void);
void eval_derivative(struct atom *p1);
void derivative(void);
void d_scalar_scalar(struct atom *F, struct atom *X);
void dsum(struct atom *p1, struct atom *p2);
void dproduct(struct atom *p1, struct atom *p2);
void dpower(struct atom *F, struct atom *X);
void dlog(struct atom *p1, struct atom *p2);
void dd(struct atom *p1, struct atom *p2);
void dfunction(struct atom *p1, struct atom *p2);
void dsin(struct atom *p1, struct atom *p2);
void dcos(struct atom *p1, struct atom *p2);
void dtan(struct atom *p1, struct atom *p2);
void darcsin(struct atom *p1, struct atom *p2);
void darccos(struct atom *p1, struct atom *p2);
void darctan(struct atom *p1, struct atom *p2);
void dsinh(struct atom *p1, struct atom *p2);
void dcosh(struct atom *p1, struct atom *p2);
void dtanh(struct atom *p1, struct atom *p2);
void darcsinh(struct atom *p1, struct atom *p2);
void darccosh(struct atom *p1, struct atom *p2);
void darctanh(struct atom *p1, struct atom *p2);
void derf(struct atom *p1, struct atom *p2);
void derfc(struct atom *p1, struct atom *p2);
void d_tensor_tensor(struct atom *p1, struct atom *p2);
void d_scalar_tensor(struct atom *p1, struct atom *p2);
void d_tensor_scalar(struct atom *p1, struct atom *p2);
void eval_det(struct atom *p1);
void det(void);
void eval_dim(struct atom *p1);
void eval_do(struct atom *p1);
void eval_eigenvec(struct atom *p1);
void eigenvec(double *D, double *Q, int n);
int eigenvec_step(double *D, double *Q, int n);
void eigenvec_step_nib(double *D, double *Q, int n, int p, int q);
void eval_erf(struct atom *p1);
void erffunc(void);
void eval_erfc(struct atom *p1);
void erfcfunc(void);
void eval_eval(struct atom *p1);
void asubst(void);
int acmp(struct atom *p1, struct atom *p2);
void eval_exp(struct atom *p1);
void expfunc(void);
void eval_expcos(struct atom *p1);
void expcos(void);
void eval_expcosh(struct atom *p1);
void expcosh(void);
void eval_expsin(struct atom *p1);
void expsin(void);
void eval_expsinh(struct atom *p1);
void expsinh(void);
void eval_exptan(struct atom *p1);
void exptan(void);
void eval_exptanh(struct atom *p1);
void exptanh(void);
void eval_factorial(struct atom *p1);
void factorial(void);
void eval_float(struct atom *p1);
void floatfunc(void);
void floatfunc_subst(void);
void eval_floor(struct atom *p1);
void floorfunc(void);
void eval_for(struct atom *p1);
void eval_hadamard(struct atom *p1);
void hadamard(void);
void eval_imag(struct atom *p1);
void imag(void);
void eval_index(struct atom *p1);
void indexfunc(struct atom *T, int h);
void eval_infixform(struct atom *p1);
void print_infixform(struct atom *p);
void infixform_subexpr(struct atom *p);
void infixform_expr(struct atom *p);
void infixform_expr_nib(struct atom *p);
void infixform_term(struct atom *p);
void infixform_term_nib(struct atom *p);
void infixform_numerators(struct atom *p);
void infixform_denominators(struct atom *p);
void infixform_factor(struct atom *p);
void infixform_power(struct atom *p);
void infixform_reciprocal(struct atom *p);
void infixform_factorial(struct atom *p);
void infixform_index(struct atom *p);
void infixform_arglist(struct atom *p);
void infixform_rational(struct atom *p);
void infixform_double(struct atom *p);
void infixform_base(struct atom *p);
void infixform_numeric_base(struct atom *p);
void infixform_numeric_exponent(struct atom *p);
void infixform_tensor(struct atom *p);
void infixform_tensor_nib(struct atom *p, int d, int k);
void eval_inner(struct atom *p1);
void inner(void);
void eval_integral(struct atom *p1);
void integral(void);
void integral_nib(struct atom *F, struct atom *X);
void integral_lookup(int h, struct atom *F);
int integral_classify(struct atom *p);
int integral_search(int h, struct atom *F, char **table, int n);
int integral_search_nib(int h, struct atom *F, struct atom *I, struct atom *C);
void decomp(void);
void decomp_sum(struct atom *F, struct atom *X);
void decomp_product(struct atom *F, struct atom *X);
void partition_term(void);
void eval_inv(struct atom *p1);
void inv(void);
void eval_kronecker(struct atom *p1);
void kronecker(void);
void eval_log(struct atom *p1);
void logfunc(void);
void eval_mag(struct atom *p1);
void mag(void);
void mag_nib(void);
void eval_minor(struct atom *p1);
void eval_minormatrix(struct atom *p1);
void minormatrix(int row, int col);
void eval_mod(struct atom *p1);
void modfunc(void);
void mod_rationals(struct atom *p1, struct atom *p2);
void mod_integers(struct atom *p1, struct atom *p2);
void eval_multiply(struct atom *p1);
void multiply(void);
void multiply_factors(int n);
void flatten_factors(int h);
struct atom * multiply_tensor_factors(int h);
void multiply_scalar_factors(int h);
struct atom * combine_numerical_factors(int h, struct atom *COEF);
void combine_factors(int h);
int combine_factors_nib(int i, int j);
void sort_factors_provisional(int h);
int sort_factors_provisional_func(const void *q1, const void *q2);
int cmp_factors_provisional(struct atom *p1, struct atom *p2);
void normalize_power_factors(int h);
void expand_sum_factors(int h);
void sort_factors(int n);
int sort_factors_func(const void *q1, const void *q2);
int cmp_factors(struct atom *p1, struct atom *p2);
int order_factor(struct atom *p);
void multiply_numbers(struct atom *p1, struct atom *p2);
void multiply_rationals(struct atom *p1, struct atom *p2);
struct atom * reduce_radical_factors(int h, struct atom *COEF);
int any_radical_factors(int h);
struct atom * reduce_radical_double(int h, struct atom *COEF);
struct atom * reduce_radical_rational(int h, struct atom *COEF);
void multiply_expand(void);
void multiply_noexpand(void);
void multiply_factors_noexpand(int n);
void negate(void);
void negate_noexpand(void);
void reciprocate(void);
void divide(void);
void eval_nil(struct atom *p1);
void eval_noexpand(struct atom *p1);
void eval_not(struct atom *p1);
void eval_nroots(struct atom *p1);
void nroots(void);
void nfindroot(double cr[], double ci[], int n, double *par, double *pai);
void fata(double cr[], double ci[], int n, double ar, double ai, double *far, double *fai);
void nreduce(double cr[], double ci[], int n, double ar, double ai);
double zabs(double r, double i);
double urandom(void);
void eval_number(struct atom *p1);
void eval_numerator(struct atom *p1);
void numerator(void);
void eval_or(struct atom *p1);
void eval_outer(struct atom *p1);
void outer(void);
void eval_polar(struct atom *p1);
void polar(void);
void eval_power(struct atom *p1);
void power(void);
void power_numbers(struct atom *BASE, struct atom *EXPO);
void power_numbers_factor(struct atom *BASE, struct atom *EXPO);
void power_double(struct atom *BASE, struct atom *EXPO);
void power_minusone(struct atom *EXPO);
void normalize_clock_rational(struct atom *EXPO);
void normalize_clock_double(struct atom *EXPO);
void power_natural_number(struct atom *EXPO);
void normalize_polar(struct atom *EXPO);
void normalize_polar_term(struct atom *EXPO);
void normalize_polar_term_rational(struct atom *R);
void normalize_polar_term_double(struct atom *R);
void power_sum(struct atom *BASE, struct atom *EXPO);
void power_complex_number(struct atom *BASE, struct atom *EXPO);
void power_complex_plus(struct atom *X, struct atom *Y, int n);
void power_complex_minus(struct atom *X, struct atom *Y, int n);
void power_complex_double(struct atom *X, struct atom *Y, struct atom *EXPO);
void power_complex_rational(struct atom *X, struct atom *Y, struct atom *EXPO);
void eval_prefixform(struct atom *p1);
void print_prefixform(struct atom *p);
void prefixform(struct atom *p);
void eval_print(struct atom *p1);
void print_result(void);
int annotate_result(struct atom *p1, struct atom *p2);
void eval_product(struct atom *p1);
void eval_quote(struct atom *p1);
void eval_rank(struct atom *p1);
void eval_rationalize(struct atom *p1);
void rationalize(void);
int find_divisor(struct atom *p);
int find_divisor_term(struct atom *p);
int find_divisor_factor(struct atom *p);
void cancel_factor(void);
void eval_real(struct atom *p1);
void real(void);
void eval_rect(struct atom *p1);
void rect(void);
void eval_roots(struct atom *p1);
void roots(void);
int findroot(int h, int n);
void horner(int h, int n, struct atom *A);
void divisors(int n);
void divisors_nib(int h, int k);
void reduce(int h, int n, struct atom *A);
void coeffs(struct atom *P, struct atom *X);
void eval_rotate(struct atom *p1);
void rotate_h(struct atom *PSI, uint32_t c, int n);
void rotate_p(struct atom *PSI, struct atom *PHASE, uint32_t c, int n);
void rotate_w(struct atom *PSI, uint32_t c, int m, int n);
void rotate_x(struct atom *PSI, uint32_t c, int n);
void rotate_y(struct atom *PSI, uint32_t c, int n);
void rotate_z(struct atom *PSI, uint32_t c, int n);
void rotate_q(struct atom *PSI, int n);
void rotate_v(struct atom *PSI, int n);
void eval_run(struct atom *p1);
char * read_file(char *filename);
void eval_setq(struct atom *p1);
void setq_indexed(struct atom *p1);
void set_component(struct atom *LVAL, struct atom *RVAL, int h);
void setq_usrfunc(struct atom *p1);
void convert_body(struct atom *A);
void eval_sgn(struct atom *p1);
void sgn(void);
void eval_simplify(struct atom *p1);
void simplify(void);
void simplify_tensor(struct atom *p1);
void simplify_scalar(struct atom *p1);
void simplify_pass1(void);
void simplify_pass2(void);
void simplify_pass3(void);
int complexity(struct atom *p);
void eval_sin(struct atom *p1);
void sinfunc(void);
void sinfunc_sum(struct atom *p1);
void eval_sinh(struct atom *p1);
void sinhfunc(void);
void eval_sqrt(struct atom *p1);
void sqrtfunc(void);
void eval_status(struct atom *p1);
void eval_stop(struct atom *p1);
void eval_subst(struct atom *p1);
void subst(void);
void eval_sum(struct atom *p1);
void eval_tan(struct atom *p1);
void tanfunc(void);
void tanfunc_sum(struct atom *p1);
void eval_tanh(struct atom *p1);
void tanhfunc(void);
void eval_taylor(struct atom *p1);
void eval_tensor(struct atom *p1);
void promote_tensor(void);
int compatible_dimensions(struct atom *p, struct atom *q);
struct atom * copy_tensor(struct atom *p1);
void eval_test(struct atom *p1);
void eval_testeq(struct atom *p1);
void eval_testge(struct atom *p1);
void eval_testgt(struct atom *p1);
void eval_testle(struct atom *p1);
void eval_testlt(struct atom *p1);
int cmp_args(struct atom *p1);
void eval_transpose(struct atom *p1);
void transpose(int n, int m);
void eval_unit(struct atom *p1);
void eval_user_function(struct atom *p1);
void eval_user_symbol(struct atom *p1);
void eval_zero(struct atom *p1);
void factor_factor(void);
void factor_bignum(uint32_t *N, struct atom *M);
void factor_int(int n);
void fmt(void);
void fmt_args(struct atom *p);
void fmt_base(struct atom *p);
void fmt_denominators(struct atom *p);
void fmt_double(struct atom *p);
void fmt_exponent(struct atom *p);
void fmt_expr(struct atom *p);
void fmt_expr_nib(struct atom *p);
void fmt_factor(struct atom *p);
void fmt_frac(struct atom *p);
void fmt_function(struct atom *p);
void fmt_indices(struct atom *p);
void fmt_infix_operator(int c);
void fmt_list(struct atom *p);
void fmt_matrix(struct atom *p, int d, int k);
void fmt_numerators(struct atom *p);
void fmt_numeric_exponent(struct atom *p);
void fmt_power(struct atom *p);
void fmt_rational(struct atom *p);
void fmt_reciprocal(struct atom *p);
void fmt_roman_char(int c);
void fmt_roman_string(char *s);
void fmt_space(void);
void fmt_string(struct atom *p);
void fmt_subexpr(struct atom *p);
void fmt_symbol(struct atom *p);
int fmt_symbol_fragment(char *s, int k);
void fmt_table(int x, int y, struct atom *p);
void fmt_tensor(struct atom *p);
void fmt_term(struct atom *p);
void fmt_term_nib(struct atom *p);
void fmt_update_fraction(void);
void fmt_update_list(int t);
void fmt_update_subexpr(void);
void fmt_update_subscript(void);
void fmt_update_superscript(void);
void fmt_update_table(int n, int m);
void fmt_vector(struct atom *p);
void fmt_draw(int x, int y, struct atom *p);
void fmt_draw_char(int x, int y, int c);
void fmt_draw_delims(int x, int y, int h, int d, int w);
void fmt_draw_ldelim(int x, int y, int h, int d);
void fmt_draw_rdelim(int x, int y, int h, int d);
void fmt_draw_table(int x, int y, struct atom *p);
void fmt_putw(uint32_t w);
void gc(void);
void untag(struct atom *p);
int main(int argc, char *argv[]);
void run_infile(char *infile);
void run_stdin(void);
void display(void);
void printbuf(char *s, int color);
void eval_draw(struct atom *p1);
void eval_exit(struct atom *p1);
void outbuf_init(void);
void outbuf_puts(char *s);
void outbuf_putc(int c);
int iszero(struct atom *p);
int isequaln(struct atom *p, int n);
int isequalq(struct atom *p, int a, int b);
int isplusone(struct atom *p);
int isminusone(struct atom *p);
int isinteger(struct atom *p);
int isinteger1(struct atom *p);
int isfraction(struct atom *p);
int isposint(struct atom *p);
int iseveninteger(struct atom *p);
int isradical(struct atom *p);
int isnegativeterm(struct atom *p);
int isnegativenumber(struct atom *p);
int iscomplexnumber(struct atom *p);
int isimaginarynumber(struct atom *p);
int isimaginaryunit(struct atom *p);
int isoneoversqrttwo(struct atom *p);
int isminusoneoversqrttwo(struct atom *p);
int isdoublez(struct atom *p);
int isdenominator(struct atom *p);
int isnumerator(struct atom *p);
int isdoublesomewhere(struct atom *p);
int isdenormalpolar(struct atom *p);
int isdenormalpolarterm(struct atom *p);
int issquarematrix(struct atom *p);
int issmallinteger(struct atom *p);
void run(char *buf);
void run_buf(char *buf);
char * scan_input(char *s);
void print_trace(int color);
void run_init_script(void);
void stopf(char *s);
void exitf(char *s);
char * scan(char *s);
char * scan1(char *s);
char * scan_nib(char *s);
void scan_stmt(void);
void scan_comparison(void);
void scan_expression(void);
int another_factor_pending(void);
void scan_term(void);
void scan_power(void);
void scan_factor(void);
void scan_symbol(void);
void scan_string(void);
void scan_function_call(void);
void scan_integer(void);
void scan_subexpr(void);
void get_token_skip_newlines(void);
void get_token(void);
void get_token_nib(void);
void update_token_buf(char *a, char *b);
void scan_error(char *errmsg);
void build_tensor(int h);
void static_negate(void);
void static_reciprocate(void);
void push(struct atom *p);
struct atom * pop(void);
void fpush(struct atom *p);
struct atom * fpop(void);
void save_symbol(struct atom *p);
void restore_symbol(void);
void dupl(void);
void swap(void);
void push_integer(int n);
void push_rational(int a, int b);
void push_bignum(int sign, uint32_t *a, uint32_t *b);
int pop_integer(void);
void push_double(double d);
double pop_double(void);
void push_string(char *s);
struct atom * lookup(char *s);
char * printname(struct atom *p);
void set_symbol(struct atom *p1, struct atom *p2, struct atom *p3);
struct atom * get_binding(struct atom *p1);
struct atom * get_usrfunc(struct atom *p);
void init_symbol_table(void);
struct atom *
alloc_atom(void)
{
	struct atom *p;

	if (free_count == 0)
		alloc_block();

	p = free_list;
	free_list = p->u.next;

	free_count--;
	alloc_count++;

	return p;
}

void
alloc_block(void)
{
	int i;
	struct atom *p;

	if (block_count == MAXBLOCKS) {
		tos = 0;
		tof = 0;
		gc(); // prep for next run
		exitf("out of memory");
	}

	p = alloc_mem(BLOCKSIZE * sizeof (struct atom));

	mem[block_count++] = p;

	for (i = 0; i < BLOCKSIZE - 1; i++) {
		p[i].atomtype = FREEATOM;
		p[i].u.next = p + i + 1;
	}

	p[i].atomtype = FREEATOM;
	p[i].u.next = NULL;

	free_list = p;
	free_count = BLOCKSIZE;
}

struct atom *
alloc_vector(int nrow)
{
	struct atom *p = alloc_tensor(nrow);
	p->u.tensor->ndim = 1;
	p->u.tensor->dim[0] = nrow;
	return p;
}

struct atom *
alloc_matrix(int nrow, int ncol)
{
	struct atom *p = alloc_tensor(nrow * ncol);
	p->u.tensor->ndim = 2;
	p->u.tensor->dim[0] = nrow;
	p->u.tensor->dim[1] = ncol;
	return p;
}

struct atom *
alloc_tensor(int nelem)
{
	int i;
	struct atom *p;
	struct tensor *t;
	p = alloc_atom();
	t = alloc_mem(sizeof (struct tensor) + nelem * sizeof (struct atom *));
	p->atomtype = TENSOR;
	p->u.tensor = t;
	t->nelem = nelem;
	for (i = 0; i < nelem; i++)
		t->elem[i] = zero;
	tensor_count++;
	return p;
}

struct atom *
alloc_str(void)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = STR;
	p->u.str = NULL;
	string_count++;
	return p;
}

void *
alloc_mem(int n)
{
	void *p = malloc(n);
	if (p == NULL)
		exit(1);
	return p;
}
double
mfloat(uint32_t *p)
{
	int i, n;
	double d;
	n = MLENGTH(p);
	d = 0.0;
	for (i = 0; i < n; i++)
		d += scalbn((double) p[i], 32 * i);
	return d;
}

void
msetbit(uint32_t *x, uint32_t k)
{
	x[k / 32] |= 1 << (k % 32);
}

void
mclrbit(uint32_t *x, uint32_t k)
{
	x[k / 32] &= ~(1 << (k % 32));
}

// convert string to bignum (9 decimal digits fits in 32 bits)

uint32_t *
mscan(char *s)
{
	int i, k, len;
	uint32_t *a, *b, *t;
	a = mint(0);
	len = (int) strlen(s);
	if (len == 0)
		return a;
	k = len % 9;
	if (k == 0)
		k = 9;
	for (i = 0; i < k; i++)
		a[0] = 10 * a[0] + s[i] - '0';
	if (k == len)
		return a;
	b = mint(0);
	while (k < len) {
		b[0] = 1000000000; // 10^9
		t = mmul(a, b);
		mfree(a);
		a = t;
		b[0] = 0;
		for (i = 0; i < 9; i++)
			b[0] = 10 * b[0] + s[k++] - '0';
		t = madd(a, b);
		mfree(a);
		a = t;
	}
	mfree(b);
	return a;
}

// convert bignum to string (returned value points to static buffer)

char *
mstr(uint32_t *u)
{
	int i, k, n, r;
	static char *buf;
	static int len;

	// estimate string length

	// note that 0xffffffff -> 000000004 294967295

	// hence space for 8 leading zeroes is required

	n = 10 * MLENGTH(u) + 10;

	n = 1000 * (n / 1000 + 1);

	if (n > len) {
		if (buf)
			free(buf);
		buf = alloc_mem(n);
		len = n;
	}

	u = mcopy(u);

	k = len - 1;
	buf[k] = '\0'; // string terminator

	for (;;) {
		r = mdivby1billion(u);
		for (i = 0; i < 9; i++) {
			buf[--k] = r % 10 + '0';
			r /= 10;
		}
		if (MZERO(u))
			break;
	}

	mfree(u);

	// remove leading zeroes

	while (buf[k] == '0' && buf[k + 1])
		k++;

	return buf + k;
}

// returns remainder, quotient returned in u

int
mdivby1billion(uint32_t *u)
{
	int i;
	uint64_t r = 0;
	for (i = MLENGTH(u) - 1; i >= 0; i--) {
		r = r << 32 | u[i];
		u[i] = (uint32_t) (r / 1000000000);
		r -= (uint64_t) 1000000000 * u[i];
	}
	mnorm(u);
	return (int) r;
}

// returns u + v

uint32_t *
madd(uint32_t *u, uint32_t *v)
{
	int i, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	if (nu > nv)
		nw = nu + 1;
	else
		nw = nv + 1;
	w = mnew(nw);
	for (i = 0; i < nu; i++)
		w[i] = u[i];
	for (i = nu; i < nw; i++)
		w[i] = 0;
	t = 0;
	for (i = 0; i < nv; i++) {
		t += (uint64_t) w[i] + v[i];
		w[i] = (uint32_t) t;
		t >>= 32;
	}
	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = (uint32_t) t;
		t >>= 32;
	}
	mnorm(w);
	return w;
}

// returns u - v

uint32_t *
msub(uint32_t *u, uint32_t *v)
{
	int i, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	if (nu > nv)
		nw = nu;
	else
		nw = nv;
	w = mnew(nw);
	for (i = 0; i < nu; i++)
		w[i] = u[i];
	for (i = nu; i < nw; i++)
		w[i] = 0;
	t = 0;
	for (i = 0; i < nv; i++) {
		t += (uint64_t) w[i] - v[i];
		w[i] = (uint32_t) t;
		t = (int64_t) t >> 32; // cast to extend sign
	}
	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = (uint32_t) t;
		t = (int64_t) t >> 32; // cast to extend sign
	}
	mnorm(w);
	return w;
}

// returns u * v

uint32_t *
mmul(uint32_t *u, uint32_t *v)
{
	int i, j, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	nw = nu + nv;
	w = mnew(nw);
	for (i = 0; i < nw; i++)
		w[i] = 0;
	for (i = 0; i < nu; i++) {
		t = 0;
		for (j = 0; j < nv; j++) {
			t += (uint64_t) u[i] * v[j] + w[i + j];
			w[i + j] = (uint32_t) t;
			t >>= 32;
		}
		w[i + j] = (uint32_t) t;
	}
	mnorm(w);
	return w;
}

// returns floor(u / v)

uint32_t *
mdiv(uint32_t *u, uint32_t *v)
{
	int i, k, nu, nv;
	uint32_t *q, qhat, *w;
	uint64_t a, b, t;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(v) == 1 && v[0] == 0)
		stopf("divide by zero"); // v = 0
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	k = nu - nv;
	if (k < 0) {
		q = mnew(1);
		q[0] = 0;
		return q; // u < v, return zero
	}
	u = mcopy(u);
	q = mnew(k + 1);
	w = mnew(nv + 1);
	b = v[nv - 1];
	do {
		q[k] = 0;
		while (nu >= nv + k) {
			// estimate 32-bit partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = a << 32 | u[nu - 2];
			if (a < b)
				break;
			qhat = (uint32_t) (a / (b + 1));
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += (uint64_t) qhat * v[i];
				w[i] = (uint32_t) t;
				t >>= 32;
			}
			w[nv] = (uint32_t) t;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += (uint64_t) u[i] - w[i - k];
				u[i] = (uint32_t) t;
				t = (int64_t) t >> 32; // cast to extend sign
			}
			if (t) {
				// u is negative, restore u
				t = 0;
				for (i = k; i < nu; i++) {
					t += (uint64_t) u[i] + w[i - k];
					u[i] = (uint32_t) t;
					t >>= 32;
				}
				break;
			}
			q[k] += qhat;
			mnorm(u);
			nu = MLENGTH(u);
		}
	} while (--k >= 0);
	mnorm(q);
	mfree(u);
	mfree(w);
	return q;
}

// returns u mod v

uint32_t *
mmod(uint32_t *u, uint32_t *v)
{
	int i, k, nu, nv;
	uint32_t qhat, *w;
	uint64_t a, b, t;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(v) == 1 && v[0] == 0)
		stopf("divide by zero"); // v = 0
	u = mcopy(u);
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	k = nu - nv;
	if (k < 0)
		return u; // u < v
	w = mnew(nv + 1);
	b = v[nv - 1];
	do {
		while (nu >= nv + k) {
			// estimate 32-bit partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = a << 32 | u[nu - 2];
			if (a < b)
				break;
			qhat = (uint32_t) (a / (b + 1));
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += (uint64_t) qhat * v[i];
				w[i] = (uint32_t) t;
				t >>= 32;
			}
			w[nv] = (uint32_t) t;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += (uint64_t) u[i] - w[i - k];
				u[i] = (uint32_t) t;
				t = (int64_t) t >> 32; // cast to extend sign
			}
			if (t) {
				// u is negative, restore u
				t = 0;
				for (i = k; i < nu; i++) {
					t += (uint64_t) u[i] + w[i - k];
					u[i] = (uint32_t) t;
					t >>= 32;
				}
				break;
			}
			mnorm(u);
			nu = MLENGTH(u);
		}
	} while (--k >= 0);
	mfree(w);
	return u;
}

// returns u ** v

uint32_t *
mpow(uint32_t *u, uint32_t *v)
{
	uint32_t *t, *w;
	u = mcopy(u);
	v = mcopy(v);
	// w = 1
	w = mnew(1);
	w[0] = 1;
	for (;;) {
		if (v[0] & 1) {
			// w = w * u
			t = mmul(w, u);
			mfree(w);
			w = t;
		}
		// v = v >> 1
		mshr(v);
		// v = 0?
		if (MLENGTH(v) == 1 && v[0] == 0)
			break;
		// u = u * u
		t = mmul(u, u);
		mfree(u);
		u = t;
	}
	mfree(u);
	mfree(v);
	return w;
}

// u = u >> 1

void
mshr(uint32_t *u)
{
	int i;
	for (i = 0; i < MLENGTH(u) - 1; i++) {
		u[i] >>= 1;
		if (u[i + 1] & 1)
			u[i] |= 0x80000000;
	}
	u[i] >>= 1;
	mnorm(u);
}

// compare u and v

int
mcmp(uint32_t *u, uint32_t *v)
{
	int i;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(u) < MLENGTH(v))
		return -1;
	if (MLENGTH(u) > MLENGTH(v))
		return 1;
	for (i = MLENGTH(u) - 1; i >= 0; i--) {
		if (u[i] < v[i])
			return -1;
		if (u[i] > v[i])
			return 1;
	}
	return 0; // u = v
}

int
meq(uint32_t *u, uint32_t *v)
{
	int i;
	if (MLENGTH(u) != MLENGTH(v))
		return 0;
	for (i = 0; i < MLENGTH(u); i++)
		if (u[i] != v[i])
			return 0;
	return 1;
}

// convert unsigned to bignum

uint32_t *
mint(uint32_t n)
{
	uint32_t *p;
	p = mnew(1);
	p[0] = n;
	return p;
}

uint32_t *
mnew(int n)
{
	uint32_t *u;
	u = alloc_mem((n + 1) * sizeof (uint32_t));
	bignum_count++;
	*u = n;
	return u + 1;
}

void
mfree(uint32_t *u)
{
	free(u - 1);
	bignum_count--;
}

uint32_t *
mcopy(uint32_t *u)
{
	int i;
	uint32_t *v;
	v = mnew(MLENGTH(u));
	for (i = 0; i < MLENGTH(u); i++)
		v[i] = u[i];
	return v;
}

// remove leading zeroes

void
mnorm(uint32_t *u)
{
	while (MLENGTH(u) > 1 && u[MLENGTH(u) - 1] == 0)
		MLENGTH(u)--;
}

uint32_t *
mgcd(uint32_t *u, uint32_t *v)
{
	int i, k, n, sign;
	uint32_t *t;

	if (MZERO(u)) {
		t = mcopy(v);
		return t;
	}

	if (MZERO(v)) {
		t = mcopy(u);
		return t;
	}

	u = mcopy(u);
	v = mcopy(v);

	k = 0;

	while ((u[0] & 1) == 0 && (v[0] & 1) == 0) {
		mshr(u);
		mshr(v);
		k++;
	}

	if (u[0] & 1) {
		t = mcopy(v);
		sign = -1;
	} else {
		t = mcopy(u);
		sign = 1;
	}

	while (1) {

		while ((t[0] & 1) == 0)
			mshr(t);

		if (sign == 1) {
			mfree(u);
			u = mcopy(t);
		} else {
			mfree(v);
			v = mcopy(t);
		}

		mfree(t);

		if (mcmp(u, v) < 0) {
			t = msub(v, u);
			sign = -1;
		} else {
			t = msub(u, v);
			sign = 1;
		}

		if (MZERO(t)) {
			mfree(t);
			mfree(v);
			n = (k / 32) + 1;
			v = mnew(n);
			for (i = 0; i < n; i++)
				v[i] = 0;
			msetbit(v, k);
			t = mmul(u, v);
			mfree(u);
			mfree(v);
			return t;
		}
	}
}

// returns NULL if not perfect root, otherwise returns a^(1/n)

uint32_t *
mroot(uint32_t *a, uint32_t *n)
{
	int i, j, k;
	uint32_t *b, *c, m;

	if (MLENGTH(n) > 1 || n[0] == 0)
		return NULL;

	// k is bit length of a

	k = 32 * (MLENGTH(a) - 1);

	m = a[MLENGTH(a) - 1];

	while (m) {
		m >>= 1;
		k++;
	}

	if (k == 0)
		return mint(0);

	// initial guess of index of ms bit in result

	k = (k - 1) / n[0];

	j = k / 32 + 1; // k is bit index, not number of bits

	b = mnew(j);

	for (i = 0; i < j; i++)
		b[i] = 0;

	while (k >= 0) {
		msetbit(b, k);
		mnorm(b);
		c = mpow(b, n);
		switch (mcmp(c, a)) {
		case -1:
			break;
		case 0:
			mfree(c);
			return b;
		case 1:
			mclrbit(b, k);
			break;
		}
		mfree(c);
		k--;
	}

	mfree(b);

	return NULL;
}
// create a list from n things on the stack

void
list(int n)
{
	int i;
	push_symbol(NIL);
	for (i = 0; i < n; i++)
		cons();
}

void
cons(void)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = CONS;
	p->u.cons.cdr = pop();
	p->u.cons.car = pop();
	push(p);
}

int
lengthf(struct atom *p)
{
	int n = 0;
	while (iscons(p)) {
		n++;
		p = cdr(p);
	}
	return n;
}

// returns 1 if expr p contains expr q, otherweise returns 0

int
findf(struct atom *p, struct atom *q)
{
	int i;

	if (equal(p, q))
		return 1;

	if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (findf(p->u.tensor->elem[i], q))
				return 1;
		return 0;
	}

	while (iscons(p)) {
		if (findf(car(p), q))
			return 1;
		p = cdr(p);
	}

	return 0;
}

// sort n things on the stack

void
sort(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_func);
}

int
sort_func(const void *p1, const void *p2)
{
	return cmp(*((struct atom **) p1), *((struct atom **) p2));
}

int
cmp(struct atom *p1, struct atom *p2)
{
	int t;

	if (p1 == p2)
		return 0;

	if (p1 == symbol(NIL))
		return -1;

	if (p2 == symbol(NIL))
		return 1;

	if (isnum(p1) && isnum(p2))
		return cmp_numbers(p1, p2);

	if (isnum(p1))
		return -1;

	if (isnum(p2))
		return 1;

	if (isstr(p1) && isstr(p2))
		return strcmp(p1->u.str, p2->u.str);

	if (isstr(p1))
		return -1;

	if (isstr(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return strcmp(printname(p1), printname(p2));

	if (issymbol(p1))
		return -1;

	if (issymbol(p2))
		return 1;

	if (istensor(p1) && istensor(p2))
		return cmp_tensors(p1, p2);

	if (istensor(p1))
		return -1;

	if (istensor(p2))
		return 1;

	while (iscons(p1) && iscons(p2)) {
		t = cmp(car(p1), car(p2));
		if (t)
			return t;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

	return 0;
}

int
cmp_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2))
		return cmp_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d1 < d2)
		return -1;

	if (d1 > d2)
		return 1;

	return 0;
}

int
cmp_rationals(struct atom *a, struct atom *b)
{
	int t;
	uint32_t *ab, *ba;
	if (a->sign == MMINUS && b->sign == MPLUS)
		return -1;
	if (a->sign == MPLUS && b->sign == MMINUS)
		return 1;
	if (isinteger(a) && isinteger(b)) {
		if (a->sign == MMINUS)
			return mcmp(b->u.q.a, a->u.q.a);
		else
			return mcmp(a->u.q.a, b->u.q.a);
	}
	ab = mmul(a->u.q.a, b->u.q.b);
	ba = mmul(a->u.q.b, b->u.q.a);
	if (a->sign == MMINUS)
		t = mcmp(ba, ab);
	else
		t = mcmp(ab, ba);
	mfree(ab);
	mfree(ba);
	return t;
}

int
cmp_tensors(struct atom *p1, struct atom *p2)
{
	int i, t;

	t = p1->u.tensor->ndim - p2->u.tensor->ndim;

	if (t)
		return t;

	for (i = 0; i < p1->u.tensor->ndim; i++) {
		t = p1->u.tensor->dim[i] - p2->u.tensor->dim[i];
		if (t)
			return t;
	}

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		t = cmp(p1->u.tensor->elem[i], p2->u.tensor->elem[i]);
		if (t)
			return t;
	}

	return 0;
}

int
find_denominator(struct atom *p)
{
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
			return 1;
		p = cdr(p);
	}
	return 0;
}

int
count_denominators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isdenominator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

int
count_numerators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isnumerator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}
// automatic variables not visible to the garbage collector are reclaimed

void
evalg(void)
{
	if (gc_level == eval_level && alloc_count > MAXBLOCKS * BLOCKSIZE / 10)
		gc();
	gc_level++;
	evalf();
	gc_level--;
}

// call evalf instead of evalg to evaluate without garbage collection

void
evalf(void)
{
	struct atom *p;
	eval_level++;
	p = pop();
	fpush(p); // make visible to garbage collector
	evalf_nib(p);
	fpop();
	eval_level--;
}

void
evalf_nib(struct atom *p1)
{
	if (interrupt)
		stopf("interrupt");

	if (eval_level == 200)
		stopf("circular definition?");

	if (eval_level > max_eval_level)
		max_eval_level = eval_level;

	if (iscons(p1) && iskeyword(car(p1))) {
		expanding++;
		car(p1)->u.ksym.func(p1); // call through function pointer
		expanding--;
		return;
	}

	if (iscons(p1) && isusersymbol(car(p1))) {
		eval_user_function(p1);
		return;
	}

	if (iskeyword(p1)) { // bare keyword
		push(p1);
		push_symbol(LAST); // default arg
		list(2);
		evalg();
		return;
	}

	if (isusersymbol(p1)) {
		eval_user_symbol(p1);
		return;
	}

	if (istensor(p1)) {
		eval_tensor(p1);
		return;
	}

	push(p1); // rational, double, or string
}

// evaluate '=' as '=='

void
evalp(void)
{
	struct atom *p1;
	p1 = pop();
	if (car(p1) == symbol(SETQ)) {
		push_symbol(TESTEQ);
		push(cadr(p1));
		push(caddr(p1));
		list(3);
		p1 = pop();
	}
	push(p1);
	evalg();
}
void
eval_abs(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	absfunc();
}

void
absfunc(void)
{
	int h;
	struct atom *p1, *p2, *p3;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		if (isnegativenumber(p1))
			negate();
		return;
	}

	if (istensor(p1)) {
		if (p1->u.tensor->ndim > 1) {
			push_symbol(ABS);
			push(p1);
			list(2);
			return;
		}
		push(p1);
		push(p1);
		conjfunc();
		inner();
		push_rational(1, 2);
		power();
		return;
	}

	push(p1);
	push(p1);
	conjfunc();
	multiply();
	push_rational(1, 2);
	power();

	p2 = pop();
	push(p2);
	floatfunc();
	p3 = pop();
	if (isdouble(p3)) {
		push(p2);
		if (isnegativenumber(p3))
			negate();
		return;
	}

	// abs(1/a) evaluates to 1/abs(a)

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(p1);
		reciprocate();
		absfunc();
		reciprocate();
		return;
	}

	// abs(a*b) evaluates to abs(a)*abs(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			absfunc();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	if (isnegativeterm(p1) || (car(p1) == symbol(ADD) && isnegativeterm(cadr(p1)))) {
		push(p1);
		negate();
		p1 = pop();
	}

	push_symbol(ABS);
	push(p1);
	list(2);
}
void
eval_add(struct atom *p1)
{
	int h = tos;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
	add_terms(tos - h);
	expanding++;
}

void
add(void)
{
	add_terms(2);
}

void
add_terms(int n)
{
	int i, h;
	struct atom *p1, *T;

	if (n < 2)
		return;

	h = tos - n;

	flatten_terms(h);

	T = combine_tensors(h);

	combine_terms(h);

	if (simplify_terms(h))
		combine_terms(h);

	n = tos - h;

	if (n == 0) {
		if (istensor(T))
			push(T);
		else
			push_integer(0);
		return;
	}

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // prepend ADD to list
	}

	if (!istensor(T))
		return;

	p1 = pop();

	T = copy_tensor(T);

	n = T->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(T->u.tensor->elem[i]);
		push(p1);
		add();
		T->u.tensor->elem[i] = pop();
	}

	push(T);
}

void
flatten_terms(int h)
{
	int i, n;
	struct atom *p1;
	n = tos;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(ADD)) {
			stack[i] = cadr(p1);
			p1 = cddr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

struct atom *
combine_tensors(int h)
{
	int i, j;
	struct atom *p1, *T;
	T = symbol(NIL);
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(T)) {
				push(T);
				push(p1);
				add_tensors();
				T = pop();
			} else
				T = p1;
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			tos--;
			i--; // use same index again
		}
	}
	return T;
}

void
add_tensors(void)
{
	int i, n;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (!compatible_dimensions(p1, p2))
		stopf("incompatible tensor arithmetic");

	p1 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2->u.tensor->elem[i]);
		add();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}

void
combine_terms(int h)
{
	int i, j;
	sort_terms(tos - h);
	for (i = h; i < tos - 1; i++) {
		if (combine_terms_nib(i, i + 1)) {
			if (iszero(stack[i])) {
				for (j = i + 2; j < tos; j++)
					stack[j - 2] = stack[j]; // remove 2 terms
				tos -= 2;
			} else {
				for (j = i + 2; j < tos; j++)
					stack[j - 1] = stack[j]; // remove 1 term
				tos -= 1;
			}
			i--; // use same index again
		}
	}
	if (h < tos && iszero(stack[tos - 1]))
		tos--;
}

int
combine_terms_nib(int i, int j)
{
	int denorm;
	struct atom *coeff1, *coeff2, *p1, *p2;

	p1 = stack[i];
	p2 = stack[j];

	if (iszero(p2))
		return 1;

	if (iszero(p1)) {
		stack[i] = p2;
		return 1;
	}

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	coeff1 = one;
	coeff2 = one;

	denorm = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		denorm = 1;
		if (isnum(car(p1))) {
			coeff1 = car(p1);
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				denorm = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		if (isnum(car(p2))) {
			coeff2 = car(p2);
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL))
				p2 = car(p2);
		}
	}

	if (!equal(p1, p2))
		return 0;

	add_numbers(coeff1, coeff2);

	coeff1 = pop();

	if (iszero(coeff1)) {
		stack[i] = coeff1;
		return 1;
	}

	if (isplusone(coeff1) && !isdouble(coeff1)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend MULTIPLY
		} else
			push(p1);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend coeff1
			cons(); // prepend MULTIPLY
		} else {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}

void
sort_terms(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_terms_func);
}

int
sort_terms_func(const void *q1, const void *q2)
{
	return cmp_terms(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_terms(struct atom *p1, struct atom *p2)
{
	int a, b, c;

	// 1st level: imaginary terms on the right

	a = isimaginaryterm(p1);
	b = isimaginaryterm(p2);

	if (a == 0 && b == 1)
		return -1; // ok

	if (a == 1 && b == 0)
		return 1; // out of order

	// 2nd level: numericals on the right

	if (isnum(p1) && isnum(p2))
		return 0; // don't care about order, save time, don't compare

	if (isnum(p1))
		return 1; // out of order

	if (isnum(p2))
		return -1; // ok

	// 3rd level: sort by factors

	a = 0;
	b = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		a = 1; // p1 is a list of factors
		if (isnum(car(p1))) {
			// skip over coeff
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				a = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		b = 1; // p2 is a list of factors
		if (isnum(car(p2))) {
			// skip over coeff
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL)) {
				p2 = car(p2);
				b = 0;
			}
		}
	}

	if (a == 0 && b == 0)
		return cmp_factors(p1, p2);

	if (a == 0 && b == 1) {
		c = cmp_factors(p1, car(p2));
		if (c == 0)
			c = -1; // lengthf(p1) < lengthf(p2)
		return c;
	}

	if (a == 1 && b == 0) {
		c = cmp_factors(car(p1), p2);
		if (c == 0)
			c = 1; // lengthf(p1) > lengthf(p2)
		return c;
	}

	while (iscons(p1) && iscons(p2)) {
		c = cmp_factors(car(p1), car(p2));
		if (c)
			return c;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	return 0;
}

int
simplify_terms(int h)
{
	int i, n = 0;
	struct atom *p1, *p2;
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (isradicalterm(p1)) {
			push(p1);
			evalf();
			p2 = pop();
			if (!equal(p1, p2)) {
				stack[i] = p2;
				n++;
			}
		}
	}
	return n;
}

int
isradicalterm(struct atom *p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
}

int
isimaginaryterm(struct atom *p)
{
	if (isimaginaryfactor(p))
		return 1;
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isimaginaryfactor(car(p)))
				return 1;
			p = cdr(p);
		}
	}
	return 0;
}

int
isimaginaryfactor(struct atom *p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}

void
add_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2)) {
		add_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 + d2);
}

void
add_rationals(struct atom *p1, struct atom *p2)
{
	int sign;
	uint32_t *a, *ab, *b, *ba, *c;

	if (iszero(p1)) {
		push(p2);
		return;
	}

	if (iszero(p2)) {
		push(p1);
		return;
	}

	if (isinteger(p1) && isinteger(p2)) {
		add_integers(p1, p2);
		return;
	}

	ab = mmul(p1->u.q.a, p2->u.q.b);
	ba = mmul(p1->u.q.b, p2->u.q.a);

	if (p1->sign == p2->sign) {
		a = madd(ab, ba);
		sign = p1->sign;
	} else {
		switch (mcmp(ab, ba)) {
		case 1:
			a = msub(ab, ba);
			sign = p1->sign;
			break;
		case 0:
			push_integer(0);
			mfree(ab);
			mfree(ba);
			return;
		case -1:
			a = msub(ba, ab);
			sign = p2->sign;
			break;
		default:
			// never gets here, fix compiler warning
			return;
		}
	}

	mfree(ab);
	mfree(ba);

	b = mmul(p1->u.q.b, p2->u.q.b);
	c = mgcd(a, b);

	push_bignum(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

void
add_integers(struct atom *p1, struct atom *p2)
{
	int sign;
	uint32_t *a, *b, *c;

	a = p1->u.q.a;
	b = p2->u.q.a;

	if (p1->sign == p2->sign) {
		c = madd(a, b);
		sign = p1->sign;
	} else {
		switch (mcmp(a, b)) {
		case 1:
			c = msub(a, b);
			sign = p1->sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			c = msub(b, a);
			sign = p2->sign;
			break;
		default:
			// never gets here, fix compiler warning
			return;
		}
	}

	push_bignum(sign, c, mint(1));
}

void
subtract(void)
{
	negate();
	add();
}
void
eval_adj(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	adj();
}

void
adj(void)
{
	int col, i, j, k, n, row;
	struct atom *p1, *p2, *p3;

	p1 = pop();

	if (!istensor(p1)) {
		push_integer(1); // adj of scalar is 1 because adj = det inv
		return;
	}

	if (!issquarematrix(p1))
		stopf("adj: square matrix expected");

	n = p1->u.tensor->dim[0];

	// p2 is the adjunct matrix

	p2 = alloc_matrix(n, n);

	if (n == 2) {
		p2->u.tensor->elem[0] = p1->u.tensor->elem[3];
		push(p1->u.tensor->elem[1]);
		negate();
		p2->u.tensor->elem[1] = pop();
		push(p1->u.tensor->elem[2]);
		negate();
		p2->u.tensor->elem[2] = pop();
		p2->u.tensor->elem[3] = p1->u.tensor->elem[0];
		push(p2);
		return;
	}

	// p3 is for computing cofactors

	p3 = alloc_matrix(n - 1, n - 1);

	for (row = 0; row < n; row++) {
		for (col = 0; col < n; col++) {
			k = 0;
			for (i = 0; i < n; i++)
				for (j = 0; j < n; j++)
					if (i != row && j != col)
						p3->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
			push(p3);
			det();
			if ((row + col) % 2)
				negate();
			p2->u.tensor->elem[n * col + row] = pop(); // transpose
		}
	}

	push(p2);
}
void
eval_and(struct atom *p1)
{
	struct atom *p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (iszero(p2)) {
			push_integer(0);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(1);
}
void
eval_arccos(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	arccos();
}

void
arccos(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arccos();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1.0 <= d && d <= 1.0) {
			d = acos(d);
			push_double(d);
			return;
		}
	}

	// arccos(z) = -i log(z + i sqrt(1 - z^2))

	if (isdouble(p1) || isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		sqrtfunc();
		push(imaginaryunit);
		multiply();
		push(p1);
		add();
		logfunc();
		push(imaginaryunit);
		multiply();
		negate();
		return;
	}

	// arccos(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1 / sqrt(2)) = 3/4 pi

	if (isminusoneoversqrttwo(p1)) {
		push_rational(3, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(0) = 1/2 pi

	if (iszero(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1/2) = 1/3 pi

	if (isequalq(p1, 1 ,2)) {
		push_rational(1, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1) = 0

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	// arccos(-1/2) = 2/3 pi

	if (isequalq(p1, -1, 2)) {
		push_rational(2, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1) = pi

	if (isminusone(p1)) {
		push_symbol(PI);
		return;
	}

	push_symbol(ARCCOS);
	push(p1);
	list(2);
}
void
eval_arccosh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	arccosh();
}

void
arccosh(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arccosh();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (d >= 1.0) {
			d = acosh(d);
			push_double(d);
			return;
		}
	}

	// arccosh(z) = log(sqrt(z^2 - 1) + z)

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCCOSH);
	push(p1);
	list(2);
}
void
eval_arcsin(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	arcsin();
}

void
arcsin(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arcsin();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1.0 <= d && d <= 1.0) {
			d = asin(d);
			push_double(d);
			return;
		}
	}

	// arcsin(z) = -i log(i z + sqrt(1 - z^2))

	if (isdouble(p1) || isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(imaginaryunit);
		push(p1);
		multiply();
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		sqrtfunc();
		add();
		logfunc();
		multiply();
		return;
	}

	// arcsin(-x) = -arcsin(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arcsin();
		negate();
		return;
	}

	// arcsin(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(0) = 0

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arcsin(1/2) = 1/6 pi

	if (isequalq(p1, 1, 2)) {
		push_rational(1, 6);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(1) = 1/2 pi

	if (isplusone(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	push_symbol(ARCSIN);
	push(p1);
	list(2);
}
void
eval_arcsinh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	arcsinh();
}

void
arcsinh(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arcsinh();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = asinh(d);
		push_double(d);
		return;
	}

	// arcsinh(z) = log(sqrt(z^2 + 1) + z)

	if (isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arcsinh();
		negate();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCSINH);
	push(p1);
	list(2);
}
void
eval_arctan(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
	} else
		push_integer(1);

	arctan();
}

void
arctan(void)
{
	int i, n;
	struct atom *X, *Y, *Z;

	X = pop();
	Y = pop();

	if (istensor(Y)) {
		Y = copy_tensor(Y);
		n = Y->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(Y->u.tensor->elem[i]);
			push(X);
			arctan();
			Y->u.tensor->elem[i] = pop();
		}
		push(Y);
		return;
	}

	if (isnum(X) && isnum(Y)) {
		arctan_numbers(X, Y);
		return;
	}

	// arctan(z) = -1/2 i log((i - z) / (i + z))

	if (!iszero(X) && (isdoublez(X) || isdoublez(Y))) {
		push(Y);
		push(X);
		divide();
		Z = pop();
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(Z);
		subtract();
		push(imaginaryunit);
		push(Z);
		add();
		divide();
		logfunc();
		multiply();
		return;
	}

	// arctan(-y,x) = -arctan(y,x)

	if (isnegativeterm(Y)) {
		push(Y);
		negate();
		push(X);
		arctan();
		negate();
		return;
	}

	if (car(Y) == symbol(TAN) && isplusone(X)) {
		push(cadr(Y)); // x of tan(x)
		return;
	}

	push_symbol(ARCTAN);
	push(Y);
	push(X);
	list(3);
}

void
arctan_numbers(struct atom *X, struct atom *Y)
{
	double x, y;
	struct atom *T;

	if (iszero(X) && iszero(Y)) {
		push_symbol(ARCTAN);
		push_integer(0);
		push_integer(0);
		list(3);
		return;
	}

	if (isnum(X) && isnum(Y) && (isdouble(X) || isdouble(Y))) {
		push(X);
		x = pop_double();
		push(Y);
		y = pop_double();
		push_double(atan2(y, x));
		return;
	}

	// X and Y are rational numbers

	if (iszero(Y)) {
		if (isnegativenumber(X))
			push_symbol(PI);
		else
			push_integer(0);
		return;
	}

	if (iszero(X)) {
		if (isnegativenumber(Y))
			push_rational(-1, 2);
		else
			push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// convert fractions to integers

	push(Y);
	push(X);
	divide();
	absfunc();
	T = pop();

	push(T);
	numerator();
	if (isnegativenumber(Y))
		negate();
	Y = pop();

	push(T);
	denominator();
	if (isnegativenumber(X))
		negate();
	X = pop();

	// compare numerators and denominators, ignore signs

	if (mcmp(X->u.q.a, Y->u.q.a) != 0 || mcmp(X->u.q.b, Y->u.q.b) != 0) {
		// not equal
		if (isnegativenumber(Y)) {
			push_symbol(ARCTAN);
			push(Y);
			negate();
			push(X);
			list(3);
			negate();
		} else {
			push_symbol(ARCTAN);
			push(Y);
			push(X);
			list(3);
		}
		return;
	}

	// X = Y modulo sign

	if (isnegativenumber(X)) {
		if (isnegativenumber(Y))
			push_rational(-3, 4);
		else
			push_rational(3, 4);
	} else {
		if (isnegativenumber(Y))
			push_rational(-1, 4);
		else
			push_rational(1, 4);
	}

	push_symbol(PI);
	multiply();
}
void
eval_arctanh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	arctanh();
}

void
arctanh(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arctanh();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isplusone(p1) || isminusone(p1)) {
		push_symbol(ARCTANH);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1.0 < d && d < 1.0) {
			d = atanh(d);
			push_double(d);
			return;
		}
	}

	// arctanh(z) = 1/2 log(1 + z) - 1/2 log(1 - z)

	if (isdouble(p1) || isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		add();
		logfunc();
		push_double(1.0);
		push(p1);
		subtract();
		logfunc();
		subtract();
		push_double(0.5);
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arctanh(-x) = -arctanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arctanh();
		negate();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCTANH);
	push(p1);
	list(2);
}
void
eval_arg(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	polar(); // normalize
	arg();
}

// use numerator and denominator to handle (a + i b) / (c + i d)

// may return a denormalized angle

void
arg(void)
{
	int i, n, t;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arg();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	t = isdoublesomewhere(p1);

	push(p1);
	numerator();
	arg1();

	push(p1);
	denominator();
	arg1();

	subtract();

	if (t)
		floatfunc();
}

void
arg1(void)
{
	int h;
	struct atom *p1, *RE, *IM;

	p1 = pop();

	if (isrational(p1)) {
		if (isnegativenumber(p1)) {
			push_symbol(PI);
			negate();
		} else
			push_integer(0);
		return;
	}

	if (isdouble(p1)) {
		if (isnegativenumber(p1))
			push_double(-M_PI);
		else
			push_double(0.0);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(PI);
		push(caddr(p1));
		multiply();
		return;
	}

	// e ^ expr

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		imag();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			arg();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar and clock forms
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(IM);
		push(RE);
		arctan();
		return;
	}

	push_integer(0); // p1 is real
}
void
eval_binding(struct atom *p1)
{
	push(get_binding(cadr(p1)));
}
void
eval_ceiling(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	ceilingfunc();
}

void
ceilingfunc(void)
{
	int i, n;
	uint32_t *a, *b;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			ceilingfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	if (isrational(p1)) {
		a = mdiv(p1->u.q.a, p1->u.q.b);
		b = mint(1);
		if (isnegativenumber(p1))
			push_bignum(MMINUS, a, b);
		else {
			push_bignum(MPLUS, a, b);
			push_integer(1);
			add();
		}
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = ceil(d);
		push_double(d);
		return;
	}

	push_symbol(CEILING);
	push(p1);
	list(2);
}
void
eval_check(struct atom *p1)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		stopf("check");
	push_symbol(NIL); // no result is printed
}
void
eval_circexp(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	circexp();
}

void
circexp(void)
{
	circexp_subst();
	evalf();
}

void
circexp_subst(void)
{
	int i, h, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			circexp_subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (car(p1) == symbol(COS)) {
		push_symbol(EXPCOS);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(SIN)) {
		push_symbol(EXPSIN);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(TAN)) {
		push_symbol(EXPTAN);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push_symbol(EXPCOSH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push_symbol(EXPSINH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push_symbol(EXPTANH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	// none of the above

	if (iscons(p1)) {
		h = tos;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			circexp_subst();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	push(p1);
}
void
eval_clear(struct atom *p1)
{
	int i;

	(void) p1; // silence compiler

	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	for (i = 0; i < 27 * BUCKETSIZE; i++) {
		binding[i] = NULL;
		usrfunc[i] = NULL;
	}

	run_init_script();

	restore_symbol();
	restore_symbol();

	if (gc_level == eval_level)
		gc();

	push_symbol(NIL); // result
}
void
eval_clock(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	clockfunc();
}

void
clockfunc(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			clockfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	mag();

	push_integer(-1); // base

	push(p1);
	arg();
	push_symbol(PI);
	divide();

	power();

	multiply();
}
void
eval_cofactor(struct atom *p1)
{
	int i, j;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!issquarematrix(p2))
		stopf("cofactor: square matrix expected");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stopf("cofactor: index err");

	push(p2);

	minormatrix(i, j);

	det();

	if ((i + j) % 2)
		negate();
}
void
eval_conj(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	conjfunc();
}

void
conjfunc(void)
{
	conjfunc_subst();
	evalf();
}

void
conjfunc_subst(void)
{
	int h, i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			conjfunc_subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(POWER);
		push_integer(-1);
		push(caddr(p1));
		negate();
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = tos;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			conjfunc_subst();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	push(p1);
}
void
eval_contract(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_integer(1);
		push_integer(2);
		contract();
		return;
	}

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
		contract();
		p1 = cddr(p1);
	}
}

void
contract(void)
{
	int h, i, j, k, m, n, ncol, ndim, nelem, nrow;
	int index[MAXDIM];
	struct atom *p1, *p2, *p3;

	p3 = pop();
	p2 = pop();
	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim = p1->u.tensor->ndim;

	push(p2);
	n = pop_integer();

	push(p3);
	m = pop_integer();

	if (n < 1 || n > ndim || m < 1 || m > ndim || n == m)
		stopf("contract: index error");

	n--; // make zero based
	m--;

	ncol = p1->u.tensor->dim[n];
	nrow = p1->u.tensor->dim[m];

	if (ncol != nrow)
		stopf("contract: unequal tensor dimensions");

	// nelem is the number of elements in result

	nelem = p1->u.tensor->nelem / ncol / nrow;

	p2 = alloc_tensor(nelem);

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		for (j = 0; j < ncol; j++) {
			index[n] = j;
			index[m] = j;
			k = index[0];
			for (h = 1; h < ndim; h++)
				k = k * p1->u.tensor->dim[h] + index[h];
			push(p1->u.tensor->elem[k]);
		}

		add_terms(ncol);

		p2->u.tensor->elem[i] = pop();

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (j == n || j == m)
				continue;
			if (++index[j] < p1->u.tensor->dim[j])
				break;
			index[j] = 0;
		}
	}

	if (nelem == 1) {
		push(p2->u.tensor->elem[0]);
		return;
	}

	// add dim info

	p2->u.tensor->ndim = ndim - 2;

	k = 0;

	for (i = 0; i < ndim; i++)
		if (i != n && i != m)
			p2->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	push(p2);
}
void
eval_cos(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	cosfunc();
}

void
cosfunc(void)
{
	int i, n;
	double d;
	struct atom *p1, *p2, *X, *Y;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			cosfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = cos(d);
		push_double(d);
		return;
	}

	// cos(z) = 1/2 exp(i z) + 1/2 exp(-i z)

	if (isdoublez(p1)) {
		push_double(0.5);
		push(imaginaryunit);
		push(p1);
		multiply();
		expfunc();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		expfunc();
		add();
		multiply();
		return;
	}

	// cos(-x) = cos(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		cosfunc();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		cosfunc_sum(p1);
		return;
	}

	// cos(arctan(y,x)) = x (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(X);
		push(X);
		push(X);
		multiply();
		push(Y);
		push(Y);
		multiply();
		add();
		push_rational(-1, 2);
		power();
		multiply();
		return;
	}

	// cos(arcsin(x)) = sqrt(1 - x^2)

	if (car(p1) == symbol(ARCSIN)) {
		push_integer(1);
		push(cadr(p1));
		push_integer(2);
		power();
		subtract();
		push_rational(1, 2);
		power();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push(p2);
		d = pop_double();
		d = cos(d * M_PI);
		push_double(d);
		return;
	}

	push(p2); // nonnegative by cos(-x) = cos(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
	case 90:
	case 270:
		push_integer(0);
		break;
	case 60:
	case 300:
		push_rational(1, 2);
		break;
	case 120:
	case 240:
		push_rational(-1, 2);
		break;
	case 45:
	case 315:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 135:
	case 225:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 30:
	case 330:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 210:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 0:
		push_integer(1);
		break;
	case 180:
		push_integer(-1);
		break;
	default:
		push_symbol(COS);
		push(p1);
		list(2);
		break;
	}
}

// cos(x + n/2 pi) = cos(x) cos(n/2 pi) - sin(x) sin(n/2 pi)

void
cosfunc_sum(struct atom *p1)
{
	struct atom *p2, *p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push_integer(2);
		push(car(p2));
		multiply();
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			p3 = pop();
			push(p3);
			cosfunc();
			push(car(p2));
			cosfunc();
			multiply();
			push(p3);
			sinfunc();
			push(car(p2));
			sinfunc();
			multiply();
			subtract();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(COS);
	push(p1);
	list(2);
}
void
eval_cosh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	coshfunc();
}

void
coshfunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			coshfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = cosh(d);
		push_double(d);
		return;
	}

	// cosh(z) = 1/2 exp(z) + 1/2 exp(-z)

	if (isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		expfunc();
		push(p1);
		negate();
		expfunc();
		add();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	// cosh(-x) = cosh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		coshfunc();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(COSH);
	push(p1);
	list(2);
}
void
eval_defint(struct atom *p1)
{
	struct atom *F, *X, *A, *B;

	push(cadr(p1));
	evalf();
	F = pop();

	p1 = cddr(p1);

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		X = pop();

		push(cadr(p1));
		evalf();
		A = pop();

		push(caddr(p1));
		evalf();
		B = pop();

		push(F);
		push(X);
		integral();
		F = pop();

		push(F);
		push(X);
		push(B);
		subst();
		evalf();

		push(F);
		push(X);
		push(A);
		subst();
		evalf();

		subtract();
		F = pop();

		p1 = cdddr(p1);
	}

	push(F);
}
void
eval_denominator(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	denominator();
}

void
denominator(void)
{
	struct atom *p0, *p1, *p2;

	p1 = pop();

	if (isrational(p1)) {
		push_bignum(MPLUS, mcopy(p1->u.q.b), mint(1));
		return;
	}

	p2 = one; // denominator

	while (find_divisor(p1)) {

		p0 = pop(); // p0 is a denominator

		push(p0); // cancel in orig expr
		push(p1);
		cancel_factor();
		p1 = pop();

		push(p0); // update denominator
		push(p2);
		cancel_factor();
		p2 = pop();
	}

	push(p2);
}
void
eval_derivative(struct atom *p1)
{
	int flag, i, n;
	struct atom *X, *Y = NULL; // silence compiler

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_symbol(X_LOWER);
		derivative();
		return;
	}

	flag = 0;

	while (iscons(p1) || flag) {

		if (flag) {
			X = Y;
			flag = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			push_symbol(X_LOWER);
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				derivative();
			}
			continue;
		}

		if (iscons(p1)) {

			push(car(p1));
			evalf();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				for (i = 0; i < n; i++) {
					push(X);
					derivative();
				}
				continue;
			}

			flag = 1;
		}

		push(X);
		derivative();
	}
}

void
derivative(void)
{
	struct atom *F, *X;

	X = pop();
	F = pop();

	if (istensor(F)) {
		if (istensor(X))
			d_tensor_tensor(F, X);
		else
			d_tensor_scalar(F, X);
	} else {
		if (istensor(X))
			d_scalar_tensor(F, X);
		else
			d_scalar_scalar(F, X);
	}
}

void
d_scalar_scalar(struct atom *F, struct atom *X)
{
	if (!isusersymbol(X))
		stopf("derivative: symbol expected");

	// d(x,x)?

	if (equal(F, X)) {
		push_integer(1);
		return;
	}

	// d(a,x)?

	if (!iscons(F)) {
		push_integer(0);
		return;
	}

	if (car(F) == symbol(ADD)) {
		dsum(F, X);
		return;
	}

	if (car(F) == symbol(MULTIPLY)) {
		dproduct(F, X);
		return;
	}

	if (car(F) == symbol(POWER)) {
		dpower(F, X);
		return;
	}

	if (car(F) == symbol(DERIVATIVE)) {
		dd(F, X);
		return;
	}

	if (car(F) == symbol(LOG)) {
		dlog(F, X);
		return;
	}

	if (car(F) == symbol(SIN)) {
		dsin(F, X);
		return;
	}

	if (car(F) == symbol(COS)) {
		dcos(F, X);
		return;
	}

	if (car(F) == symbol(TAN)) {
		dtan(F, X);
		return;
	}

	if (car(F) == symbol(ARCSIN)) {
		darcsin(F, X);
		return;
	}

	if (car(F) == symbol(ARCCOS)) {
		darccos(F, X);
		return;
	}

	if (car(F) == symbol(ARCTAN)) {
		darctan(F, X);
		return;
	}

	if (car(F) == symbol(SINH)) {
		dsinh(F, X);
		return;
	}

	if (car(F) == symbol(COSH)) {
		dcosh(F, X);
		return;
	}

	if (car(F) == symbol(TANH)) {
		dtanh(F, X);
		return;
	}

	if (car(F) == symbol(ARCSINH)) {
		darcsinh(F, X);
		return;
	}

	if (car(F) == symbol(ARCCOSH)) {
		darccosh(F, X);
		return;
	}

	if (car(F) == symbol(ARCTANH)) {
		darctanh(F, X);
		return;
	}

	if (car(F) == symbol(ERF)) {
		derf(F, X);
		return;
	}

	if (car(F) == symbol(ERFC)) {
		derfc(F, X);
		return;
	}

	if (car(F) == symbol(INTEGRAL) && caddr(F) == X) {
		push(cadr(F));
		return;
	}

	dfunction(F, X);
}

void
dsum(struct atom *p1, struct atom *p2)
{
	int h = tos;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		derivative();
		p1 = cdr(p1);
	}
	add_terms(tos - h);
}

void
dproduct(struct atom *p1, struct atom *p2)
{
	int i, j, n;
	struct atom *p3;
	n = lengthf(p1) - 1;
	for (i = 0; i < n; i++) {
		p3 = cdr(p1);
		for (j = 0; j < n; j++) {
			push(car(p3));
			if (i == j) {
				push(p2);
				derivative();
			}
			p3 = cdr(p3);
		}
		multiply_factors(n);
	}
	add_terms(n);
}

//	     v
//	y = u
//
//	log y = v log u
//
//	1 dy   v du           dv
//	- -- = - -- + (log u) --
//	y dx   u dx           dx
//
//	dy    v  v du           dv
//	-- = u  (- -- + (log u) --)
//	dx       u dx           dx

void
dpower(struct atom *F, struct atom *X)
{
	if (isnum(cadr(F)) && isnum(caddr(F))) {
		push_integer(0); // irr or imag
		return;
	}

	push(caddr(F));		// v/u
	push(cadr(F));
	divide();

	push(cadr(F));		// du/dx
	push(X);
	derivative();

	multiply();

	push(cadr(F));		// log u
	logfunc();

	push(caddr(F));		// dv/dx
	push(X);
	derivative();

	multiply();

	add();

	push(F);		// u^v

	multiply();
}

void
dlog(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	divide();
}

//	derivative of derivative
//
//	example: d(d(f(x,y),y),x)
//
//	p1 = d(f(x,y),y)
//
//	p2 = x
//
//	cadr(p1) = f(x,y)
//
//	caddr(p1) = y

void
dd(struct atom *p1, struct atom *p2)
{
	struct atom *p3;

	// d(f(x,y),x)

	push(cadr(p1));
	push(p2);
	derivative();

	p3 = pop();

	if (car(p3) == symbol(DERIVATIVE)) {

		// sort dx terms

		push_symbol(DERIVATIVE);
		push_symbol(DERIVATIVE);
		push(cadr(p3));

		if (lessp(caddr(p3), caddr(p1))) {
			push(caddr(p3));
			list(3);
			push(caddr(p1));
		} else {
			push(caddr(p1));
			list(3);
			push(caddr(p3));
		}

		list(3);

	} else {
		push(p3);
		push(caddr(p1));
		derivative();
	}
}

// derivative of a generic function

void
dfunction(struct atom *p1, struct atom *p2)
{
	struct atom *p3;

	p3 = cdr(p1); // p3 is the argument list for the function

	if (p3 == symbol(NIL) || findf(p3, p2)) {
		push_symbol(DERIVATIVE);
		push(p1);
		push(p2);
		list(3);
	} else
		push_integer(0);
}

void
dsin(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosfunc();
	multiply();
}

void
dcos(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinfunc();
	multiply();
	negate();
}

void
dtan(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosfunc();
	push_integer(-2);
	power();
	multiply();
}

void
darcsin(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	push_rational(-1, 2);
	power();
	multiply();
}

void
darccos(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	push_rational(-1, 2);
	power();
	multiply();
	negate();
}

void
darctan(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	add();
	reciprocate();
	multiply();
}

void
dsinh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	coshfunc();
	multiply();
}

void
dcosh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinhfunc();
	multiply();
}

void
dtanh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	coshfunc();
	push_integer(-2);
	power();
	multiply();
}

void
darcsinh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(1);
	add();
	push_rational(-1, 2);
	power();
	multiply();
}

void
darccosh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	add();
	push_rational(-1, 2);
	power();
	multiply();
}

void
darctanh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	reciprocate();
	multiply();
}

void
derf(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	expfunc();
	push_symbol(PI);
	push_rational(-1, 2);
	power();
	multiply();
	push_integer(2);
	multiply();
	push(cadr(p1));
	push(p2);
	derivative();
	multiply();
}

void
derfc(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	expfunc();
	push_symbol(PI);
	push_rational(-1,2);
	power();
	multiply();
	push_integer(-2);
	multiply();
	push(cadr(p1));
	push(p2);
	derivative();
	multiply();
}

// gradient of tensor p1 wrt tensor p2

void
d_tensor_tensor(struct atom *p1, struct atom *p2)
{
	int i, j, k, m, n;
	struct atom *p3;

	if (p1->u.tensor->ndim + p2->u.tensor->ndim > MAXDIM)
		stopf("rank exceeds max");

	n = p1->u.tensor->nelem;
	m = p2->u.tensor->nelem;

	p3 = alloc_tensor(n * m);

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			push(p1->u.tensor->elem[i]);
			push(p2->u.tensor->elem[j]);
			derivative();
			p3->u.tensor->elem[m * i + j] = pop();
		}
	}

	// dim info

	p3->u.tensor->ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	k = 0;

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	n = p2->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}

// gradient of scalar p1 wrt tensor p2

void
d_scalar_tensor(struct atom *p1, struct atom *p2)
{
	int i, n;
	struct atom *p3;

	p3 = copy_tensor(p2);

	n = p2->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1);
		push(p2->u.tensor->elem[i]);
		derivative();
		p3->u.tensor->elem[i] = pop();
	}

	push(p3);
}

// derivative of tensor p1 wrt scalar p2

void
d_tensor_scalar(struct atom *p1, struct atom *p2)
{
	int i, n;
	struct atom *p3;

	p3 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2);
		derivative();
		p3->u.tensor->elem[i] = pop();
	}

	push(p3);
}
void
eval_det(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	det();
}

void
det(void)
{
	int h, i, j, k, m, n;
	struct atom *p1, *p2;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	if (!issquarematrix(p1))
		stopf("det: square matrix expected");

	n = p1->u.tensor->dim[0];

	switch (n) {
	case 1:
		push(p1->u.tensor->elem[0]);
		return;
	case 2:
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[3]);
		multiply();
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[2]);
		multiply();
		subtract();
		return;
	case 3:
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[4]);
		push(p1->u.tensor->elem[8]);
		multiply_factors(3);
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[5]);
		push(p1->u.tensor->elem[6]);
		multiply_factors(3);
		push(p1->u.tensor->elem[2]);
		push(p1->u.tensor->elem[3]);
		push(p1->u.tensor->elem[7]);
		multiply_factors(3);
		push_integer(-1);
		push(p1->u.tensor->elem[2]);
		push(p1->u.tensor->elem[4]);
		push(p1->u.tensor->elem[6]);
		multiply_factors(4);
		push_integer(-1);
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[3]);
		push(p1->u.tensor->elem[8]);
		multiply_factors(4);
		push_integer(-1);
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[5]);
		push(p1->u.tensor->elem[7]);
		multiply_factors(4);
		add_terms(6);
		return;
	default:
		break;
	}

	p2 = alloc_matrix(n - 1, n - 1);

	h = tos;

	for (m = 0; m < n; m++) {
		if (iszero(p1->u.tensor->elem[m]))
			continue;
		k = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
				if (j != m)
					p2->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
		push(p2);
		det();
		push(p1->u.tensor->elem[m]);
		multiply();
		if (m % 2)
			negate();
	}

	n = tos - h;

	if (n == 0)
		push_integer(0);
	else
		add_terms(n);
}
void
eval_dim(struct atom *p1)
{
	int k;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	if (!istensor(p2)) {
		push_integer(1);
		return;
	}

	if (lengthf(p1) == 2)
		k = 1;
	else {
		push(caddr(p1));
		evalf();
		k = pop_integer();
	}

	if (k < 1 || k > p2->u.tensor->ndim)
		stopf("dim 2nd arg: error");

	push_integer(p2->u.tensor->dim[k - 1]);
}
void
eval_do(struct atom *p1)
{
	push_symbol(NIL);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop(); // discard previous result
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
}
void
eval_eigenvec(struct atom *p1)
{
	int i, j, n;
	static double *D, *Q;

	push(cadr(p1));
	evalf();
	floatfunc();
	p1 = pop();

	if (!issquarematrix(p1))
		stopf("eigenvec: square matrix expected");

	n = p1->u.tensor->dim[0];

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (!isdouble(p1->u.tensor->elem[n * i + j]))
				stopf("eigenvec: numerical matrix expected");

	for (i = 0; i < n - 1; i++)
		for (j = i + 1; j < n; j++)
			if (fabs(p1->u.tensor->elem[n * i + j]->u.d - p1->u.tensor->elem[n * j + i]->u.d) > 1e-10)
				stopf("eigenvec: symmetrical matrix expected");

	if (D)
		free(D);
	if (Q)
		free(Q);

	D = alloc_mem(n * n * sizeof (double));
	Q = alloc_mem(n * n * sizeof (double));

	// initialize D

	for (i = 0; i < n; i++) {
		D[n * i + i] = p1->u.tensor->elem[n * i + i]->u.d;
		for (j = i + 1; j < n; j++) {
			D[n * i + j] = p1->u.tensor->elem[n * i + j]->u.d;
			D[n * j + i] = p1->u.tensor->elem[n * i + j]->u.d;
		}
	}

	// initialize Q

	for (i = 0; i < n; i++) {
		Q[n * i + i] = 1.0;
		for (j = i + 1; j < n; j++) {
			Q[n * i + j] = 0.0;
			Q[n * j + i] = 0.0;
		}
	}

	eigenvec(D, Q, n);

	p1 = alloc_matrix(n, n);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			push_double(Q[n * j + i]); // transpose
			p1->u.tensor->elem[n * i + j] = pop();
		}
	}

	push(p1);
}

void
eigenvec(double *D, double *Q, int n)
{
	int i;

	for (i = 0; i < 100; i++)
		if (eigenvec_step(D, Q, n) == 0)
			return;

	stopf("eigenvec: convergence error");
}

//	Example: p = 1, q = 3
//
//		c	0	s	0
//
//		0	1	0	0
//	G =
//		-s	0	c	0
//
//		0	0	0	1
//
//	The effect of multiplying G times A is...
//
//	row 1 of A    = c (row 1 of A ) + s (row 3 of A )
//	          n+1                n                 n
//
//	row 3 of A    = c (row 3 of A ) - s (row 1 of A )
//	          n+1                n                 n
//
//	In terms of components the overall effect is...
//
//	row 1 = c row 1 + s row 3
//
//		A[1,1] = c A[1,1] + s A[3,1]
//
//		A[1,2] = c A[1,2] + s A[3,2]
//
//		A[1,3] = c A[1,3] + s A[3,3]
//
//		A[1,4] = c A[1,4] + s A[3,4]
//
//	row 3 = c row 3 - s row 1
//
//		A[3,1] = c A[3,1] - s A[1,1]
//
//		A[3,2] = c A[3,2] - s A[1,2]
//
//		A[3,3] = c A[3,3] - s A[1,3]
//
//		A[3,4] = c A[3,4] - s A[1,4]
//
//	                                   T
//	The effect of multiplying A times G  is...
//
//	col 1 of A    = c (col 1 of A ) + s (col 3 of A )
//	          n+1                n                 n
//
//	col 3 of A    = c (col 3 of A ) - s (col 1 of A )
//	          n+1                n                 n
//
//	In terms of components the overall effect is...
//
//	col 1 = c col 1 + s col 3
//
//		A[1,1] = c A[1,1] + s A[1,3]
//
//		A[2,1] = c A[2,1] + s A[2,3]
//
//		A[3,1] = c A[3,1] + s A[3,3]
//
//		A[4,1] = c A[4,1] + s A[4,3]
//
//	col 3 = c col 3 - s col 1
//
//		A[1,3] = c A[1,3] - s A[1,1]
//
//		A[2,3] = c A[2,3] - s A[2,1]
//
//		A[3,3] = c A[3,3] - s A[3,1]
//
//		A[4,3] = c A[4,3] - s A[4,1]
//
//	What we want to do is just compute the upper triangle of A since we
//	know the lower triangle is identical.
//
//	In other words, we just want to update components A[i,j] where i < j.
//
//
//
//	Example: p = 2, q = 5
//
//				p			q
//
//			j=1	j=2	j=3	j=4	j=5	j=6
//
//		i=1	.	A[1,2]	.	.	A[1,5]	.
//
//	p	i=2	A[2,1]	A[2,2]	A[2,3]	A[2,4]	A[2,5]	A[2,6]
//
//		i=3	.	A[3,2]	.	.	A[3,5]	.
//
//		i=4	.	A[4,2]	.	.	A[4,5]	.
//
//	q	i=5	A[5,1]	A[5,2]	A[5,3]	A[5,4]	A[5,5]	A[5,6]
//
//		i=6	.	A[6,2]	.	.	A[6,5]	.
//
//
//
//	This is what B = GA does:
//
//	row 2 = c row 2 + s row 5
//
//		B[2,1] = c * A[2,1] + s * A[5,1]
//		B[2,2] = c * A[2,2] + s * A[5,2]
//		B[2,3] = c * A[2,3] + s * A[5,3]
//		B[2,4] = c * A[2,4] + s * A[5,4]
//		B[2,5] = c * A[2,5] + s * A[5,5]
//		B[2,6] = c * A[2,6] + s * A[5,6]
//
//	row 5 = c row 5 - s row 2
//
//		B[5,1] = c * A[5,1] + s * A[2,1]
//		B[5,2] = c * A[5,2] + s * A[2,2]
//		B[5,3] = c * A[5,3] + s * A[2,3]
//		B[5,4] = c * A[5,4] + s * A[2,4]
//		B[5,5] = c * A[5,5] + s * A[2,5]
//		B[5,6] = c * A[5,6] + s * A[2,6]
//
//	               T
//	This is what BG  does:
//
//	col 2 = c col 2 + s col 5
//
//		B[1,2] = c * A[1,2] + s * A[1,5]
//		B[2,2] = c * A[2,2] + s * A[2,5]
//		B[3,2] = c * A[3,2] + s * A[3,5]
//		B[4,2] = c * A[4,2] + s * A[4,5]
//		B[5,2] = c * A[5,2] + s * A[5,5]
//		B[6,2] = c * A[6,2] + s * A[6,5]
//
//	col 5 = c col 5 - s col 2
//
//		B[1,5] = c * A[1,5] - s * A[1,2]
//		B[2,5] = c * A[2,5] - s * A[2,2]
//		B[3,5] = c * A[3,5] - s * A[3,2]
//		B[4,5] = c * A[4,5] - s * A[4,2]
//		B[5,5] = c * A[5,5] - s * A[5,2]
//		B[6,5] = c * A[6,5] - s * A[6,2]
//
//
//
//	Step 1: Just do upper triangle (i < j), B[2,5] = 0
//
//		B[1,2] = c * A[1,2] + s * A[1,5]
//
//		B[2,3] = c * A[2,3] + s * A[5,3]
//		B[2,4] = c * A[2,4] + s * A[5,4]
//		B[2,6] = c * A[2,6] + s * A[5,6]
//
//		B[1,5] = c * A[1,5] - s * A[1,2]
//		B[3,5] = c * A[3,5] - s * A[3,2]
//		B[4,5] = c * A[4,5] - s * A[4,2]
//
//		B[5,6] = c * A[5,6] + s * A[2,6]
//
//
//
//	Step 2: Transpose where i > j since A[i,j] == A[j,i]
//
//		B[1,2] = c * A[1,2] + s * A[1,5]
//
//		B[2,3] = c * A[2,3] + s * A[3,5]
//		B[2,4] = c * A[2,4] + s * A[4,5]
//		B[2,6] = c * A[2,6] + s * A[5,6]
//
//		B[1,5] = c * A[1,5] - s * A[1,2]
//		B[3,5] = c * A[3,5] - s * A[2,3]
//		B[4,5] = c * A[4,5] - s * A[2,4]
//
//		B[5,6] = c * A[5,6] + s * A[2,6]
//
//
//
//	Step 3: Same as above except reorder
//
//	k < p		(k = 1)
//
//		A[1,2] = c * A[1,2] + s * A[1,5]
//		A[1,5] = c * A[1,5] - s * A[1,2]
//
//	p < k < q	(k = 3..4)
//
//		A[2,3] = c * A[2,3] + s * A[3,5]
//		A[3,5] = c * A[3,5] - s * A[2,3]
//
//		A[2,4] = c * A[2,4] + s * A[4,5]
//		A[4,5] = c * A[4,5] - s * A[2,4]
//
//	q < k		(k = 6)
//
//		A[2,6] = c * A[2,6] + s * A[5,6]
//		A[5,6] = c * A[5,6] - s * A[2,6]

int
eigenvec_step(double *D, double *Q, int n)
{
	int count, i, j;

	count = 0;

	// for each upper triangle "off-diagonal" component do step_nib

	for (i = 0; i < n - 1; i++) {
		for (j = i + 1; j < n; j++) {
			if (D[n * i + j] != 0.0) {
				eigenvec_step_nib(D, Q, n, i, j);
				count++;
			}
		}
	}

	return count;
}

void
eigenvec_step_nib(double *D, double *Q, int n, int p, int q)
{
	int k;
	double t, theta;
	double c, cc, s, ss;

	// compute c and s

	// from Numerical Recipes (except they have a_qq - a_pp)

	theta = 0.5 * (D[n * p + p] - D[n * q + q]) / D[n * p + q];

	t = 1.0 / (fabs(theta) + sqrt(theta * theta + 1.0));

	if (theta < 0.0)
		t = -t;

	c = 1.0 / sqrt(t * t + 1.0);

	s = t * c;

	// D = GD

	// which means "add rows"

	for (k = 0; k < n; k++) {
		cc = D[n * p + k];
		ss = D[n * q + k];
		D[n * p + k] = c * cc + s * ss;
		D[n * q + k] = c * ss - s * cc;
	}

	// D = D transpose(G)

	// which means "add columns"

	for (k = 0; k < n; k++) {
		cc = D[n * k + p];
		ss = D[n * k + q];
		D[n * k + p] = c * cc + s * ss;
		D[n * k + q] = c * ss - s * cc;
	}

	// Q = GQ

	// which means "add rows"

	for (k = 0; k < n; k++) {
		cc = Q[n * p + k];
		ss = Q[n * q + k];
		Q[n * p + k] = c * cc + s * ss;
		Q[n * q + k] = c * ss - s * cc;
	}

	D[n * p + q] = 0.0;
	D[n * q + p] = 0.0;
}
void
eval_erf(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	erffunc();
}

void
erffunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			erffunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = erf(d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
		return;
	}

	push_symbol(ERF);
	push(p1);
	list(2);
}
void
eval_erfc(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	erfcfunc();
}

void
erfcfunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			erfcfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = erfc(d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}
void
eval_eval(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
		asubst();
		p1 = cddr(p1);
	}
}

// arithmetic subst

void
asubst(void)
{
	int h, i, n;
	struct atom *p1, *p2, *p3;

	p3 = pop(); // new expr
	p2 = pop(); // old expr
	p1 = pop(); // expr

	if (p2 == symbol(NIL) || p3 == symbol(NIL)) {
		push(p1);
		return;
	}

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			push(p2);
			push(p3);
			asubst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (equal(p1, p2)) {
		push(p3);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	// depth first

	h = tos;

	push(car(p1)); // func name
	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		push(p3);
		asubst();
		p1 = cdr(p1);
	}

	list(tos - h);

	evalf(); // normalize

	p1 = pop();

	if (car(p1) == symbol(ADD) && car(p2) == symbol(ADD) && acmp(p1, p2)) {
		push(p1);
		push(p2);
		subtract();
		push(p3);
		add();
		return;
	}

	if (car(p1) == symbol(MULTIPLY) && car(p2) == symbol(MULTIPLY) && acmp(p1, p2)) {
		push(p1);
		push(p2);
		divide();
		push(p3);
		multiply();
		return;
	}

	push(p1);
}

int
acmp(struct atom *p1, struct atom *p2)
{
	while (iscons(p1) && iscons(p2)) {
		if (equal(car(p1), car(p2)))
			p2 = cdr(p2);
		p1 = cdr(p1);
	}
	if (iscons(p2))
		return 0;
	else
		return 1;
}
void
eval_exp(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expfunc();
}

void
expfunc(void)
{
	push_symbol(EXP1);
	swap();
	power();
}
void
eval_expcos(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expcos();
}

void
expcos(void)
{
	struct atom *p1;
	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	add();
}
void
eval_expcosh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expcosh();
}

void
expcosh(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	add();
	push_rational(1, 2);
	multiply();
}
void
eval_expsin(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expsin();
}

void
expsin(void)
{
	struct atom *p1;
	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();
}
void
eval_expsinh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expsinh();
}

void
expsinh(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	subtract();
	push_rational(1, 2);
	multiply();
}
// tan(z) = (i - i exp(2 i z)) / (exp(2 i z) + 1)

void
eval_exptan(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	exptan();
}

void
exptan(void)
{
	struct atom *p1;

	push_integer(2);
	push(imaginaryunit);
	multiply_factors(3);
	expfunc();

	p1 = pop();

	push(imaginaryunit);
	push(imaginaryunit);
	push(p1);
	multiply();
	subtract();

	push(p1);
	push_integer(1);
	add();

	divide();
}
void
eval_exptanh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	exptanh();
}

void
exptanh(void)
{
	struct atom *p1;
	push_integer(2);
	multiply();
	expfunc();
	p1 = pop();
	push(p1);
	push_integer(1);
	subtract();
	push(p1);
	push_integer(1);
	add();
	divide();
}
void
eval_factorial(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	factorial();
}

void
factorial(void)
{
	int i, n;
	double m;
	struct atom *p1;

	p1 = pop();

	if (isposint(p1)) {
		push(p1);
		n = pop_integer();
		push_integer(1);
		for (i = 2; i <= n; i++) {
			push_integer(i);
			multiply();
		}
		return;
	}

	if (isdouble(p1) && p1->u.d >= 0 && floor(p1->u.d) == p1->u.d) {
		push(p1);
		n = pop_integer();
		m = 1.0;
		for (i = 2; i <= n; i++)
			m *= i;
		push_double(m);
		return;
	}

	push_symbol(FACTORIAL);
	push(p1);
	list(2);
}
void
eval_float(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	floatfunc();
}

void
floatfunc(void)
{
	floatfunc_subst();
	evalf();
	floatfunc_subst(); // in case pi popped up
	evalf();
}

void
floatfunc_subst(void)
{
	int h, i, n;
	double a, b;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			floatfunc_subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (p1 == symbol(PI)) {
		push_double(M_PI);
		return;
	}

	if (p1 == symbol(EXP1)) {
		push_double(M_E);
		return;
	}

	if (isrational(p1)) {
		a = mfloat(p1->u.q.a);
		b = mfloat(p1->u.q.b);
		if (isnegativenumber(p1))
			a = -a;
		push_double(a / b);
		return;
	}

	// don't float exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push_symbol(POWER);
		push_symbol(EXP1);
		push(caddr(p1));
		floatfunc_subst();
		list(3);
		return;
	}

	// don't float imaginary unit, but multiply it by 1.0

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(MULTIPLY);
		push_double(1.0);
		push_symbol(POWER);
		push(cadr(p1));
		push(caddr(p1));
		floatfunc_subst();
		list(3);
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = tos;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			floatfunc_subst();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	push(p1);
}
void
eval_floor(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	floorfunc();
}

void
floorfunc(void)
{
	int i, n;
	uint32_t *a, *b;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			floorfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	if (isrational(p1)) {
		a = mdiv(p1->u.q.a, p1->u.q.b);
		b = mint(1);
		if (isnegativenumber(p1)) {
			push_bignum(MMINUS, a, b);
			push_integer(-1);
			add();
		} else
			push_bignum(MPLUS, a, b);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = floor(d);
		push_double(d);
		return;
	}

	push_symbol(FLOOR);
	push(p1);
	list(2);
}
void
eval_for(struct atom *p1)
{
	int j, k;
	struct atom *p2, *p3;

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("for: index symbol err");

	push(caddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("for: index range err");
	push(p3);
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("for: index range err");
	push(p3);
	k = pop_integer();

	p1 = cddddr(p1);

	save_symbol(p2);

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		p3 = p1;
		while (iscons(p3)) {
			push(car(p3));
			evalg();
			pop(); // discard return value
			p3 = cdr(p3);
		}
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
	}

	restore_symbol();

	push_symbol(NIL); // return value
}
void
eval_hadamard(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		hadamard();
		p1 = cdr(p1);
	}
}

void
hadamard(void)
{
	int i, n;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim != p2->u.tensor->ndim)
		stopf("hadamard");

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i])
			stopf("hadamard");

	p1 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2->u.tensor->elem[i]);
		multiply();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
void
eval_imag(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	imag();
}

void
imag(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			imag();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rect();
	p1 = pop();
	push_rational(-1, 2);
	push(imaginaryunit);
	push(p1);
	push(p1);
	conjfunc();
	subtract();
	multiply_factors(3);
}
void
eval_index(struct atom *p1)
{
	int h, n;
	struct atom *T;

	T = cadr(p1);

	p1 = cddr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	// try to optimize by indexing before eval

	if (isusersymbol(T)) {
		p1 = get_binding(T);
		n = tos - h;
		if (istensor(p1) && n <= p1->u.tensor->ndim) {
			T = p1;
			indexfunc(T, h);
			evalf();
			return;
		}
	}

	push(T);
	evalf();
	T = pop();

	if (!istensor(T)) {
		tos = h; // pop all
		push(T); // quirky, but EVA2.txt depends on it
		return;
	}

	indexfunc(T, h);
}

void
indexfunc(struct atom *T, int h)
{
	int i, k, m, n, r, t, w;
	struct atom *p1;

	m = T->u.tensor->ndim;

	n = tos - h;

	r = m - n; // rank of result

	if (r < 0)
		stopf("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > T->u.tensor->dim[i])
			stopf("index error");
		k = k * T->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (r == 0) {
		push(T->u.tensor->elem[k]); // scalar result
		return;
	}

	w = 1;

	for (i = n; i < m; i++)
		w *= T->u.tensor->dim[i];

	k *= w;

	p1 = alloc_tensor(w);

	for (i = 0; i < w; i++)
		p1->u.tensor->elem[i] = T->u.tensor->elem[k + i];

	p1->u.tensor->ndim = r;

	for (i = 0; i < r; i++)
		p1->u.tensor->dim[i] = T->u.tensor->dim[n + i];

	push(p1);
}
void
eval_infixform(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	outbuf_init();
	infixform_expr(p1);

	push_string(outbuf);
}

// for tty mode and debugging

void
print_infixform(struct atom *p)
{
	outbuf_init();
	infixform_expr(p);
	outbuf_puts("\n");
	printbuf(outbuf, BLACK);
}

void
infixform_subexpr(struct atom *p)
{
	outbuf_puts("(");
	infixform_expr(p);
	outbuf_puts(")");
}

void
infixform_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		outbuf_puts("-");
	if (car(p) == symbol(ADD))
		infixform_expr_nib(p);
	else
		infixform_term(p);
}

void
infixform_expr_nib(struct atom *p)
{
	infixform_term(cadr(p));
	p = cddr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			outbuf_puts(" - ");
		else
			outbuf_puts(" + ");
		infixform_term(car(p));
		p = cdr(p);
	}
}

void
infixform_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY))
		infixform_term_nib(p);
	else
		infixform_factor(p);
}

void
infixform_term_nib(struct atom *p)
{
	if (find_denominator(p)) {
		infixform_numerators(p);
		outbuf_puts(" / ");
		infixform_denominators(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)))
		p = cdr(p); // sign already emitted

	infixform_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		outbuf_puts(" "); // space in between factors
		infixform_factor(car(p));
		p = cdr(p);
	}
}

void
infixform_numerators(struct atom *p)
{
	int k;
	char *s;
	struct atom *q;

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (++k > 1)
			outbuf_puts(" "); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			outbuf_puts(s);
			continue;
		}

		infixform_factor(q);
	}

	if (k == 0)
		outbuf_puts("1");
}

void
infixform_denominators(struct atom *p)
{
	int k, n;
	char *s;
	struct atom *q;

	n = count_denominators(p);

	if (n > 1)
		outbuf_puts("(");

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (++k > 1)
			outbuf_puts(" "); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			outbuf_puts(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			infixform_factor(q);
		} else {
			infixform_base(cadr(q));
			outbuf_puts("^");
			infixform_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	if (n > 1)
		outbuf_puts(")");
}

void
infixform_factor(struct atom *p)
{
	if (isrational(p)) {
		infixform_rational(p);
		return;
	}

	if (isdouble(p)) {
		infixform_double(p);
		return;
	}

	if (issymbol(p)) {
		if (p == symbol(EXP1))
			outbuf_puts("exp(1)");
		else
			outbuf_puts(printname(p));
		return;
	}

	if (isstr(p)) {
		outbuf_puts(p->u.str);
		return;
	}

	if (istensor(p)) {
		infixform_tensor(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		infixform_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		infixform_power(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		infixform_factorial(p);
		return;
	}

	if (car(p) == symbol(INDEX)) {
		infixform_index(p);
		return;
	}

	// use d if for derivative if d not defined

	if (car(p) == symbol(DERIVATIVE) && get_usrfunc(symbol(D_LOWER)) == symbol(NIL)) {
		outbuf_puts("d");
		infixform_arglist(p);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		infixform_expr(cadr(p));
		outbuf_puts(" = ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		infixform_expr(cadr(p));
		outbuf_puts(" == ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		infixform_expr(cadr(p));
		outbuf_puts(" >= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		infixform_expr(cadr(p));
		outbuf_puts(" > ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		infixform_expr(cadr(p));
		outbuf_puts(" <= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		infixform_expr(cadr(p));
		outbuf_puts(" < ");
		infixform_expr(caddr(p));
		return;
	}

	// other function

	if (iscons(p)) {
		infixform_base(car(p));
		infixform_arglist(p);
		return;
	}

	outbuf_puts(" ? ");
}

void
infixform_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		outbuf_puts("exp(");
		infixform_expr(caddr(p));
		outbuf_puts(")");
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(J_LOWER)))) {
			outbuf_puts("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(I_LOWER)))) {
			outbuf_puts("i");
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		infixform_reciprocal(p);
		return;
	}

	infixform_base(cadr(p));

	outbuf_puts("^");

	p = caddr(p); // p now points to exponent

	if (isnum(p))
		infixform_numeric_exponent(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

// p = y^x where x is a negative number

void
infixform_reciprocal(struct atom *p)
{
	outbuf_puts("1 / "); // numerator
	if (isminusone(caddr(p))) {
		p = cadr(p);
		infixform_factor(p);
	} else {
		infixform_base(cadr(p));
		outbuf_puts("^");
		infixform_numeric_exponent(caddr(p)); // sign is not emitted
	}
}

void
infixform_factorial(struct atom *p)
{
	infixform_base(cadr(p));
	outbuf_puts("!");
}

void
infixform_index(struct atom *p)
{
	infixform_base(cadr(p));
	outbuf_puts("[");
	p = cddr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	outbuf_puts("]");
}

void
infixform_arglist(struct atom *p)
{
	outbuf_puts("(");
	p = cdr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	outbuf_puts(")");
}

// sign is not emitted

void
infixform_rational(struct atom *p)
{
	char *s;

	s = mstr(p->u.q.a);
	outbuf_puts(s);

	s = mstr(p->u.q.b);

	if (strcmp(s, "1") == 0)
		return;

	outbuf_puts("/");

	outbuf_puts(s);
}

// sign is not emitted

void
infixform_double(struct atom *p)
{
	char *s;

	snprintf(strbuf, STRBUFLEN, "%g", fabs(p->u.d));

	s = strbuf;

	while (*s && *s != 'E' && *s != 'e')
		outbuf_putc(*s++);

	if (!*s)
		return;

	s++;

	outbuf_puts(" 10^");

	if (*s == '-') {
		outbuf_puts("(-");
		s++;
		while (*s == '0')
			s++; // skip leading zeroes
		outbuf_puts(s);
		outbuf_puts(")");
	} else {
		if (*s == '+')
			s++;
		while (*s == '0')
			s++; // skip leading zeroes
		outbuf_puts(s);
	}
}

void
infixform_base(struct atom *p)
{
	if (isnum(p))
		infixform_numeric_base(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

void
infixform_numeric_base(struct atom *p)
{
	if (isposint(p))
		infixform_rational(p);
	else
		infixform_subexpr(p);
}

// sign is not emitted

void
infixform_numeric_exponent(struct atom *p)
{
	if (isdouble(p)) {
		outbuf_puts("(");
		infixform_double(p);
		outbuf_puts(")");
		return;
	}

	if (isinteger(p)) {
		infixform_rational(p);
		return;
	}

	outbuf_puts("(");
	infixform_rational(p);
	outbuf_puts(")");
}

void
infixform_tensor(struct atom *p)
{
	infixform_tensor_nib(p, 0, 0);
}

void
infixform_tensor_nib(struct atom *p, int d, int k)
{
	int i, n, span;

	if (d == p->u.tensor->ndim) {
		infixform_expr(p->u.tensor->elem[k]);
		return;
	}

	span = 1;

	n = p->u.tensor->ndim;

	for (i = d + 1; i < n; i++)
		span *= p->u.tensor->dim[i];

	outbuf_puts("(");

	n = p->u.tensor->dim[d];

	for (i = 0; i < n; i++) {

		infixform_tensor_nib(p, d + 1, k);

		if (i < n - 1)
			outbuf_puts(",");

		k += span;
	}

	outbuf_puts(")");
}
void
eval_inner(struct atom *p1)
{
	int h = tos;

	// evaluate from right to left

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		p1 = cdr(p1);
	}

	if (h == tos)
		stopf("inner: no args");

	evalg();

	while (tos - h > 1) {
		swap();
		evalg();
		swap();
		inner();
	}
}

void
inner(void)
{
	int i, j, k, n, mcol, mrow, ncol, ndim, nrow;
	struct atom *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		p2 = copy_tensor(p2);
		n = p2->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2->u.tensor->elem[i]);
			multiply();
			p2->u.tensor->elem[i] = pop();
		}
		push(p2);
		return;
	}

	k = p1->u.tensor->ndim - 1;

	ncol = p1->u.tensor->dim[k];
	mrow = p2->u.tensor->dim[0];

	if (ncol != mrow)
		stopf("inner: dimension err");

	ndim = p1->u.tensor->ndim + p2->u.tensor->ndim - 2;

	if (ndim > MAXDIM)
		stopf("inner: rank exceeds max");

	//	nrow is the number of rows in p1
	//
	//	mcol is the number of columns p2
	//
	//	Example:
	//
	//	A[3][3][4] B[4][4][3]
	//
	//	  3  3				nrow = 3 * 3 = 9
	//
	//	                4  3		mcol = 4 * 3 = 12

	nrow = p1->u.tensor->nelem / ncol;
	mcol = p2->u.tensor->nelem / mrow;

	p3 = alloc_tensor(nrow * mcol);

	for (i = 0; i < nrow; i++) {
		for (j = 0; j < mcol; j++) {
			for (k = 0; k < ncol; k++) {
				push(p1->u.tensor->elem[i * ncol + k]);
				push(p2->u.tensor->elem[k * mcol + j]);
				multiply();
			}
			add_terms(ncol);
			p3->u.tensor->elem[i * mcol + j] = pop();
		}
	}

	if (ndim == 0) {
		push(p3->u.tensor->elem[0]); // scalar result
		return;
	}

	// dim info

	p3->u.tensor->ndim = ndim;

	k = 0;

	n = p1->u.tensor->ndim - 1;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	n = p2->u.tensor->ndim;

	for (i = 1; i < n; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
char *integral_tab_exp[] = {

// x^n exp(a x + b)

	"exp(a x)",
	"exp(a x) / a",
	"1",

	"exp(a x + b)",
	"exp(a x + b) / a",
	"1",

	"x exp(a x)",
	"exp(a x) (a x - 1) / (a^2)",
	"1",

	"x exp(a x + b)",
	"exp(a x + b) (a x - 1) / (a^2)",
	"1",

	"x^2 exp(a x)",
	"exp(a x) (a^2 x^2 - 2 a x + 2) / (a^3)",
	"1",

	"x^2 exp(a x + b)",
	"exp(a x + b) (a^2 x^2 - 2 a x + 2) / (a^3)",
	"1",

	"x^3 exp(a x)",
	"(a^3 x^3 - 3 a^2 x^2 + 6 a x - 6) exp(a x) / a^4",
	"1",

	"x^3 exp(a x + b)",
	"(a^3 x^3 - 3 a^2 x^2 + 6 a x - 6) exp(a x + b) / a^4",
	"1",

	"x^4 exp(a x)",
	"((a^4*x^4-4*a^3*x^3+12*a^2*x^2-24*a*x+24)*exp(a*x))/a^5",
	"1",

	"x^4 exp(a x + b)",
	"((a^4*x^4-4*a^3*x^3+12*a^2*x^2-24*a*x+24)*exp(a*x+b))/a^5",
	"1",

	"x^5 exp(a x)",
	"((a^5*x^5-5*a^4*x^4+20*a^3*x^3-60*a^2*x^2+120*a*x-120)*exp(a*x))/a^6",
	"1",

	"x^5 exp(a x + b)",
	"((a^5*x^5-5*a^4*x^4+20*a^3*x^3-60*a^2*x^2+120*a*x-120)*exp(a*x+b))/a^6",
	"1",

	"x^6 exp(a x)",
	"((a^6*x^6-6*a^5*x^5+30*a^4*x^4-120*a^3*x^3+360*a^2*x^2-720*a*x+720)*exp(a*x))/a^7",
	"1",

	"x^6 exp(a x + b)",
	"((a^6*x^6-6*a^5*x^5+30*a^4*x^4-120*a^3*x^3+360*a^2*x^2-720*a*x+720)*exp(a*x+b))/a^7",
	"1",

	"x^7 exp(a x)",
	"((a^7*x^7-7*a^6*x^6+42*a^5*x^5-210*a^4*x^4+840*a^3*x^3-2520*a^2*x^2+5040*a*x-5040)*exp(a*x))/a^8",
	"1",

	"x^7 exp(a x + b)",
	"((a^7*x^7-7*a^6*x^6+42*a^5*x^5-210*a^4*x^4+840*a^3*x^3-2520*a^2*x^2+5040*a*x-5040)*exp(a*x+b))/a^8",
	"1",

	"x^8 exp(a x)",
	"((a^8*x^8-8*a^7*x^7+56*a^6*x^6-336*a^5*x^5+1680*a^4*x^4-6720*a^3*x^3+20160*a^2*x^2-40320*a*x+40320)*exp(a*x))/a^9",
	"1",

	"x^8 exp(a x + b)",
	"((a^8*x^8-8*a^7*x^7+56*a^6*x^6-336*a^5*x^5+1680*a^4*x^4-6720*a^3*x^3+20160*a^2*x^2-40320*a*x+40320)*exp(a*x+b))/a^9",
	"1",

	"x^9 exp(a x)",
	"x^9 exp(a x) / a - 9 x^8 exp(a x) / a^2 + 72 x^7 exp(a x) / a^3 - 504 x^6 exp(a x) / a^4 + 3024 x^5 exp(a x) / a^5 - 15120 x^4 exp(a x) / a^6 + 60480 x^3 exp(a x) / a^7 - 181440 x^2 exp(a x) / a^8 + 362880 x exp(a x) / a^9 - 362880 exp(a x) / a^10",
	"1",

	"x^9 exp(a x + b)",
	"x^9 exp(a x + b) / a - 9 x^8 exp(a x + b) / a^2 + 72 x^7 exp(a x + b) / a^3 - 504 x^6 exp(a x + b) / a^4 + 3024 x^5 exp(a x + b) / a^5 - 15120 x^4 exp(a x + b) / a^6 + 60480 x^3 exp(a x + b) / a^7 - 181440 x^2 exp(a x + b) / a^8 + 362880 x exp(a x + b) / a^9 - 362880 exp(a x + b) / a^10",
	"1",

	"x^10 exp(a x)",
	"x^10 exp(a x) / a - 10 x^9 exp(a x) / a^2 + 90 x^8 exp(a x) / a^3 - 720 x^7 exp(a x) / a^4 + 5040 x^6 exp(a x) / a^5 - 30240 x^5 exp(a x) / a^6 + 151200 x^4 exp(a x) / a^7 - 604800 x^3 exp(a x) / a^8 + 1814400 x^2 exp(a x) / a^9 - 3628800 x exp(a x) / a^10 + 3628800 exp(a x) / a^11",
	"1",

	"x^10 exp(a x + b)",
	"x^10 exp(a x + b) / a - 10 x^9 exp(a x + b) / a^2 + 90 x^8 exp(a x + b) / a^3 - 720 x^7 exp(a x + b) / a^4 + 5040 x^6 exp(a x + b) / a^5 - 30240 x^5 exp(a x + b) / a^6 + 151200 x^4 exp(a x + b) / a^7 - 604800 x^3 exp(a x + b) / a^8 + 1814400 x^2 exp(a x + b) / a^9 - 3628800 x exp(a x + b) / a^10 + 3628800 exp(a x + b) / a^11",
	"1",

	"x^11 exp(a x)",
	"x^11 exp(a x) / a - 11 x^10 exp(a x) / a^2 + 110 x^9 exp(a x) / a^3 - 990 x^8 exp(a x) / a^4 + 7920 x^7 exp(a x) / a^5 - 55440 x^6 exp(a x) / a^6 + 332640 x^5 exp(a x) / a^7 - 1663200 x^4 exp(a x) / a^8 + 6652800 x^3 exp(a x) / a^9 - 19958400 x^2 exp(a x) / a^10 + 39916800 x exp(a x) / a^11 - 39916800 exp(a x) / a^12",
	"1",

	"x^11 exp(a x + b)",
	"x^11 exp(a x + b) / a - 11 x^10 exp(a x + b) / a^2 + 110 x^9 exp(a x + b) / a^3 - 990 x^8 exp(a x + b) / a^4 + 7920 x^7 exp(a x + b) / a^5 - 55440 x^6 exp(a x + b) / a^6 + 332640 x^5 exp(a x + b) / a^7 - 1663200 x^4 exp(a x + b) / a^8 + 6652800 x^3 exp(a x + b) / a^9 - 19958400 x^2 exp(a x + b) / a^10 + 39916800 x exp(a x + b) / a^11 - 39916800 exp(a x + b) / a^12",
	"1",

// sin exp

	"sin(x) exp(a x)",
	"a sin(x) exp(a x) / (a^2 + 1) - cos(x) exp(a x) / (a^2 + 1)",
	"a^2 + 1", // denominator not zero

	"sin(x) exp(a x + b)",
	"a sin(x) exp(a x + b) / (a^2 + 1) - cos(x) exp(a x + b) / (a^2 + 1)",
	"a^2 + 1", // denominator not zero

	"sin(x) exp(i x)",
	"-1/4 exp(2 i x) + 1/2 i x",
	"1",

	"sin(x) exp(i x + b)",
	"-1/4 exp(b + 2 i x) + 1/2 i x exp(b)",
	"1",

	"sin(x) exp(-i x)",
	"-1/4 exp(-2 i x) - 1/2 i x",
	"1",

	"sin(x) exp(-i x + b)",
	"-1/4 exp(b - 2 i x) - 1/2 i x exp(b)",
	"1",

// cos exp

	"cos(x) exp(a x)",
	"a cos(x) exp(a x) / (a^2 + 1) + sin(x) exp(a x) / (a^2 + 1)",
	"a^2 + 1", // denominator not zero

	"cos(x) exp(a x + b)",
	"a cos(x) exp(a x + b) / (a^2 + 1) + sin(x) exp(a x + b) / (a^2 + 1)",
	"a^2 + 1", // denominator not zero

	"cos(x) exp(i x)",
	"1/2 x - 1/4 i exp(2 i x)",
	"1",

	"cos(x) exp(i x + b)",
	"1/2 x exp(b) - 1/4 i exp(b + 2 i x)",
	"1",

	"cos(x) exp(-i x)",
	"1/2 x + 1/4 i exp(-2 i x)",
	"1",

	"cos(x) exp(-i x + b)",
	"1/2 x exp(b) + 1/4 i exp(b - 2 i x)",
	"1",

// sin cos exp

	"sin(x) cos(x) exp(a x)",
	"a sin(2 x) exp(a x) / (2 (a^2 + 4)) - cos(2 x) exp(a x) / (a^2 + 4)",
	"a^2 + 4", // denominator not zero

// x^n exp(a x^2 + b)

	"exp(a x^2)",
	"-1/2 i sqrt(pi) erf(i sqrt(a) x) / sqrt(a)",
	"1",

	"exp(a x^2 + b)",
	"-1/2 i sqrt(pi) exp(b) erf(i sqrt(a) x) / sqrt(a)",
	"1",

	"x exp(a x^2)",
	"1/2 exp(a x^2) / a",
	"1",

	"x exp(a x^2 + b)",
	"1/2 exp(a x^2 + b) / a",
	"1",

	"x^2 exp(a x^2)",
	"1/2 x exp(a x^2) / a + 1/4 i sqrt(pi) erf(i sqrt(a) x) / a^(3/2)",
	"1",

	"x^2 exp(a x^2 + b)",
	"1/2 x exp(a x^2 + b) / a + 1/4 i sqrt(pi) exp(b) erf(i sqrt(a) x) / a^(3/2)",
	"1",

	"x^3 exp(a x^2)",
	"1/2 exp(a x^2) (x^2 / a - 1 / a^2)",
	"1",

	"x^3 exp(a x^2 + b)",
	"1/2 exp(a x^2) exp(b) (x^2 / a - 1 / a^2)",
	"1",

	"x^4 exp(a x^2)",
	"x^3 exp(a x^2) / (2 a) - 3 x exp(a x^2) / (4 a^2) - 3 i pi^(1/2) erf(i a^(1/2) x) / (8 a^(5/2))",
	"1",

	"x^4 exp(a x^2 + b)",
	"x^3 exp(a x^2 + b) / (2 a) - 3 x exp(a x^2 + b) / (4 a^2) - 3 i pi^(1/2) erf(i a^(1/2) x) exp(b) / (8 a^(5/2))",
	"1",

	"x^5 exp(a x^2)",
	"x^4 exp(a x^2) / (2 a) - x^2 exp(a x^2) / a^2 + exp(a x^2) / a^3",
	"1",

	"x^5 exp(a x^2 + b)",
	"x^4 exp(a x^2 + b) / (2 a) - x^2 exp(a x^2 + b) / a^2 + exp(a x^2 + b) / a^3",
	"1",

	"x^6 exp(a x^2)",
	"x^5 exp(a x^2) / (2 a) - 5 x^3 exp(a x^2) / (4 a^2) + 15 x exp(a x^2) / (8 a^3) + 15 i pi^(1/2) erf(i a^(1/2) x) / (16 a^(7/2))",
	"1",

	"x^6 exp(a x^2 + b)",
	"x^5 exp(a x^2 + b) / (2 a) - 5 x^3 exp(a x^2 + b) / (4 a^2) + 15 x exp(a x^2 + b) / (8 a^3) + 15 i pi^(1/2) erf(i a^(1/2) x) exp(b) / (16 a^(7/2))",
	"1",

	"x^7 exp(a x^2)",
	"x^6 exp(a x^2) / (2 a) - 3 x^4 exp(a x^2) / (2 a^2) + 3 x^2 exp(a x^2) / a^3 - 3 exp(a x^2) / a^4",
	"1",

	"x^7 exp(a x^2 + b)",
	"x^6 exp(a x^2 + b) / (2 a) - 3 x^4 exp(a x^2 + b) / (2 a^2) + 3 x^2 exp(a x^2 + b) / a^3 - 3 exp(a x^2 + b) / a^4",
	"1",

	"x^8 exp(a x^2)",
	"x^7 exp(a x^2) / (2 a) - 7 x^5 exp(a x^2) / (4 a^2) + 35 x^3 exp(a x^2) / (8 a^3) - 105 x exp(a x^2) / (16 a^4) - 105 i pi^(1/2) erf(i a^(1/2) x) / (32 a^(9/2))",
	"1",

	"x^8 exp(a x^2 + b)",
	"x^7 exp(a x^2 + b) / (2 a) - 7 x^5 exp(a x^2 + b) / (4 a^2) + 35 x^3 exp(a x^2 + b) / (8 a^3) - 105 x exp(a x^2 + b) / (16 a^4) - 105 i pi^(1/2) erf(i a^(1/2) x) exp(b) / (32 a^(9/2))",
	"1",

	"x^9 exp(a x^2)",
	"x^8 exp(a x^2) / (2 a) - 2 x^6 exp(a x^2) / a^2 + 6 x^4 exp(a x^2) / a^3 - 12 x^2 exp(a x^2) / a^4 + 12 exp(a x^2) / a^5",
	"1",

	"x^9 exp(a x^2 + b)",
	"x^8 exp(a x^2 + b) / (2 a) - 2 x^6 exp(a x^2 + b) / a^2 + 6 x^4 exp(a x^2 + b) / a^3 - 12 x^2 exp(a x^2 + b) / a^4 + 12 exp(a x^2 + b) / a^5",
	"1",
};

// log(a x) is transformed to log(a) + log(x)

char *integral_tab_log[] = {

	"log(x)",
	"x log(x) - x",
	"1",

	"log(a x + b)",
	"x log(a x + b) + b log(a x + b) / a - x",
	"1",

	"x log(x)",
	"x^2 log(x) 1/2 - x^2 1/4",
	"1",

	"x log(a x + b)",
	"1/2 (a x - b) (a x + b) log(a x + b) / a^2 - 1/4 x (a x - 2 b) / a",
	"1",

	"x^2 log(x)",
	"x^3 log(x) 1/3 - 1/9 x^3",
	"1",

	"x^2 log(a x + b)",
	"1/3 (a x + b) (a^2 x^2 - a b x + b^2) log(a x + b) / a^3 - 1/18 x (2 a^2 x^2 - 3 a b x + 6 b^2) / a^2",
	"1",

	"log(x)^2",
	"x log(x)^2 - 2 x log(x) + 2 x",
	"1",

	"log(a x + b)^2",
	"(a x + b) (log(a x + b)^2 - 2 log(a x + b) + 2) / a",
	"1",

	"log(x) / x^2",
	"-(log(x) + 1) / x",
	"1",

	"log(a x + b) / x^2",
	"a log(x) / b - (a x + b) log(a x + b) / (b x)",
	"1",

	"1 / (x (a + log(x)))",
	"log(a + log(x))",
	"1",
};

char *integral_tab_trig[] = {

	"sin(a x)",
	"-cos(a x) / a",
	"1",

	"cos(a x)",
	"sin(a x) / a",
	"1",

	"tan(a x)",
	"-log(cos(a x)) / a",
	"1",

// sin(a x)^n

	"sin(a x)^2",
	"-sin(2 a x) / (4 a) + 1/2 x",
	"1",

	"sin(a x)^3",
	"-2 cos(a x) / (3 a) - cos(a x) sin(a x)^2 / (3 a)",
	"1",

	"sin(a x)^4",
	"-sin(2 a x) / (4 a) + sin(4 a x) / (32 a) + 3/8 x",
	"1",

	"sin(a x)^5",
	"-cos(a x)^5 / (5 a) + 2 cos(a x)^3 / (3 a) - cos(a x) / a",
	"1",

	"sin(a x)^6",
	"sin(2 a x)^3 / (48 a) - sin(2 a x) / (4 a) + 3 sin(4 a x) / (64 a) + 5/16 x",
	"1",

// cos(a x)^n

	"cos(a x)^2",
	"sin(2 a x) / (4 a) + 1/2 x",
	"1",

	"cos(a x)^3",
	"cos(a x)^2 sin(a x) / (3 a) + 2 sin(a x) / (3 a)",
	"1",

	"cos(a x)^4",
	"sin(2 a x) / (4 a) + sin(4 a x) / (32 a) + 3/8 x",
	"1",

	"cos(a x)^5",
	"sin(a x)^5 / (5 a) - 2 sin(a x)^3 / (3 a) + sin(a x) / a",
	"1",

	"cos(a x)^6",
	"-sin(2 a x)^3 / (48 a) + sin(2 a x) / (4 a) + 3 sin(4 a x) / (64 a) + 5/16 x",
	"1",

//

	"sin(a x) cos(a x)",
	"1/2 sin(a x)^2 / a",
	"1",

	"sin(a x) cos(a x)^2",
	"-1/3 cos(a x)^3 / a",
	"1",

	"sin(a x)^2 cos(a x)",
	"1/3 sin(a x)^3 / a",
	"1",

	"sin(a x)^2 cos(a x)^2",
	"1/8 x - 1/32 sin(4 a x) / a",
	"1",
// 329
	"1 / sin(a x) / cos(a x)",
	"log(tan(a x)) / a",
	"1",
// 330
	"1 / sin(a x) / cos(a x)^2",
	"(1 / cos(a x) + log(tan(a x 1/2))) / a",
	"1",
// 331
	"1 / sin(a x)^2 / cos(a x)",
	"(log(tan(pi 1/4 + a x 1/2)) - 1 / sin(a x)) / a",
	"1",
// 333
	"1 / sin(a x)^2 / cos(a x)^2",
	"-2 / (a tan(2 a x))",
	"1",
//
	"sin(a x) / cos(a x)",
	"-log(cos(a x)) / a",
	"1",

	"sin(a x) / cos(a x)^2",
	"1 / a / cos(a x)",
	"1",

	"sin(a x)^2 / cos(a x)",
	"-(sin(a x) + log(cos(a x / 2) - sin(a x / 2)) - log(sin(a x / 2) + cos(a x / 2))) / a",
	"1",

	"sin(a x)^2 / cos(a x)^2",
	"tan(a x) / a - x",
	"1",

	"cos(a x) / sin(a x)",
	"log(sin(a x)) / a",
	"1",

	"cos(a x) / sin(a x)^2",
	"-1 / (a sin(a x))",
	"1",

	"cos(a x)^2 / sin(a x)^2",
	"-x - cos(a x) / sin(a x) / a",
	"1",

	"sin(a + b x)",
	"-cos(a + b x) / b",
	"1",

	"cos(a + b x)",
	"sin(a + b x) / b",
	"1",

	"x sin(a x)",
	"sin(a x) / (a^2) - x cos(a x) / a",
	"1",

	"x^2 sin(a x)",
	"2 x sin(a x) / (a^2) - (a^2 x^2 - 2) cos(a x) / (a^3)",
	"1",

	"x cos(a x)",
	"cos(a x) / (a^2) + x sin(a x) / a",
	"1",

	"x^2 cos(a x)",
	"2 x cos(a x) / (a^2) + (a^2 x^2 - 2) sin(a x) / (a^3)",
	"1",

	"1 / tan(a x)",
	"log(sin(a x)) / a",
	"1",

	"1 / cos(a x)",
	"log(tan(pi 1/4 + a x 1/2)) / a",
	"1",

	"1 / sin(a x)",
	"log(tan(a x 1/2)) / a",
	"1",

	"1 / sin(a x)^2",
	"-1 / (a tan(a x))",
	"1",

	"1 / cos(a x)^2",
	"tan(a x) / a",
	"1",

	"1 / (b + b sin(a x))",
	"-tan(pi 1/4 - a x 1/2) / (a b)",
	"1",

	"1 / (b - b sin(a x))",
	"tan(pi 1/4 + a x 1/2) / (a b)",
	"1",

	"1 / (b + b cos(a x))",
	"tan(a x 1/2) / (a b)",
	"1",

	"1 / (b - b cos(a x))",
	"-1 / (tan(a x 1/2) a b)",
	"1",

	"1 / (a + b sin(x))",
	"log((a tan(x 1/2) + b - sqrt(b^2 - a^2)) / (a tan(x 1/2) + b + sqrt(b^2 - a^2))) / sqrt(b^2 - a^2)",
	"b^2 - a^2",

	"1 / (a + b cos(x))",
	"log((sqrt(b^2 - a^2) tan(x 1/2) + a + b) / (sqrt(b^2 - a^2) tan(x 1/2) - a - b)) / sqrt(b^2 - a^2)",
	"b^2 - a^2",

	"x sin(a x) sin(b x)",
	"1/2 ((x sin(x (a - b)))/(a - b) - (x sin(x (a + b)))/(a + b) + cos(x (a - b))/(a - b)^2 - cos(x (a + b))/(a + b)^2)",
	"and(not(a + b == 0),not(a - b == 0))",

	"sin(a x)/(cos(a x) - 1)^2",
	"1/a * 1/(cos(a x) - 1)",
	"1",

	"sin(a x)/(1 - cos(a x))^2",
	"1/a * 1/(cos(a x) - 1)",
	"1",

	"cos(x)^3 sin(x)",
	"-1/4 cos(x)^4",
	"1",

	"cos(a x)^5",
	"sin(a x)^5 / (5 a) - 2 sin(a x)^3 / (3 a) + sin(a x) / a",
	"1",

	"cos(a x)^5 / sin(a x)^2",
	"sin(a x)^3 / (3 a) - 2 sin(a x) / a - 1 / (a sin(a x))",
	"1",

	"cos(a x)^3 / sin(a x)^2",
	"-sin(a x) / a - 1 / (a sin(a x))",
	"1",

	"cos(a x)^5 / sin(a x)",
	"log(sin(a x)) / a + sin(a x)^4 / (4 a) - sin(a x)^2 / a",
	"1",

	"cos(a x)^3 / sin(a x)",
	"log(sin(a x)) / a - sin(a x)^2 / (2 a)",
	"1",

	"cos(a x) sin(a x)^3",
	"sin(a x)^4 / (4 a)",
	"1",

	"cos(a x)^3 sin(a x)^2",
	"-sin(a x)^5 / (5 a) + sin(a x)^3 / (3 a)",
	"1",

	"cos(a x)^2 sin(a x)^3",
	"cos(a x)^5 / (5 a) - cos(a x)^3 / (3 a)",
	"1",

	"cos(a x)^4 sin(a x)",
	"-cos(a x)^5 / (5 a)",
	"1",

	"cos(a x)^7 / sin(a x)^2",
	"-sin(a x)^5 / (5 a) + sin(a x)^3 / a - 3 sin(a x) / a - 1 / (a sin(a x))",
	"1",

// cos(a x)^n / sin(a x)

	"cos(a x)^2 / sin(a x)",
	"cos(a x) / a + log(tan(1/2 a x)) / a",
	"1",

	"cos(a x)^4 / sin(a x)",
	"4 cos(a x) / (3 a) - cos(a x) sin(a x)^2 / (3 a) + log(tan(1/2 a x)) / a",
	"1",

	"cos(a x)^6 / sin(a x)",
	"cos(a x)^5 / (5 a) - 2 cos(a x)^3 / (3 a) + 2 cos(a x) / a - cos(a x) sin(a x)^2 / a + log(tan(1/2 a x)) / a",
	"1",
};

char *integral_tab_power[] = {

	"a", // for forms c^d where both c and d are constant expressions
	"a x",
	"1",

	"1 / x",
	"log(x)",
	"1",

	"x^a",			// integrand
	"x^(a + 1) / (a + 1)",	// answer
	"not(a = -1)",		// condition

	"a^x",
	"a^x / log(a)",
	"or(not(number(a)),a>0)",

	"1 / (a + b x)",
	"log(a + b x) / b",
	"1",
// 124
	"sqrt(a x + b)",
	"2/3 (a x + b)^(3/2) / a",
	"1",
// 138
	"sqrt(a x^2 + b)",
	"1/2 x sqrt(a x^2 + b) + 1/2 b log(sqrt(a) sqrt(a x^2 + b) + a x) / sqrt(a)",
	"1",
// 131
	"1 / sqrt(a x + b)",
	"2 sqrt(a x + b) / a",
	"1",

	"1 / ((a + b x)^2)",
	"-1 / (b (a + b x))",
	"1",

	"1 / ((a + b x)^3)",
	"-1 / ((2 b) ((a + b x)^2))",
	"1",
// 16
	"1 / (a x^2 + b)",
	"arctan(sqrt(a) x / sqrt(b)) / sqrt(a) / sqrt(b)",
	"1",
// 17
	"1 / sqrt(1 - x^2)",
	"arcsin(x)",
	"1",

	"sqrt(1 + x^2 / (1 - x^2))",
	"arcsin(x)",
	"1",

	"1 / sqrt(a x^2 + b)",
	"log(sqrt(a) sqrt(a x^2 + b) + a x) / sqrt(a)",
	"1",
// 65
	"1 / (a x^2 + b)^2",
	"1/2 ((arctan((sqrt(a) x) / sqrt(b))) / (sqrt(a) b^(3/2)) + x / (a b x^2 + b^2))",
	"1",
// 67 (m=2)
	"1 / (a + b x^2)^3",
	"x / (a + b x^2)^2 / (4 a) + 3 x / (8 a (a^2 + a b x^2)) + 3 arctan(b^(1/2) x / a^(1/2),1) / (8 a^(5/2) b^(1/2))",
	"1",
// 67 (m=3)
	"1 / (a + b x^2)^4",
	"11 x / (16 a (a + b x^2)^3) + 5 b x^3 / (6 a^2 (a + b x^2)^3) + 5 b^2 x^5 / (16 a^3 (a + b x^2)^3) + 5 arctan(b^(1/2) x / a^(1/2),1) / (16 a^(7/2) b^(1/2))",
	"1",
// 165
	"(a x^2 + b)^(-3/2)",
	"x / b / sqrt(a x^2 + b)",
	"1",
// 74
	"1 / (a x^3 + b)",
	"-log(a^(2/3) x^2 - a^(1/3) b^(1/3) x + b^(2/3))/(6 a^(1/3) b^(2/3))"
	" + log(a^(1/3) x + b^(1/3))/(3 a^(1/3) b^(2/3))"
	" - (i log(1 - (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(1/3) b^(2/3))"
	" + (i log(1 + (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(1/3) b^(2/3))", // from Wolfram Alpha
	"1",
// 77 78
	"1 / (a x^4 + b)",
	"-log(-sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(1/4) b^(3/4))"
	" + log(sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(1/4) b^(3/4))"
	" - (i log(1 - i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(1/4) b^(3/4))"
	" + (i log(1 + i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(1/4) b^(3/4))"
	" + (i log(1 - i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(1/4) b^(3/4))"
	" - (i log(1 + i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(1/4) b^(3/4))", // from Wolfram Alpha
	"1",
//
	"1 / (a x^5 + b)",
	"(sqrt(5) log(2 a^(2/5) x^2 + (sqrt(5) - 1) a^(1/5) b^(1/5) x + 2 b^(2/5))"
	" - log(2 a^(2/5) x^2 + (sqrt(5) - 1) a^(1/5) b^(1/5) x + 2 b^(2/5))"
	" - sqrt(5) log(2 a^(2/5) x^2 - (1 + sqrt(5)) a^(1/5) b^(1/5) x + 2 b^(2/5))"
	" - log(2 a^(2/5) x^2 - (1 + sqrt(5)) a^(1/5) b^(1/5) x + 2 b^(2/5))"
	" + 4 log(a^(1/5) x + b^(1/5))"
	" + 2 sqrt(2 (5 + sqrt(5))) arctan((4 a^(1/5) x + (sqrt(5) - 1) b^(1/5))/(sqrt(2 (5 + sqrt(5))) b^(1/5)))"
	" + 2 sqrt(10 - 2 sqrt(5)) arctan((4 a^(1/5) x - (1 + sqrt(5)) b^(1/5))/(sqrt(10 - 2 sqrt(5)) b^(1/5))))/(20 a^(1/5) b^(4/5))", // from Wolfram Alpha
	"1",
// 164
	"sqrt(a + x^6 + 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/4 (x sqrt((x^2 + a^(1/3))^3) + 3/2 a^(1/3) x sqrt(x^2 + a^(1/3)) + 3/2 a^(2/3) log(x + sqrt(x^2 + a^(1/3))))",
	"1",
// 165
	"sqrt(-a + x^6 - 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/4 (x sqrt((x^2 - a^(1/3))^3) - 3/2 a^(1/3) x sqrt(x^2 - a^(1/3)) + 3/2 a^(2/3) log(x + sqrt(x^2 - a^(1/3))))",
	"1",

	"sinh(x)^2",
	"sinh(2 x) 1/4 - x 1/2",
	"1",

	"tanh(x)^2",
	"x - tanh(x)",
	"1",

	"cosh(x)^2",
	"sinh(2 x) 1/4 + x 1/2",
	"1",
};

char *integral_tab[] = {

	"a",
	"a x",
	"1",

	"x",
	"1/2 x^2",
	"1",
// 18
	"x / sqrt(a x^2 + b)",
	"sqrt(a x^2 + b) / a",
	"1",

	"x / (a + b x)",
	"x / b - a log(a + b x) / (b b)",
	"1",

	"x / ((a + b x)^2)",
	"(log(a + b x) + a / (a + b x)) / (b^2)",
	"1",
// 33
	"x^2 / (a + b x)",
	"a^2 log(a + b x) / b^3 + x (b x - 2 a) / (2 b^2)",
	"1",
// 34
	"x^2 / (a + b x)^2",
	"(-a^2 / (a + b x) - 2 a log(a + b x) + b x) / b^3",
	"1",

	"x^2 / (a + b x)^3",
	"(log(a + b x) + 2 a / (a + b x) - a^2 / (2 ((a + b x)^2))) / (b^3)",
	"1",

	"1 / x / (a + b x)",
	"-log((a + b x) / x) / a",
	"1",

	"1 / x / (a + b x)^2",
	"1 / (a (a + b x)) - log((a + b x) / x) / (a^2)",
	"1",

	"1 / x / (a + b x)^3",
	"(1/2 ((2 a + b x) / (a + b x))^2 + log(x / (a + b x))) / (a^3)",
	"1",

	"1 / x^2 / (a + b x)",
	"-1 / (a x) + b log((a + b x) / x) / (a^2)",
	"1",

	"1 / x^3 / (a + b x)",
	"(2 b x - a) / (2 a^2 x^2) + b^2 log(x / (a + b x)) / (a^3)",
	"1",

	"1 / x^2 / (a + b x)^2",
	"-(a + 2 b x) / (a^2 x (a + b x)) + 2 b log((a + b x) / x) / (a^3)",
	"1",

	"x / (a + b x^2)",
	"log(a + b x^2) / (2 b)",
	"1",
// 64
	"x^2 / (a x^2 + b)",
	"1/2 i a^(-3/2) sqrt(b) (log(1 + i sqrt(a) x / sqrt(b)) - log(1 - i sqrt(a) x / sqrt(b))) + x / a",
	"1",
// 68 (m=1)
	"x / (a + b x^2)^2",
	"-1 / (2 b (a + b x^2))",
	"1",
// 68 (m=2)
	"x / (a + b x^2)^3",
	"-1 / (4 b (a + b x^2)^2)",
	"1",
// 68 (m=3)
	"x / (a + b x^2)^4",
	"-1 / (6 b (a + b x^2)^3)",
	"1",
// 69 (m=1)
	"x^2 / (a + b x^2)^2",
	"-x / (2 b (a + b x^2)) + arctan(sqrt(b/a) x) / (2 sqrt(a b^3))",
	"1",
// 69 (m=2)
	"x^2 / (a + b x^2)^3",
	"x^3 / (8 a (a + b x^2)^2) + arctan(b^(1/2) x / a^(1/2),1) / (8 a^(3/2) b^(3/2)) - x / (8 b (a + b x^2)^2)",
	"1",
// 69 (m=3)
	"x^2 / (a + b x^2)^4",
	"x^3 / (6 a (a + b x^2)^3) + b x^5 / (16 a^2 (a + b x^2)^3) + arctan(b^(1/2) x / a^(1/2),1) / (16 a^(5/2) b^(3/2)) - x / (16 b (a + b x^2)^3)",
	"1",
// 70
	"1 / x * 1 / (a + b x^2)",
	"1 log(x^2 / (a + b x^2)) / (2 a)",
	"1",
// 71
	"1 / x^2 * 1 / (a x^2 + b)",
	"1/2 i sqrt(a) b^(-3/2) (log(1 + i sqrt(a) x / sqrt(b)) - log(1 - i sqrt(a) x / sqrt(b))) - 1 / (b x)",
	"1",
// 75
	"x / (a x^3 + b)",
	"log(a^(2/3) x^2 - a^(1/3) b^(1/3) x + b^(2/3))/(6 a^(2/3) b^(1/3))"
	" - log(a^(1/3) x + b^(1/3))/(3 a^(2/3) b^(1/3))"
	" - (i log(1 - (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(2/3) b^(1/3))"
	" + (i log(1 + (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(2/3) b^(1/3))", // from Wolfram Alpha
	"1",
// 76
	"x^2 / (a + b x^3)",
	"1 log(a + b x^3) / (3 b)",
	"1",
// 79 80
	"x / (a x^4 + b)",
	"(i log(1 - (i sqrt(a) x^2)/sqrt(b)))/(4 sqrt(a) sqrt(b))"
	" - (i log(1 + (i sqrt(a) x^2)/sqrt(b)))/(4 sqrt(a) sqrt(b))", // from Wolfram Alpha
	"1",
// 81 82
	"x^2 / (a x^4 + b)",
	"log(-sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(3/4) b^(1/4))"
	" - log(sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(3/4) b^(1/4))"
	" - (i log(1 - i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(3/4) b^(1/4))"
	" + (i log(1 + i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(3/4) b^(1/4))"
	" + (i log(1 - i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(3/4) b^(1/4))"
	" - (i log(1 + i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(3/4) b^(1/4))", // from Wolfram Alpha
	"1",
//
	"x^3 / (a + b x^4)",
	"1 log(a + b x^4) / (4 b)",
	"1",

	"x sqrt(a + b x)",
	"-2 (2 a - 3 b x) sqrt((a + b x)^3) 1/15 / (b^2)",
	"1",

	"x^2 sqrt(a + b x)",
	"2 (8 a^2 - 12 a b x + 15 b^2 x^2) sqrt((a + b x)^3) 1/105 / (b^3)",
	"1",

	"x^2 sqrt(a + b x^2)",
	"(sqrt(b) x sqrt(a + b x^2) (a + 2 b x^2) - a^2 log(sqrt(b) sqrt(a + b x^2) + b x)) / (8 b^(3/2))",
	"1",
// 128
	"sqrt(a x + b) / x",
	"2 sqrt(a x + b) - 2 sqrt(b) arctanh(sqrt(a x + b) / sqrt(b))",
	"1",
// 129
	"sqrt(a x + b) / x^2",
	"-sqrt(a x + b) / x - a arctanh(sqrt(a x + b) / sqrt(b)) / sqrt(b)",
	"1",

	"x / sqrt(a + b x)",
	"-2 (2 a - b x) sqrt(a + b x) / (3 (b^2))",
	"1",

	"x^2 / sqrt(a + b x)",
	"2 (8 a^2 - 4 a b x + 3 b^2 x^2) sqrt(a + b x) / (15 (b^3))",
	"1",
// 134
	"1 / x / sqrt(a x + b)",
	"-2 arctanh(sqrt(a x + b) / sqrt(b)) / sqrt(b)",
	"1",
// 137
	"1 / x^2 / sqrt(a x + b)",
	"a arctanh(sqrt(a x + b) / sqrt(b)) / b^(3/2) - sqrt(a x + b) / (b x)",
	"1",
// 158
	"1 / x / sqrt(a x^2 + b)",
	"(log(x) - log(sqrt(b) sqrt(a x^2 + b) + b)) / sqrt(b)",
	"1",
// 160
	"sqrt(a x^2 + b) / x",
	"sqrt(a x^2 + b) - sqrt(b) log(sqrt(b) sqrt(a x^2 + b) + b) + sqrt(b) log(x)",
	"1",
// 163
	"x sqrt(a x^2 + b)",
	"1/3 (a x^2 + b)^(3/2) / a",
	"1",
// 166
	"x (a x^2 + b)^(-3/2)",
	"-1 / a / sqrt(a x^2 + b)",
	"1",

	"x sqrt(a + x^6 + 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/5 sqrt((x^2 + a^(1/3))^5)",
	"1",
// 168
	"x^2 sqrt(a x^2 + b)",
	"1/8 a^(-3/2) (sqrt(a) x sqrt(a x^2 + b) (2 a x^2 + b) - b^2 log(sqrt(a) sqrt(a x^2 + b) + a x))",
	"and(number(a),a>0)",
// 169
	"x^3 sqrt(a x^2 + b)",
	"1/15 sqrt(a x^2 + b) (3 a^2 x^4 + a b x^2 - 2 b^2) / a^2",
	"1",
// 171
	"x^2 / sqrt(a x^2 + b)",
	"1/2 a^(-3/2) (sqrt(a) x sqrt(a x^2 + b) - b log(sqrt(a) sqrt(a x^2 + b) + a x))",
	"1",
// 172
	"x^3 / sqrt(a x^2 + b)",
	"1/3 (a x^2 - 2 b) sqrt(a x^2 + b) / a^2",
	"1",
// 173
	"1 / x^2 / sqrt(a x^2 + b)",
	"-sqrt(a x^2 + b) / (b x)",
	"1",
// 174
	"1 / x^3 / sqrt(a x^2 + b)",
	"-sqrt(a x^2 + b) / (2 b x^2) + a (log(sqrt(b) sqrt(a x^2 + b) + b) - log(x)) / (2 b^(3/2))",
	"1",
// 216
	"sqrt(a x^2 + b) / x^2",
	"sqrt(a) log(sqrt(a) sqrt(a x^2 + b) + a x) - sqrt(a x^2 + b) / x",
	"and(number(a),a>0)",
// 217
	"sqrt(a x^2 + b) / x^3",
	"1/2 (-sqrt(a x^2 + b) / x^2 - (a log(sqrt(b) sqrt(a x^2 + b) + b)) / sqrt(b) + (a log(x)) / sqrt(b))",
	"and(number(b),b>0)",

	"arcsin(a x)",
	"x arcsin(a x) + sqrt(1 - a^2 x^2) / a",
	"1",

	"arccos(a x)",
	"x arccos(a x) + sqrt(1 - a^2 x^2) / a",
	"1",

	"arctan(a x)",
	"x arctan(a x) - log(1 + a^2 x^2) / (2 a)",
	"1",

	"sinh(x)",
	"cosh(x)",
	"1",

	"cosh(x)",
	"sinh(x)",
	"1",

	"tanh(x)",
	"log(cosh(x))",
	"1",

	"x sinh(x)",
	"x cosh(x) - sinh(x)",
	"1",

	"x cosh(x)",
	"x sinh(x) - cosh(x)",
	"1",

	"erf(a x)",
	"x erf(a x) + exp(-a^2 x^2) / (a sqrt(pi))",
	"1",

	"x^2 (1 - x^2)^(3/2)",
	"(x sqrt(1 - x^2) (-8 x^4 + 14 x^2 - 3) + 3 arcsin(x)) 1/48",
	"1",

	"x^2 (1 - x^2)^(5/2)",
	"(x sqrt(1 - x^2) (48 x^6 - 136 x^4 + 118 x^2 - 15) + 15 arcsin(x)) 1/384",
	"1",

	"x^4 (1 - x^2)^(3/2)",
	"(-x sqrt(1 - x^2) (16 x^6 - 24 x^4 + 2 x^2 + 3) + 3 arcsin(x)) 1/128",
	"1",
};

void
eval_integral(struct atom *p1)
{
	int flag, i, n;
	struct atom *X, *Y = NULL; // silence compiler

	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_symbol(X_LOWER);
		integral();
		return;
	}

	flag = 0;

	while (iscons(p1) || flag) {

		if (flag) {
			X = Y;
			flag = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			push_symbol(X_LOWER);
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				integral();
			}
			continue;
		}

		if (!isusersymbol(X))
			stopf("integral");

		if (iscons(p1)) {

			push(car(p1));
			evalf();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				for (i = 0; i < n; i++) {
					push(X);
					integral();
				}
				continue;
			}

			flag = 1;
		}

		push(X);
		integral();
	}
}

void
integral(void)
{
	int h;
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	if (!isusersymbol(X))
		stopf("integral: symbol expected");

	if (car(F) == symbol(ADD)) {
		h = tos;
		p1 = cdr(F);
		while (iscons(p1)) {
			push(car(p1));
			push(X);
			integral();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(F) == symbol(MULTIPLY)) {
		push(F);
		push(X);
		partition_term();	// push const part then push var part
		F = pop();		// pop var part
		integral_nib(F, X);
		multiply();		// multiply by const part
		return;
	}

	integral_nib(F, X);
}

void
integral_nib(struct atom *F, struct atom *X)
{
	int h;

	save_symbol(symbol(SA));
	save_symbol(symbol(SB));
	save_symbol(symbol(SX));

	set_symbol(symbol(SX), X, symbol(NIL));

	// put constants in F(X) on the stack

	h = tos;

	push_integer(1); // 1 is a candidate for a or b

	push(F);
	push(X);
	decomp(); // push const coeffs

	integral_lookup(h, F);

	restore_symbol();
	restore_symbol();
	restore_symbol();
}

void
integral_lookup(int h, struct atom *F)
{
	int t;

	t = integral_classify(F);

	if ((t & 1) && integral_search(h, F, integral_tab_exp, sizeof integral_tab_exp / sizeof (char *)))
		return;

	if ((t & 2) && integral_search(h, F, integral_tab_log, sizeof integral_tab_log / sizeof (char *)))
		return;

	if ((t & 4) && integral_search(h, F, integral_tab_trig, sizeof integral_tab_trig / sizeof (char *)))
		return;

	if (car(F) == symbol(POWER)) {
		if (integral_search(h, F, integral_tab_power, sizeof integral_tab_power / sizeof (char *)))
			return;
	} else {
		if (integral_search(h, F, integral_tab, sizeof integral_tab / sizeof (char *)))
			return;
	}

	stopf("integral: no solution found");
}

int
integral_classify(struct atom *p)
{
	int t = 0;

	if (iscons(p)) {
		while (iscons(p)) {
			t |= integral_classify(car(p));
			p = cdr(p);
		}
		return t;
	}

	if (p == symbol(EXP1))
		return 1;

	if (p == symbol(LOG))
		return 2;

	if (p == symbol(SIN) || p == symbol(COS) || p == symbol(TAN))
		return 4;

	return 0;
}

int
integral_search(int h, struct atom *F, char **table, int n)
{
	int i;
	struct atom *C, *I;

	for (i = 0; i < n; i += 3) {

		scan1(table[i + 0]); // integrand
		I = pop();

		scan1(table[i + 2]); // condition
		C = pop();

		if (integral_search_nib(h, F, I, C))
			break;
	}

	if (i >= n)
		return 0;

	tos = h; // pop all

	scan1(table[i + 1]); // answer
	evalf();

	return 1;
}

int
integral_search_nib(int h, struct atom *F, struct atom *I, struct atom *C)
{
	int i, j;
	struct atom *p1;

	for (i = h; i < tos; i++) {

		set_symbol(symbol(SA), stack[i], symbol(NIL));

		for (j = h; j < tos; j++) {

			set_symbol(symbol(SB), stack[j], symbol(NIL));

			push(C);			// condition ok?
			evalf();
			p1 = pop();
			if (iszero(p1))
				continue;		// no, go to next j

			push(F);			// F = I?
			push(I);
			evalf();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}

	return 0;					// no
}

// push const coeffs

void
decomp(void)
{
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	// is the entire expression constant?

	if (!findf(F, X)) {
		push(F);
		return;
	}

	// sum?

	if (car(F) == symbol(ADD)) {
		decomp_sum(F, X);
		return;
	}

	// product?

	if (car(F) == symbol(MULTIPLY)) {
		decomp_product(F, X);
		return;
	}

	// naive decomp

	p1 = cdr(F);
	while (iscons(p1)) {
		push(car(p1));
		push(X);
		decomp();
		p1 = cdr(p1);
	}
}

void
decomp_sum(struct atom *F, struct atom *X)
{
	int h, i, j, k, n;
	struct atom *p1, *p2;

	h = tos;

	// partition terms

	p1 = cdr(F);

	while (iscons(p1)) {
		p2 = car(p1);
		if (findf(p2, X)) {
			if (car(p2) == symbol(MULTIPLY)) {
				push(p2);
				push(X);
				partition_term();	// push const part then push var part
			} else {
				push_integer(1);	// const part
				push(p2);		// var part
			}
		}
		p1 = cdr(p1);
	}

	// combine const parts of matching var parts

	n = tos - h;

	for (i = 0; i < n - 2; i += 2)
		for (j = i + 2; j < n; j += 2) {
			if (!equal(stack[h + i + 1], stack[h + j + 1]))
				continue;
			push(stack[h + i]); // add const parts
			push(stack[h + j]);
			add();
			stack[h + i] = pop();
			for (k = j; k < n - 2; k++)
				stack[h + k] = stack[h + k + 2];
			j -= 2; // use same j again
			n -= 2;
			tos -= 2; // pop
		}

	// push const parts, decomp var parts

	list(tos - h);
	p1 = pop();

	while (iscons(p1)) {
		push(car(p1)); // const part
		push(cadr(p1)); // var part
		push(X);
		decomp();
		p1 = cddr(p1);
	}

	// add together all constant terms

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // makes ADD head of list
	}
}

void
decomp_product(struct atom *F, struct atom *X)
{
	int h, n;
	struct atom *p1;

	// decomp factors involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X)) {
			push(car(p1));
			push(X);
			decomp();
		}
		p1 = cdr(p1);
	}

	// combine constant factors

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}
}

void
partition_term(void)
{
	int h, n;
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	// push const factors

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}

	// push var factors

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}
}
void
eval_inv(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	inv();
}

void
inv(void)
{
	struct atom *p1;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		reciprocate();
		return;
	}

	if (!issquarematrix(p1))
		stopf("inv: square matrix expected");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}
void
eval_kronecker(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		kronecker();
		p1 = cdr(p1);
	}
}

void
kronecker(void)
{
	int h, i, j, k, l, m, n, p, q;
	struct atom *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim > 2 || p2->u.tensor->ndim > 2)
		stopf("kronecker");

	m = p1->u.tensor->dim[0];
	n = p1->u.tensor->ndim == 1 ? 1 : p1->u.tensor->dim[1];

	p = p2->u.tensor->dim[0];
	q = p2->u.tensor->ndim == 1 ? 1 : p2->u.tensor->dim[1];

	p3 = alloc_tensor(m * n * p * q);

	// result matrix has (m * p) rows and (n * q) columns

	h = 0;

	for (i = 0; i < m; i++)
		for (j = 0; j < p; j++)
			for (k = 0; k < n; k++)
				for (l = 0; l < q; l++) {
					push(p1->u.tensor->elem[n * i + k]);
					push(p2->u.tensor->elem[q * j + l]);
					multiply();
					p3->u.tensor->elem[h++] = pop();
				}

	// dim info

	p3->u.tensor->ndim = n * q == 1 ? 1 : 2;

	p3->u.tensor->dim[0] = m * p;

	if (n * q > 1)
		p3->u.tensor->dim[1] = n * q;

	push(p3);
}
void
eval_log(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	logfunc();
}

void
logfunc(void)
{
	int h, i, n;
	double d;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			logfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	// log of zero is not evaluated

	if (iszero(p1)) {
		push_symbol(LOG);
		push_integer(0);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (d > 0.0) {
			push_double(log(d));
			return;
		}
	}

	// log(z) -> log(mag(z)) + i arg(z)

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		mag();
		logfunc();
		push(p1);
		arg();
		push(imaginaryunit);
		multiply();
		add();
		return;
	}

	// log(1) -> 0

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	// log(e) -> 1

	if (p1 == symbol(EXP1)) {
		push_integer(1);
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		logfunc();
		push(imaginaryunit);
		push_symbol(PI);
		multiply();
		add();
		return;
	}

	// log(10) -> log(2) + log(5)

	if (isrational(p1)) {
		h = tos;
		push(p1);
		factor_factor();
		for (i = h; i < tos; i++) {
			p2 = stack[i];
			if (car(p2) == symbol(POWER)) {
				push(caddr(p2)); // exponent
				push_symbol(LOG);
				push(cadr(p2)); // base
				list(2);
				multiply();
			} else {
				push_symbol(LOG);
				push(p2);
				list(2);
			}
			stack[i] = pop();
		}
		add_terms(tos - h);
		return;
	}

	// log(a ^ b) -> b log(a)

	if (car(p1) == symbol(POWER)) {
		push(caddr(p1));
		push(cadr(p1));
		logfunc();
		multiply();
		return;
	}

	// log(a * b) -> log(a) + log(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			logfunc();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	push_symbol(LOG);
	push(p1);
	list(2);
}
void
eval_mag(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	mag();
}

void
mag(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			mag();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	// use numerator and denominator to handle (a + i b) / (c + i d)

	push(p1);
	numerator();
	mag_nib();

	push(p1);
	denominator();
	mag_nib();

	divide();
}

void
mag_nib(void)
{
	int h;
	struct atom *p1, *RE, *IM;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		absfunc();
		return;
	}

	// -1 to a power

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_integer(1);
		return;
	}

	// exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		real();
		expfunc();
		return;
	}

	// product

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			mag();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	// sum

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(RE);
		push(RE);
		multiply();
		push(IM);
		push(IM);
		multiply();
		add();
		push_rational(1, 2);
		power();
		return;
	}

	// real

	push(p1);
}
void
eval_minor(struct atom *p1)
{
	int i, j;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2 || p2->u.tensor->dim[0] != p2->u.tensor->dim[1])
		stopf("minor");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stopf("minor");

	push(p2);

	minormatrix(i, j);

	det();
}
void
eval_minormatrix(struct atom *p1)
{
	int i, j;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2)
		stopf("minormatrix: matrix expected");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stopf("minormatrix: index err");

	push(p2);

	minormatrix(i, j);
}

void
minormatrix(int row, int col)
{
	int i, j, k, m, n;
	struct atom *p1, *p2;

	p2 = symbol(NIL); // silence compiler

	p1 = pop();

	n = p1->u.tensor->dim[0];
	m = p1->u.tensor->dim[1];

	if (n == 2 && m == 2) {
		if (row == 1) {
			if (col == 1)
				push(p1->u.tensor->elem[3]);
			else
				push(p1->u.tensor->elem[2]);
		} else {
			if (col == 1)
				push(p1->u.tensor->elem[1]);
			else
				push(p1->u.tensor->elem[0]);
		}
		return;
	}

	if (n == 2)
		p2 = alloc_vector(m - 1);

	if (m == 2)
		p2 = alloc_vector(n - 1);

	if (n > 2 && m > 2)
		p2 = alloc_matrix(n - 1, m - 1);

	row--;
	col--;

	k = 0;

	for (i = 0; i < n; i++) {

		if (i == row)
			continue;

		for (j = 0; j < m; j++) {

			if (j == col)
				continue;

			p2->u.tensor->elem[k++] = p1->u.tensor->elem[m * i + j];
		}
	}

	push(p2);
}
void
eval_mod(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	modfunc();
}

void
modfunc(void)
{
	int i, n;
	double d1, d2;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			push(p2);
			modfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!isnum(p1) || !isnum(p2) || iszero(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (isrational(p1) && isrational(p2)) {
		mod_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(fmod(d1, d2));
}

void
mod_rationals(struct atom *p1, struct atom *p2)
{
	if (isinteger(p1) && isinteger(p2)) {
		mod_integers(p1, p2);
		return;
	}
	push(p1);
	push(p1);
	push(p2);
	divide();
	absfunc();
	floorfunc();
	push(p2);
	multiply();
	if (p1->sign == p2->sign)
		negate();
	add();
}

void
mod_integers(struct atom *p1, struct atom *p2)
{
	uint32_t *a, *b;
	a = mmod(p1->u.q.a, p2->u.q.a);
	b = mint(1);
	push_bignum(p1->sign, a, b);
}
void
eval_multiply(struct atom *p1)
{
	int h = tos;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
	multiply_factors(tos - h);
	expanding++;
}

void
multiply(void)
{
	multiply_factors(2);
}

void
multiply_factors(int n)
{
	int h;
	struct atom *T;

	if (n < 2)
		return;

	h = tos - n;

	flatten_factors(h);

	T = multiply_tensor_factors(h);

	multiply_scalar_factors(h);

	if (istensor(T)) {
		push(T);
		inner();
	}
}

void
flatten_factors(int h)
{
	int i, n;
	struct atom *p1;
	n = tos;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(MULTIPLY)) {
			stack[i] = cadr(p1);
			p1 = cddr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

struct atom *
multiply_tensor_factors(int h)
{
	int i, j;
	struct atom *p1, *T;
	T = symbol(NIL);
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (!istensor(p1))
			continue;
		if (istensor(T)) {
			push(T);
			push(p1);
			hadamard();
			T = pop();
		} else
			T = p1;
		// remove the factor
		for (j = i + 1; j < tos; j++)
			stack[j - 1] = stack[j];
		i--;
		tos--;
	}
	return T;
}

void
multiply_scalar_factors(int h)
{
	int n;
	struct atom *COEF;

	COEF = one;

	COEF = combine_numerical_factors(h, COEF);

	if (iszero(COEF) || h == tos) {
		tos = h;
		push(COEF);
		return;
	}

	combine_factors(h);
	normalize_power_factors(h);

	// do again in case exp(1/2 i pi) changed to i

	combine_factors(h);
	normalize_power_factors(h);

	COEF = combine_numerical_factors(h, COEF);

	if (iszero(COEF) || h == tos) {
		tos = h;
		push(COEF);
		return;
	}

	COEF = reduce_radical_factors(h, COEF);

	if (isdouble(COEF) || !isplusone(COEF))
		push(COEF);

	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack

	n = tos - h;

	switch (n) {
	case 0:
		push_integer(1); // all factors canceled
		break;
	case 1:
		break;
	default:
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
		break;
	}
}

struct atom *
combine_numerical_factors(int h, struct atom *COEF)
{
	int i, j;
	struct atom *p1;
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			multiply_numbers(COEF, p1);
			COEF = pop();
			// remove the factor
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			i--;
			tos--;
		}
	}
	return COEF;
}

// factors that have the same base are combined by adding exponents

void
combine_factors(int h)
{
	int i, j;
	sort_factors_provisional(h);
	for (i = h; i < tos - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			// remove the factor
			for (j = i + 2; j < tos; j++)
				stack[j - 1] = stack[j];
			i--;
			tos--;
		}
	}
}

int
combine_factors_nib(int i, int j)
{
	struct atom *p1, *p2, *BASE1, *EXPO1, *BASE2, *EXPO2;

	p1 = stack[i];
	p2 = stack[j];

	if (car(p1) == symbol(POWER)) {
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
	} else {
		BASE1 = p1;
		EXPO1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		BASE2 = cadr(p2);
		EXPO2 = caddr(p2);
	} else {
		BASE2 = p2;
		EXPO2 = one;
	}

	if (!equal(BASE1, BASE2))
		return 0;

	if (isdouble(BASE2))
		BASE1 = BASE2; // if mixed rational and double, use double

	push_symbol(POWER);
	push(BASE1);
	push(EXPO1);
	push(EXPO2);
	add();
	list(3);

	stack[i] = pop();

	return 1;
}

void
sort_factors_provisional(int h)
{
	qsort(stack + h, tos - h, sizeof (struct atom *), sort_factors_provisional_func);
}

int
sort_factors_provisional_func(const void *q1, const void *q2)
{
	return cmp_factors_provisional(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors_provisional(struct atom *p1, struct atom *p2)
{
	if (car(p1) == symbol(POWER))
		p1 = cadr(p1); // p1 = base

	if (car(p2) == symbol(POWER))
		p2 = cadr(p2); // p2 = base

	return cmp(p1, p2);
}

void
normalize_power_factors(int h)
{
	int i, k;
	struct atom *p1;
	k = tos;
	for (i = h; i < k; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			push(cadr(p1));
			push(caddr(p1));
			power();
			p1 = pop();
			if (car(p1) == symbol(MULTIPLY)) {
				stack[i] = cadr(p1);
				p1 = cddr(p1);
				while (iscons(p1)) {
					push(car(p1));
					p1 = cdr(p1);
				}
			} else
				stack[i] = p1;
		}
	}
}

void
expand_sum_factors(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h, *p1, *p2;

	p2 = symbol(NIL); // silence compiler

	if (n < 2)
		return;

	// search for a sum factor

	for (i = 0; i < n; i++) {
		p2 = s[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == n)
		return; // no sum factors

	// remove the sum factor

	for (j = i + 1; j < n; j++)
		s[j - 1] = s[j];

	n--;
	tos--;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum factor

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(tos - h);
}

void
sort_factors(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_factors_func);
}

int
sort_factors_func(const void *q1, const void *q2)
{
	return cmp_factors(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors(struct atom *p1, struct atom *p2)
{
	int a, b, c;
	struct atom *base1, *base2, *expo1, *expo2;

	a = order_factor(p1);
	b = order_factor(p2);

	if (a < b)
		return -1;

	if (a > b)
		return 1;

	if (car(p1) == symbol(POWER)) {
		base1 = cadr(p1);
		expo1 = caddr(p1);
	} else {
		base1 = p1;
		expo1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		base2 = cadr(p2);
		expo2 = caddr(p2);
	} else {
		base2 = p2;
		expo2 = one;
	}

	c = cmp(base1, base2);

	if (c == 0)
		c = cmp(expo2, expo1); // swapped to reverse sort order

	return c;
}

//  1	number
//  2	number to power (root)
//  3	-1 to power (imaginary)
//  4	other factor (symbol, power, func, etc)
//  5	exponential
//  6	derivative

int
order_factor(struct atom *p)
{
	if (isnum(p))
		return 1;

	if (p == symbol(EXP1))
		return 5;

	if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(D_LOWER))
		return 6;

	if (car(p) == symbol(POWER)) {

		p = cadr(p); // p = base

		if (isminusone(p))
			return 3;

		if (isnum(p))
			return 2;

		if (p == symbol(EXP1))
			return 5;

		if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(D_LOWER))
			return 6;
	}

	return 4;
}

void
multiply_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2)) {
		multiply_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 * d2);
}

void
multiply_rationals(struct atom *p1, struct atom *p2)
{
	int sign;
	uint32_t *a, *b, *c;

	if (iszero(p1) || iszero(p2)) {
		push_integer(0);
		return;
	}

	if (p1->sign == p2->sign)
		sign = MPLUS;
	else
		sign = MMINUS;

	if (isinteger(p1) && isinteger(p2)) {
		push_bignum(sign, mmul(p1->u.q.a, p2->u.q.a), mint(1));
		return;
	}

	a = mmul(p1->u.q.a, p2->u.q.a);
	b = mmul(p1->u.q.b, p2->u.q.b);
	c = mgcd(a, b);
	push_bignum(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

// for example, 2 / sqrt(2) -> sqrt(2)

struct atom *
reduce_radical_factors(int h, struct atom *COEF)
{
	if (!any_radical_factors(h))
		return COEF;

	if (isrational(COEF))
		return reduce_radical_rational(h, COEF);
	else
		return reduce_radical_double(h, COEF);
}

int
any_radical_factors(int h)
{
	int i, n;
	n = tos;
	for (i = h; i < n; i++)
		if (isradical(stack[i]))
			return 1;
	return 0;
}

struct atom *
reduce_radical_double(int h, struct atom *COEF)
{
	int i, j;
	double a, b, c;
	struct atom *p1;

	c = COEF->u.d;

	for (i = h; i < tos; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * pow(a, b); // a > 0 by isradical above

			// remove the factor

			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];

			i--; // use same index again
			tos--;
		}
	}

	push_double(c);
	COEF = pop();

	return COEF;
}

struct atom *
reduce_radical_rational(int h, struct atom *COEF)
{
	int i, k;
	struct atom *p1, *p2, *NUMER, *DENOM, *BASE, *EXPO;

	if (isplusone(COEF) || isminusone(COEF))
		return COEF; // COEF has no factors, no cancellation is possible

	push(COEF);
	absfunc();
	p1 = pop();

	push(p1);
	numerator();
	NUMER = pop();

	push(p1);
	denominator();
	DENOM = pop();

	k = 0;

	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (!isradical(p1))
			continue;
		BASE = cadr(p1);
		EXPO = caddr(p1);
		if (isnegativenumber(EXPO)) {
			mod_integers(NUMER, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		} else {
			mod_integers(DENOM, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(-1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		}
	}

	if (k) {
		push(NUMER);
		push(DENOM);
		divide();
		if (isnegativenumber(COEF))
			negate();
		COEF = pop();
	}

	return COEF;
}

void
multiply_expand(void)
{
	expanding++;
	multiply();
	expanding--;
}

void
multiply_noexpand(void)
{
	int t;
	t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}

void
multiply_factors_noexpand(int n)
{
	int t;
	t = expanding;
	expanding = 0;
	multiply_factors(n);
	expanding = t;
}

void
negate(void)
{
	push_integer(-1);
	multiply();
}

void
negate_noexpand(void)
{
	int t;
	t = expanding;
	expanding = 0;
	negate();
	expanding = t;
}

void
reciprocate(void)
{
	push_integer(-1);
	power();
}

void
divide(void)
{
	reciprocate();
	multiply();
}
void
eval_nil(struct atom *p1)
{
	(void) p1; // silence compiler
	push_symbol(NIL);
}
void
eval_noexpand(struct atom *p1)
{
	int t;

	t = expanding;
	expanding = 0;

	push(cadr(p1));
	evalf();

	expanding = t;
}
void
eval_not(struct atom *p1)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}
#define DELTA 1e-6
#define EPSILON 1e-9

void
eval_nroots(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
	} else
		push_symbol(X_LOWER);

	nroots();
}

void
nroots(void)
{
	int h, i, n;
	struct atom *A, *P, *X, *RE, *IM;
	double ar, ai, d, xr, xi, yr, yi;
	static double *cr, *ci;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	n = tos - h; // number of coeffs on stack

	if (cr)
		free(cr);
	if (ci)
		free(ci);

	cr = alloc_mem(n * sizeof (double));
	ci = alloc_mem(n * sizeof (double));

	// convert coeffs to floating point

	for (i = 0; i < n; i++) {

		push(stack[h + i]);
		real();
		floatfunc();
		RE = pop();

		push(stack[h + i]);
		imag();
		floatfunc();
		IM = pop();

		if (!isdouble(RE) || !isdouble(IM))
			stopf("nroots: coeffs");

		cr[i] = RE->u.d;
		ci[i] = IM->u.d;
	}

	tos = h; // pop all

	// divide p(x) by leading coeff

	xr = cr[n - 1];
	xi = ci[n - 1];

	d = xr * xr + xi * xi;

	for (i = 0; i < n - 1; i++) {
		yr = (cr[i] * xr + ci[i] * xi) / d;
		yi = (ci[i] * xr - cr[i] * xi) / d;
		cr[i] = yr;
		ci[i] = yi;
	}

	cr[n - 1] = 1.0;
	ci[n - 1] = 0.0;

	// find roots

	while (n > 1) {

		nfindroot(cr, ci, n, &ar, &ai);

		if (fabs(ar) < DELTA * fabs(ai))
			ar = 0.0;

		if (fabs(ai) < DELTA * fabs(ar))
			ai = 0.0;

		// push root

		push_double(ar);
		push_double(ai);
		push(imaginaryunit);
		multiply();
		add();

		// divide p(x) by x - a

		nreduce(cr, ci, n, ar, ai);

		// note: leading coeff of p(x) is still 1

		n--;
	}

	n = tos - h; // number of roots on stack

	if (n == 0) {
		push_symbol(NIL); // no roots
		return;
	}

	if (n == 1)
		return; // one root

	sort(n);

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[h + i];

	tos = h; // pop all

	push(A);
}

void
nfindroot(double cr[], double ci[], int n, double *par, double *pai)
{
	int i, j;
	double d;
	double ar, br, dfr, dxr, far, fbr, xr, yr;
	double ai, bi, dfi, dxi, fai, fbi, xi, yi;

	// if const term is zero then root is zero

	// note: use exact zero, not "close to zero"

	// term will be exactly zero from coeffs(), no need for arbitrary cutoff

	if (cr[0] == 0.0 && ci[0] == 0.0) {
		*par = 0.0;
		*pai = 0.0;
		return;
	}

	// secant method

	for (i = 0; i < 100; i++) {

		ar = urandom();
		ai = urandom();

		fata(cr, ci, n, ar, ai, &far, &fai);

		br = ar;
		bi = ai;

		fbr = far;
		fbi = fai;

		ar = urandom();
		ai = urandom();

		for (j = 0; j < 1000; j++) {

			fata(cr, ci, n, ar, ai, &far, &fai);

			if (zabs(far, fai) < EPSILON) {
				*par = ar;
				*pai = ai;
				return;
			}

			if (zabs(far, fai) < zabs(fbr, fbi)) {

				xr = ar;
				xi = ai;

				ar = br;
				ai = bi;

				br = xr;
				bi = xi;

				xr = far;
				xi = fai;

				far = fbr;
				fai = fbi;

				fbr = xr;
				fbi = xi;
			}

			// dx = b - a

			dxr = br - ar;
			dxi = bi - ai;

			// df = fb - fa

			dfr = fbr - far;
			dfi = fbi - fai;

			// y = dx / df

			d = dfr * dfr + dfi * dfi;

			if (d == 0.0)
				break;

			yr = (dxr * dfr + dxi * dfi) / d;
			yi = (dxi * dfr - dxr * dfi) / d;

			// a = b - y * fb

			ar = br - (yr * fbr - yi * fbi);
			ai = bi - (yr * fbi + yi * fbr);
		}
	}

	stopf("nroots: convergence error");
}

// compute f at a

void
fata(double cr[], double ci[], int n, double ar, double ai, double *far, double *fai)
{
	int k;
	double xr, xi, yr, yi;

	yr = cr[n - 1];
	yi = ci[n - 1];

	for (k = n - 2; k >= 0; k--) {

		// x = a * y

		xr = ar * yr - ai * yi;
		xi = ar * yi + ai * yr;

		// y = x + c

		yr = xr + cr[k];
		yi = xi + ci[k];
	}

	*far = yr;
	*fai = yi;
}

// divide by x - a

void
nreduce(double cr[], double ci[], int n, double ar, double ai)
{
	int k;

	// divide

	for (k = n - 1; k > 0; k--) {
		cr[k - 1] += cr[k] * ar - ci[k] * ai;
		ci[k - 1] += ci[k] * ar + cr[k] * ai;
	}

	if (zabs(cr[0], ci[0]) > DELTA)
		stopf("nroots: residual error"); // not a root

	// shift

	for (k = 0; k < n - 1; k++) {
		cr[k] = cr[k + 1];
		ci[k] = ci[k + 1];
	}
}

double
zabs(double r, double i)
{
	return sqrt(r * r + i * i);
}

double
urandom(void)
{
	return 4.0 * ((double) rand() / (double) RAND_MAX) - 2.0;
}
void
eval_number(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = pop();

	if (isnum(p1))
		push_integer(1);
	else
		push_integer(0);
}
void
eval_numerator(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	numerator();
}

void
numerator(void)
{
	struct atom *p1;

	p1 = pop();

	if (isrational(p1)) {
		push_bignum(p1->sign, mcopy(p1->u.q.a), mint(1));
		return;
	}

	while (find_divisor(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
void
eval_or(struct atom *p1)
{
	struct atom *p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push_integer(1);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(0);
}
void
eval_outer(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		outer();
		p1 = cdr(p1);
	}
}

void
outer(void)
{
	int i, j, k, n, ncol, nrow;
	struct atom *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim + p2->u.tensor->ndim > MAXDIM)
		stopf("outer: rank exceeds max");

	// sync diffs

	nrow = p1->u.tensor->nelem;
	ncol = p2->u.tensor->nelem;

	p3 = alloc_tensor(nrow * ncol);

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++) {
			push(p1->u.tensor->elem[i]);
			push(p2->u.tensor->elem[j]);
			multiply();
			p3->u.tensor->elem[i * ncol + j] = pop();
		}

	// dim info

	p3->u.tensor->ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	k = 0;

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	n = p2->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
void
eval_polar(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	polar();
}

void
polar(void)
{
	int i, n;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			polar();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	mag();
	push(imaginaryunit);
	push(p1);
	arg();
	p2 = pop();
	if (isdouble(p2)) {
		push_double(p2->u.d / M_PI);
		push_symbol(PI);
		multiply_factors(3);
	} else {
		push(p2);
		multiply_factors(2);
	}
	expfunc();
	multiply();
}
void
eval_power(struct atom *p1)
{
	int t;
	struct atom *p2;

	expanding--; // undo expanding++ in evalf

	// base

	push(cadr(p1));

	// exponent

	push(caddr(p1));
	evalg();
	dupl();
	p2 = pop();

	// if exponent is negative then evaluate base without expanding

	swap();
	if (isnegativenumber(p2)) {
		t = expanding;
		expanding = 0;
		evalg();
		expanding = t;
	} else
		evalg();
	swap();

	power();

	expanding++;
}

void
power(void)
{
	int h, i, n;
	struct atom *p1, *BASE, *EXPO;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE) && istensor(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (istensor(EXPO)) {
		p1 = copy_tensor(EXPO);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(BASE);
			push(p1->u.tensor->elem[i]);
			power();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (istensor(BASE)) {
		p1 = copy_tensor(BASE);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			push(EXPO);
			power();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(M_E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(M_PI);
		BASE = pop();
	}

	if (isnum(BASE) && isnum(EXPO)) {
		power_numbers(BASE, EXPO);
		return;
	}

	// expr^0

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	// 0^expr

	if (iszero(BASE)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// 1^expr

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	// expr^1

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	// BASE is an integer?

	if (isinteger(BASE)) {
		// raise each factor in BASE to power EXPO
		// EXPO is not numerical, that case was handled by power_numbers() above
		h = tos;
		push(BASE);
		factor_factor();
		n = tos - h;
		for (i = 0; i < n; i++) {
			p1 = stack[h + i];
			if (car(p1) == symbol(POWER)) {
				push_symbol(POWER);
				push(cadr(p1)); // base
				push(caddr(p1)); // expo
				push(EXPO);
				multiply();
				list(3);
			} else {
				push_symbol(POWER);
				push(p1);
				push(EXPO);
				list(3);
			}
			stack[h + i] = pop();
		}
		if (n > 1) {
			sort_factors(n);
			list(n);
			push_symbol(MULTIPLY);
			swap();
			cons(); // prepend MULTIPLY to list
		}
		return;
	}

	// BASE is a numerical fraction?

	if (isfraction(BASE)) {
		// power numerator, power denominator
		// EXPO is not numerical, that case was handled by power_numbers() above
		push(BASE);
		numerator();
		push(EXPO);
		power();
		push(BASE);
		denominator();
		push(EXPO);
		negate();
		power();
		multiply();
		return;
	}

	// BASE = e ?

	if (BASE == symbol(EXP1)) {
		power_natural_number(EXPO);
		return;
	}

	// (a + b) ^ c

	if (car(BASE) == symbol(ADD)) {
		power_sum(BASE, EXPO);
		return;
	}

	// (a b) ^ c  -->  (a ^ c) (b ^ c)

	if (car(BASE) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(BASE);
		while (iscons(p1)) {
			push(car(p1));
			push(EXPO);
			power();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	// (a ^ b) ^ c  -->  a ^ (b c)

	if (car(BASE) == symbol(POWER)) {
		push(cadr(BASE));
		push(caddr(BASE));
		push(EXPO);
		multiply_expand(); // always expand products of exponents
		power();
		return;
	}

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}

// BASE and EXPO are rational or double

void
power_numbers(struct atom *BASE, struct atom *EXPO)
{
	int h, i, j, n;
	uint32_t *a, *b;
	struct atom *p1, *p2;

	// n^0

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	// 0^n

	if (iszero(BASE)) {
		if (isnegativenumber(EXPO))
			stopf("divide by zero");
		push_integer(0);
		return;
	}

	// 1^n

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	// n^1

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	if (isdouble(BASE) || isdouble(EXPO)) {
		power_double(BASE, EXPO);
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		a = mpow(BASE->u.q.a, EXPO->u.q.a);
		b = mpow(BASE->u.q.b, EXPO->u.q.a);
		if (isnegativenumber(BASE) && (EXPO->u.q.a[0] & 1))
			if (isnegativenumber(EXPO))
				push_bignum(MMINUS, b, a); // reciprocate
			else
				push_bignum(MMINUS, a, b);
		else
			if (isnegativenumber(EXPO))
				push_bignum(MPLUS, b, a); // reciprocate
			else
				push_bignum(MPLUS, a, b);
		return;
	}

	// exponent is a root

	h = tos;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor_factor();

	// normalize factors

	n = tos - h; // fix n now, stack can grow

	for (i = 0; i < n; i++) {
		p1 = stack[h + i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_numbers_factor(BASE, EXPO);
			stack[h + i] = pop(); // fill hole
		}
	}

	// combine numbers (leaves radicals on stack)

	p1 = one;

	for (i = h; i < tos; i++) {
		p2 = stack[i];
		if (isnum(p2)) {
			push(p1);
			push(p2);
			multiply();
			p1 = pop();
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			tos--;
			i--;
		}
	}

	// finalize

	n = tos - h;

	if (n == 0 || !isplusone(p1)) {
		push(p1);
		n++;
	}

	if (n == 1)
		return;

	sort_factors(n);
	list(n);
	push_symbol(MULTIPLY);
	swap();
	cons(); // prepend MULTIPLY to list
}

// BASE is an integer

void
power_numbers_factor(struct atom *BASE, struct atom *EXPO)
{
	uint32_t *a, *b, *n, *q, *r;
	struct atom *p0;

	if (isminusone(BASE)) {
		power_minusone(EXPO);
		p0 = pop();
		if (car(p0) == symbol(MULTIPLY)) {
			p0 = cdr(p0);
			while (iscons(p0)) {
				push(car(p0));
				p0 = cdr(p0);
			}
		} else
			push(p0);
		return;
	}

	if (isinteger(EXPO)) {
		a = mpow(BASE->u.q.a, EXPO->u.q.a);
		b = mint(1);
		if (isnegativenumber(EXPO))
			push_bignum(MPLUS, b, a); // reciprocate
		else
			push_bignum(MPLUS, a, b);
		return;
	}

	// EXPO.a          r
	// ------ == q + ------
	// EXPO.b        EXPO.b

	q = mdiv(EXPO->u.q.a, EXPO->u.q.b);
	r = mmod(EXPO->u.q.a, EXPO->u.q.b);

	// process q

	if (!MZERO(q)) {
		a = mpow(BASE->u.q.a, q);
		b = mint(1);
		if (isnegativenumber(EXPO))
			push_bignum(MPLUS, b, a); // reciprocate
		else
			push_bignum(MPLUS, a, b);
	}

	mfree(q);

	// process r

	if (MLENGTH(BASE->u.q.a) == 1 && BASE->u.q.a[0] <= 0x7fffffff) {
		// BASE is less than 2^31, hence BASE is a prime number, no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO->sign, r, mcopy(EXPO->u.q.b)); // r used here, r is not leaked
		list(3);
		return;
	}

	// BASE was too big to factor, try finding root

	n = mroot(BASE->u.q.a, EXPO->u.q.b);

	if (n == NULL) {
		// no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO->sign, r, mcopy(EXPO->u.q.b)); // r used here, r is not leaked
		list(3);
		return;
	}

	// raise n to rth power

	a = mpow(n, r);
	b = mint(1);

	mfree(n);
	mfree(r);

	if (isnegativenumber(EXPO))
		push_bignum(MPLUS, b, a); // reciprocate
	else
		push_bignum(MPLUS, a, b);
}

void
power_double(struct atom *BASE, struct atom *EXPO)
{
	double base, d, expo;

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	if (base > 0.0 || expo == floor(expo)) {
		d = pow(base, expo);
		push_double(d);
		return;
	}

	// BASE is negative and EXPO is fractional

	power_minusone(EXPO);

	if (base == -1.0)
		return;

	d = pow(-base, expo);
	push_double(d);

	multiply();
}

// power -1 to EXPO where EXPO is RATIONAL or DOUBLE

void
power_minusone(struct atom *EXPO)
{
	// optimization for i

	if (isequalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		return;
	}

	// root is an odd number?

	if (isrational(EXPO) && EXPO->u.q.b[0] & 1) {
		if (EXPO->u.q.a[0] & 1)
			push_integer(-1);
		else
			push_integer(1);
		return;
	}

	if (isrational(EXPO)) {
		normalize_clock_rational(EXPO);
		return;
	}

	if (isdouble(EXPO)) {
		normalize_clock_double(EXPO);
		rect();
		return;
	}

	push_symbol(POWER);
	push_integer(-1);
	push(EXPO);
	list(3);
}

void
normalize_clock_rational(struct atom *EXPO)
{
	int n;
	struct atom *R;

	// R = EXPO mod 2

	push(EXPO);
	push_integer(2);
	modfunc();
	R = pop();

	// convert negative rotation to positive

	if (R->sign == MMINUS) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floorfunc();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(-1, 2);
	multiply();
	add();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(-1, 2);
			add();
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(-1, 2);
			add();
			list(3);
		}
		break;
	}
}

void
normalize_clock_double(struct atom *EXPO)
{
	double expo, n, r;

	expo = EXPO->u.d;

	// expo = expo mod 2

	expo = fmod(expo, 2.0);

	// convert negative rotation to positive

	if (expo < 0.0)
		expo += 2.0;

	n = floor(2.0 * expo); // number of 90 degree turns

	r = expo - n / 2.0; // remainder

	switch ((int) n) {

	case 0:
		if (r == 0.0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
		}
		break;

	case 1:
		if (r == 0.0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (r == 0.0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0.0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
		}
		break;
	}
}

void
power_natural_number(struct atom *EXPO)
{
	double x, y;

	// exp(x + i y) = exp(x) (cos(y) + i sin(y))

	if (isdoublez(EXPO)) {
		if (car(EXPO) == symbol(ADD)) {
			x = cadr(EXPO)->u.d;
			y = cadaddr(EXPO)->u.d;
		} else {
			x = 0.0;
			y = cadr(EXPO)->u.d;
		}
		push_double(exp(x));
		push_double(y);
		cosfunc();
		push(imaginaryunit);
		push_double(y);
		sinfunc();
		multiply();
		add();
		multiply();
		return;
	}

	// e^log(expr) = expr

	if (car(EXPO) == symbol(LOG)) {
		push(cadr(EXPO));
		return;
	}

	if (isdenormalpolar(EXPO)) {
		normalize_polar(EXPO);
		return;
	}

	push_symbol(POWER);
	push_symbol(EXP1);
	push(EXPO);
	list(3);
}

void
normalize_polar(struct atom *EXPO)
{
	int h;
	struct atom *p1;
	if (car(EXPO) == symbol(ADD)) {
		h = tos;
		p1 = cdr(EXPO);
		while (iscons(p1)) {
			EXPO = car(p1);
			if (isdenormalpolarterm(EXPO))
				normalize_polar_term(EXPO);
			else {
				push_symbol(POWER);
				push_symbol(EXP1);
				push(EXPO);
				list(3);
			}
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
	} else
		normalize_polar_term(EXPO);
}

void
normalize_polar_term(struct atom *EXPO)
{
	struct atom *R;

	// exp(i pi) = -1

	if (lengthf(EXPO) == 3) {
		push_integer(-1);
		return;
	}

	R = cadr(EXPO); // R = coeff of term

	if (isrational(R))
		normalize_polar_term_rational(R);
	else
		normalize_polar_term_double(R);
}

void
normalize_polar_term_rational(struct atom *R)
{
	int n;

	// R = R mod 2

	push(R);
	push_integer(2);
	modfunc();
	R = pop();

	// convert negative rotation to positive

	if (R->sign == MMINUS) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floorfunc();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(-1, 2);
	multiply();
	add();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}

void
normalize_polar_term_double(struct atom *R)
{
	double coeff, n, r;

	coeff = R->u.d;

	// coeff = coeff mod 2

	coeff = fmod(coeff, 2.0);

	// convert negative rotation to positive

	if (coeff < 0.0)
		coeff += 2.0;

	n = floor(2.0 * coeff); // number of 90 degree turns

	r = coeff - n / 2.0; // remainder

	switch ((int) n) {

	case 0:
		if (r == 0.0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 1:
		if (r == 0.0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (r == 0.0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0.0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}

// BASE is a sum of terms

void
power_sum(struct atom *BASE, struct atom *EXPO)
{
	int h, i, n;
	struct atom *p1, *p2;

	if (iscomplexnumber(BASE) && isnum(EXPO)) {
		power_complex_number(BASE, EXPO);
		return;
	}

	if (expanding == 0 || !issmallinteger(EXPO) || isnegativenumber(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);
	n = pop_integer();

	// square the sum first (prevents infinite loop through multiply)

	h = tos;

	p1 = cdr(BASE);

	while (iscons(p1)) {
		p2 = cdr(BASE);
		while (iscons(p2)) {
			push(car(p1));
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		p1 = cdr(p1);
	}

	add_terms(tos - h);

	// continue up to power n

	for (i = 2; i < n; i++) {
		push(BASE);
		multiply();
	}
}

// BASE is rectangular complex numerical, EXPO is numerical

void
power_complex_number(struct atom *BASE, struct atom *EXPO)
{
	int n;
	struct atom *X, *Y;

	// prefixform(2+3*i) = (add 2 (multiply 3 (power -1 1/2)))

	// prefixform(1+i) = (add 1 (power -1 1/2))

	// prefixform(3*i) = (multiply 3 (power -1 1/2))

	// prefixform(i) = (power -1 1/2)

	if (car(BASE) == symbol(ADD)) {
		X = cadr(BASE);
		if (caaddr(BASE) == symbol(MULTIPLY))
			Y = cadaddr(BASE);
		else
			Y = one;
	} else if (car(BASE) == symbol(MULTIPLY)) {
		X = zero;
		Y = cadr(BASE);
	} else {
		X = zero;
		Y = one;
	}

	if (isdouble(X) || isdouble(Y) || isdouble(EXPO)) {
		power_complex_double(X, Y, EXPO);
		return;
	}

	if (!isinteger(EXPO)) {
		power_complex_rational(X, Y, EXPO);
		return;
	}

	if (!issmallinteger(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);
	n = pop_integer();

	if (n > 0)
		power_complex_plus(X, Y, n);
	else if (n < 0)
		power_complex_minus(X, Y, -n);
	else
		push_integer(1);
}

void
power_complex_plus(struct atom *X, struct atom *Y, int n)
{
	int i;
	struct atom *PX, *PY;

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i*Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}

//
//               /        \  n
//         -n   |  X - iY  |
// (X + iY)   = | -------- |
//              |   2   2  |
//               \ X + Y  /

// X and Y are rational numbers

void
power_complex_minus(struct atom *X, struct atom *Y, int n)
{
	int i;
	struct atom *PX, *PY, *R;

	// R = X^2 + Y^2

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	R = pop();

	// X = X / R

	push(X);
	push(R);
	divide();
	X = pop();

	// Y = -Y / R

	push(Y);
	negate();
	push(R);
	divide();
	Y = pop();

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i*Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}

void
power_complex_double(struct atom *X, struct atom *Y, struct atom *EXPO)
{
	double expo, r, theta, x, y;

	push(EXPO);
	expo = pop_double();

	push(X);
	x = pop_double();

	push(Y);
	y = pop_double();

	r = hypot(x, y);
	theta = atan2(y, x);

	r = pow(r, expo);
	theta = expo * theta;

	x = r * cos(theta);
	y = r * sin(theta);

	push_double(x);
	push_double(y);
	push(imaginaryunit);
	multiply();
	add();
}

// X and Y are rational, EXPO is rational and not an integer

void
power_complex_rational(struct atom *X, struct atom *Y, struct atom *EXPO)
{
	// calculate sqrt(X^2 + Y^2) ^ (1/2 * EXPO)

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	push_rational(1, 2);
	push(EXPO);
	multiply();
	power();

	// calculate (-1) ^ (EXPO * arctan(Y, X) / pi)

	push(Y);
	push(X);
	arctan();
	push_symbol(PI);
	divide();
	push(EXPO);
	multiply();
	EXPO = pop();
	power_minusone(EXPO);

	// result = sqrt(X^2 + Y^2) ^ (1/2 * EXPO) * (-1) ^ (EXPO * arctan(Y, X) / pi)

	multiply();
}
void
eval_prefixform(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	outbuf_init();
	prefixform(p1);

	push_string(outbuf);
}

// for debugging

void
print_prefixform(struct atom *p)
{
	outbuf_init();
	prefixform(p);
	outbuf_puts("\n");
	printbuf(outbuf, BLACK);
}

void
prefixform(struct atom *p)
{
	char *s;
	switch (p->atomtype) {
	case CONS:
		outbuf_puts("(");
		prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(" ");
			prefixform(car(p));
			p = cdr(p);
		}
		if (p != symbol(NIL)) {
			outbuf_puts(" . ");
			prefixform(p);
		}
		outbuf_puts(")");
		break;
	case STR:
		outbuf_puts("\"");
		outbuf_puts(p->u.str);
		outbuf_puts("\"");
		break;
	case RATIONAL:
		if (p->sign == MMINUS)
			outbuf_puts("-");
		s = mstr(p->u.q.a);
		outbuf_puts(s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		outbuf_puts("/");
		outbuf_puts(s);
		break;
	case DOUBLE:
		snprintf(strbuf, STRBUFLEN, "%g", p->u.d);
		outbuf_puts(strbuf);
		if (!strchr(strbuf, '.') && !strchr(strbuf, 'e'))
			outbuf_puts(".0");
		break;
	case KSYM:
	case USYM:
		outbuf_puts(printname(p));
		break;
	case TENSOR:
		outbuf_puts("(tensor)");
		break;
	default:
		outbuf_puts("(?)");
		break;
	}
}
void
eval_print(struct atom *p1)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(car(p1));
		evalf();
		print_result();
		p1 = cdr(p1);
	}
	push_symbol(NIL);
}

void
print_result(void)
{
	struct atom *p1, *p2;

	p2 = pop(); // result
	p1 = pop(); // input

	if (p2 == symbol(NIL))
		return;

	if (annotate_result(p1, p2)) {
		push_symbol(SETQ);
		push(p1);
		push(p2);
		list(3);
		p2 = pop();
	}

	p1 = get_binding(symbol(TTY));

	if (p1 == symbol(TTY) || iszero(p1)) {
		push(p2);
		display();
	} else
		print_infixform(p2);
}

// returns 1 if result should be annotated

int
annotate_result(struct atom *p1, struct atom *p2)
{
	if (!isusersymbol(p1))
		return 0;

	if (p1 == p2)
		return 0; // A = A

	if (p1 == symbol(I_LOWER) && isimaginaryunit(p2))
		return 0;

	if (p1 == symbol(J_LOWER) && isimaginaryunit(p2))
		return 0;

	return 1;
}
void
eval_product(struct atom *p1)
{
	int h, i, j, k, n;
	struct atom *p2, *p3;

	// product of tensor elements?

	if (lengthf(p1) == 2) {
		push(cadr(p1));
		evalf();
		p1 = pop();
		if (!istensor(p1)) {
			push(p1);
			return;
		}
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			push(p1->u.tensor->elem[i]);
		multiply_factors(n);
		return;
	}

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("product: index symbol err");

	push(caddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("product: index range err");
	push(p3);
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("product: index range err");
	push(p3);
	k = pop_integer();

	p1 = caddddr(p1);

	save_symbol(p2);

	h = tos;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		evalg();
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
		if (tos - h == 1000)
			multiply_factors(1000); // to prevent stack overflow
	}

	multiply_factors(tos - h);

	restore_symbol();
}
void
eval_quote(struct atom *p1)
{
	push(cadr(p1)); // not evaluated
}
void
eval_rank(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();
	if (istensor(p1))
		push_integer(p1->u.tensor->ndim);
	else
		push_integer(0);
}
void
eval_rationalize(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	rationalize();
}

void
rationalize(void)
{
	int i, n;
	struct atom *p0, *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			rationalize();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	p2 = one;

	while (find_divisor(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		multiply_noexpand();
		p2 = pop();
	}

	push(p1);
	push(p2);
	reciprocate();
	multiply_noexpand();
}

// returns 1 with divisor on stack, otherwise returns 0

int
find_divisor(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (find_divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return find_divisor_term(p);
}

int
find_divisor_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (find_divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return find_divisor_factor(p);
}

int
find_divisor_factor(struct atom *p)
{
	if (isinteger(p))
		return 0;

	if (isrational(p)) {
		push(p);
		denominator();
		return 1;
	}

	if (car(p) == symbol(POWER) && !isminusone(cadr(p)) && isnegativeterm(caddr(p))) {
		if (isminusone(caddr(p)))
			push(cadr(p));
		else {
			push_symbol(POWER);
			push(cadr(p));
			push(caddr(p));
			negate();
			list(3);
		}
		return 1;
	}

	return 0;
}

void
cancel_factor(void)
{
	int h;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (car(p2) == symbol(ADD)) {
		h = tos;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(p1);
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		add_terms(tos - h);
		return;
	}

	push(p1);
	push(p2);
	multiply();
}
void
eval_real(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	real();
}

void
real(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			real();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rect();
	p1 = pop();
	push(p1);
	push(p1);
	conjfunc();
	add();
	push_rational(1, 2);
	multiply();
}
void
eval_rect(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	rect();
}

void
rect(void)
{
	int h, i, n;
	struct atom *p1, *p2, *BASE, *EXPO;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			rect();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	if (car(p1) != symbol(POWER)) {
		push(p1);
		return;
	}

	BASE = cadr(p1);
	EXPO = caddr(p1);

	// handle sum in exponent

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		h = tos;
		while (iscons(p1)) {
			push_symbol(POWER);
			push(BASE);
			push(car(p1));
			list(3);
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	// return mag(p1) * cos(arg(p1)) + i sin(arg(p1)))

	push(p1);
	mag();

	push(p1);
	arg();
	p2 = pop();

	push(p2);
	cosfunc();

	push(imaginaryunit);
	push(p2);
	sinfunc();
	multiply();

	add();

	multiply();
}
void
eval_roots(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
	} else
		push_symbol(X_LOWER);

	roots();
}

void
roots(void)
{
	int h, i, j, k, n;
	struct atom *A, *P, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	k = tos;

	n = k - h; // number of coeffs on stack

	// check coeffs

	for (i = 0; i < n; i++)
		if (!isrational(stack[h + i]))
			stopf("roots: coeffs");

	// find roots

	while (n > 1) {

		if (findroot(h, n) == 0)
			break; // no root found

		// A is the root

		A = stack[tos - 1];

		// divide p(x) by X - A

		reduce(h, n, A);

		n--;
	}

	n = tos - k; // number of roots on stack

	if (n == 0) {
		tos = h; // pop all
		push_symbol(NIL); // no roots
		return;
	}

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[k + i], stack[k + i + 1])) {
			for (j = i + 1; j < n - 1; j++)
				stack[k + j] = stack[k + j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		A = stack[k];
		tos = h; // pop all
		push(A); // one root
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[k + i];

	tos = h; // pop all

	push(A);
}

int
findroot(int h, int n)
{
	int i, j, m, p, q, r;
	struct atom *A, *C, *PA;

	// check constant term

	if (iszero(stack[h])) {
		push_integer(0); // root is zero
		return 1;
	}

	// eliminate denominators

	for (i = 0; i < n; i++) {
		C = stack[h + i];
		if (isinteger(C))
			continue;
		push(C);
		denominator();
		C = pop();
		for (j = 0; j < n; j++) {
			push(stack[h + j]);
			push(C);
			multiply();
			stack[h + j] = pop();
		}
	}

	p = tos;

	push(stack[h]);
	m = pop_integer();
	divisors(m); // divisors of constant term

	q = tos;

	push(stack[h + n - 1]);
	m = pop_integer();
	divisors(m); // divisors of leading coeff

	r = tos;

	for (i = p; i < q; i++) {
		for (j = q; j < r; j++) {

			// try positive A

			push(stack[i]);
			push(stack[j]);
			divide();
			A = pop();

			horner(h, n, A);

			PA = pop(); // polynomial evaluated at A

			if (iszero(PA)) {
				tos = p; // pop all
				push(A);
				return 1; // root on stack
			}

			// try negative A

			push(A);
			negate();
			A = pop();

			horner(h, n, A);

			PA = pop(); // polynomial evaluated at A

			if (iszero(PA)) {
				tos = p; // pop all
				push(A);
				return 1; // root on stack
			}
		}
	}

	tos = p; // pop all

	return 0; // no root
}

// evaluate p(x) at x = A using horner's rule

void
horner(int h, int n, struct atom *A)
{
	int i;

	push(stack[h + n - 1]);

	for (i = n - 2; i >= 0; i--) {
		push(A);
		multiply();
		push(stack[h + i]);
		add();
	}
}

// push all divisors of n

void
divisors(int n)
{
	int h, i, k;

	h = tos;

	factor_int(n);

	k = tos;

	// contruct divisors by recursive descent

	push_integer(1);

	divisors_nib(h, k);

	// move

	n = tos - k;

	for (i = 0; i < n; i++)
		stack[h + i] = stack[k + i];

	tos = h + n; // pop all
}

//	Generate all divisors for a factored number
//
//	Input:		Factor pairs on stack (base, expo)
//
//			h	first pair
//
//			k	just past last pair
//
//	Output:		Divisors on stack
//
//	For example, the number 12 (= 2^2 3^1) has 6 divisors:
//
//	1, 2, 3, 4, 6, 12

void
divisors_nib(int h, int k)
{
	int i, n;
	struct atom *ACCUM, *BASE, *EXPO;

	if (h == k)
		return;

	ACCUM = pop();

	BASE = stack[h + 0];
	EXPO = stack[h + 1];

	push(EXPO);
	n = pop_integer();

	for (i = 0; i <= n; i++) {
		push(ACCUM);
		push(BASE);
		push_integer(i);
		power();
		multiply();
		divisors_nib(h + 2, k);
	}
}

// divide by X - A

void
reduce(int h, int n, struct atom *A)
{
	int i;

	for (i = n - 1; i > 0; i--) {
		push(A);
		push(stack[h + i]);
		multiply();
		push(stack[h + i - 1]);
		add();
		stack[h + i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stopf("roots: residual error"); // not a root

	// move

	for (i = 0; i < n - 1; i++)
		stack[h + i] = stack[h + i + 1];
}

// push coefficients of polynomial P(X) on stack

void
coeffs(struct atom *P, struct atom *X)
{
	struct atom *C;

	for (;;) {

		push(P);
		push(X);
		push_integer(0);
		subst();
		evalf();
		C = pop();

		push(C);

		push(P);
		push(C);
		subtract();
		P = pop();

		if (iszero(P))
			break;

		push(P);
		push(X);
		divide();
		P = pop();
	}
}
#define NUMQBITS PSI->u.tensor->nelem
#define KET0 PSI->u.tensor->elem[i ^ n]
#define KET1 PSI->u.tensor->elem[i]

#define POWEROF2(x) (((x) & ((x) - 1)) == 0)

void
eval_rotate(struct atom *p1)
{
	int m, n;
	uint32_t c;
	struct atom *PSI, *OPCODE, *PHASE;

	push(cadr(p1));
	evalf();
	PSI = pop();

	if (!istensor(PSI) || PSI->u.tensor->ndim > 1 || PSI->u.tensor->nelem > 32768 || !POWEROF2(PSI->u.tensor->nelem))
		stopf("rotate error 1 first argument is not a vector or dimension error");

	c = 0;

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (!iscons(cdr(p1)))
			stopf("rotate error 2 unexpected end of argument list");

		OPCODE = car(p1);
		push(cadr(p1));
		evalf();
		n = pop_integer();

		if (n > 14 || (1 << n) >= PSI->u.tensor->nelem)
			stopf("rotate error 3 qubit number format or range");

		p1 = cddr(p1);

		if (OPCODE == symbol(C_UPPER)) {
			c |= 1 << n;
			continue;
		}

		if (OPCODE == symbol(H_UPPER)) {
			rotate_h(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(P_UPPER)) {
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			push(imaginaryunit);
			multiply();
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(Q_UPPER)) {
			rotate_q(PSI, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(V_UPPER)) {
			rotate_v(PSI, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(W_UPPER)) {
			m = n;
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			n = pop_integer();
			if (n > 14 || (1 << n) >= PSI->u.tensor->nelem)
				stopf("rotate error 3 qubit number format or range");
			rotate_w(PSI, c, m, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(X_UPPER)) {
			rotate_x(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(Y_UPPER)) {
			rotate_y(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol(Z_UPPER)) {
			rotate_z(PSI, c, n);
			c = 0;
			continue;
		}

		stopf("rotate error 4 unknown rotation code");
	}

	push(PSI);
}

// hadamard

void
rotate_h(struct atom *PSI, uint32_t c, int n)
{
	int i;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(KET0);
			push(KET1);
			add();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			push(KET0);
			push(KET1);
			subtract();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			KET1 = pop();
			KET0 = pop();
		}
	}
}

// phase

void
rotate_p(struct atom *PSI, struct atom *PHASE, uint32_t c, int n)
{
	int i;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(KET1);
			push(PHASE);
			multiply();
			KET1 = pop();
		}
	}
}

// swap

void
rotate_w(struct atom *PSI, uint32_t c, int m, int n)
{
	int i;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if ((i & m) && !(i & n)) {
			push(PSI->u.tensor->elem[i]);
			push(PSI->u.tensor->elem[i ^ m ^ n]);
			PSI->u.tensor->elem[i] = pop();
			PSI->u.tensor->elem[i ^ m ^ n] = pop();
		}
	}
}

void
rotate_x(struct atom *PSI, uint32_t c, int n)
{
	int i;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(KET0);
			push(KET1);
			KET0 = pop();
			KET1 = pop();
		}
	}
}

void
rotate_y(struct atom *PSI, uint32_t c, int n)
{
	int i;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(imaginaryunit);
			negate();
			push(KET0);
			multiply();
			push(imaginaryunit);
			push(KET1);
			multiply();
			KET0 = pop();
			KET1 = pop();
		}
	}
}

void
rotate_z(struct atom *PSI, uint32_t c, int n)
{
	int i;
	n = 1 << n;
	for (i = 0; i < NUMQBITS; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(KET1);
			negate();
			KET1 = pop();
		}
	}
}

// quantum fourier transform

void
rotate_q(struct atom *PSI, int n)
{
	int i, j;
	struct atom *PHASE;
	for (i = n; i >= 0; i--) {
		rotate_h(PSI, 0, i);
		for (j = 0; j < i; j++) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			evalf();
			multiply_factors(3);
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, 1 << j, i);
		}
	}
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(PSI, 0, i, n - i);
}

// inverse qft

void
rotate_v(struct atom *PSI, int n)
{
	int i, j;
	struct atom *PHASE;
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(PSI, 0, i, n - i);
	for (i = 0; i <= n; i++) {
		for (j = i - 1; j >= 0; j--) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			evalf();
			multiply_factors(3);
			negate();
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, 1 << j, i);
		}
		rotate_h(PSI, 0, i);
	}
}
void
eval_run(struct atom *p1)
{
	char *buf;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p1 = pop();
	if (!isstr(p1))
		stopf("run: file name expected");

	p2 = alloc_str();
	buf = read_file(p1->u.str);
	if (buf == NULL)
		stopf("run: cannot read file");
	p2->u.str = buf;

	fpush(p2); // make visible to garbage collector
	run_buf(buf);
	fpop(); // buf is freed on next gc

	push_symbol(NIL); // return value
}

char *
read_file(char *filename)
{
	int fd, n;
	char *buf;
	off_t t;

	fd = open(filename, O_RDONLY);

	if (fd < 0)
		return NULL;

	t = lseek(fd, 0, SEEK_END);

	if (t < 0 || t > 0x1000000) { // 16 MB max
		close(fd);
		return NULL;
	}

	if (lseek(fd, 0, SEEK_SET)) {
		close(fd);
		return NULL;
	}

	n = (int) t;

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		return NULL;
	}

	if (read(fd, buf, n) != n) {
		free(buf);
		close(fd);
		return NULL;
	}

	close(fd);

	buf[n] = '\0';

	return buf;
}
void
eval_setq(struct atom *p1)
{
	struct atom *p2;

	push_symbol(NIL); // return value

	if (caadr(p1) == symbol(INDEX)) {
		setq_indexed(p1);
		return;
	}

	if (iscons(cadr(p1))) {
		setq_usrfunc(p1);
		return;
	}

	if (!isusersymbol(cadr(p1)))
		stopf("user symbol expected");

	push(caddr(p1));
	evalg();
	p2 = pop();

	set_symbol(cadr(p1), p2, symbol(NIL));
}

//	Example: a[1] = b
//
//	p1----->cons--->cons------------------->cons
//		|	|			|
//		setq	cons--->cons--->cons	b
//			|	|	|
//			index	a	1
//
//	caadr(p1) = index
//	cadadr(p1) = a
//	caddr(p1) = b

void
setq_indexed(struct atom *p1)
{
	int h;
	struct atom *S, *LVAL, *RVAL;

	S = cadadr(p1);

	if (!isusersymbol(S))
		stopf("user symbol expected");

	push(S);
	evalg();

	push(caddr(p1));
	evalg();

	RVAL = pop();
	LVAL = pop();

	// eval indices

	p1 = cddadr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	set_component(LVAL, RVAL, h);

	set_symbol(S, LVAL, symbol(NIL));
}

void
set_component(struct atom *LVAL, struct atom *RVAL, int h)
{
	int i, k, m, n, t;

	if (!istensor(LVAL))
		stopf("index error");

	// n is the number of indices

	n = tos - h;

	if (n < 1 || n > LVAL->u.tensor->ndim)
		stopf("index error");

	// k is the combined index

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL->u.tensor->dim[i])
			stopf("index error");
		k = k * LVAL->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (istensor(RVAL)) {
		m = RVAL->u.tensor->ndim;
		if (n + m != LVAL->u.tensor->ndim)
			stopf("index error");
		for (i = 0; i < m; i++)
			if (LVAL->u.tensor->dim[n + i] != RVAL->u.tensor->dim[i])
				stopf("index error");
		m = RVAL->u.tensor->nelem;
		for (i = 0; i < m; i++)
			LVAL->u.tensor->elem[m * k + i] = RVAL->u.tensor->elem[i];
	} else {
		if (n != LVAL->u.tensor->ndim)
			stopf("index error");
		LVAL->u.tensor->elem[k] = RVAL;
	}
}

// Example:
//
//      f(x,y)=x^y
//
// For this definition, p1 points to the following structure.
//
//     p1
//      |
//   ___v__    ______                        ______
//  |CONS  |->|CONS  |--------------------->|CONS  |
//  |______|  |______|                      |______|
//      |         |                             |
//   ___v__    ___v__    ______    ______    ___v__    ______    ______
//  |SETQ  |  |CONS  |->|CONS  |->|CONS  |  |CONS  |->|CONS  |->|CONS  |
//  |______|  |______|  |______|  |______|  |______|  |______|  |______|
//                |         |         |         |         |         |
//             ___v__    ___v__    ___v__    ___v__    ___v__    ___v__
//            |SYM f |  |SYM x |  |SYM y |  |POWER |  |SYM x |  |SYM y |
//            |______|  |______|  |______|  |______|  |______|  |______|
//
// We have
//
//	caadr(p1) points to f
//	cdadr(p1) points to the list (x y)
//	caddr(p1) points to (power x y)

void
setq_usrfunc(struct atom *p1)
{
	struct atom *F, *A, *B, *C;

	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!isusersymbol(F))
		stopf("user symbol expected");

	if (lengthf(A) > 9)
		stopf("more than 9 arguments");

	push(B);
	convert_body(A);
	C = pop();

	set_symbol(F, B, C);
}

void
convert_body(struct atom *A)
{
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG1);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG2);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG3);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG4);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG5);
	subst();

	A = cdr(A);

	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG6);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG7);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG8);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG9);
	subst();
}
void
eval_sgn(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sgn();
}

void
sgn(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			sgn();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!isnum(p1)) {
		push_symbol(SGN);
		push(p1);
		list(2);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativenumber(p1))
		push_integer(-1);
	else
		push_integer(1);
}
void
eval_simplify(struct atom *p1)
{
	push(cadr(p1));
	evalg();
	simplify();
}

void
simplify(void)
{
	struct atom *p1;
	p1 = pop();
	if (istensor(p1))
		simplify_tensor(p1);
	else
		simplify_scalar(p1);
}

void
simplify_tensor(struct atom *p1)
{
	int i, n;
	p1 = copy_tensor(p1);
	push(p1); // make visible to gc
	n = p1->u.tensor->nelem;
	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		simplify();
		p1->u.tensor->elem[i] = pop();
	}
}

void
simplify_scalar(struct atom *p1)
{
	int h;

	// already simple?

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	fpush(p1); // make visible to gc

	h = tos;
	push(car(p1));
	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		simplify();
		p1 = cdr(p1);
	}

	fpop();

	list(tos - h);
	evalg();

	simplify_pass1();
	simplify_pass2(); // try exponential form
	simplify_pass3(); // try polar form
}

void
simplify_pass1(void)
{
	struct atom *p1, *NUM, *DEN, *R, *T;

	p1 = pop();

	// already simple?

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rationalize();
		T = pop();
		if (car(T) == symbol(ADD)) {
			push(p1); // no change
			return;
		}
	} else
		T = p1;

	push(T);
	numerator();
	NUM = pop();

	push(T);
	denominator();
	evalf(); // to expand denominator
	DEN = pop();

	// if DEN is a sum then rationalize it

	if (car(DEN) == symbol(ADD)) {
		push(DEN);
		rationalize();
		T = pop();
		if (car(T) != symbol(ADD)) {
			// update NUM
			push(T);
			denominator();
			evalf(); // to expand denominator
			push(NUM);
			multiply();
			NUM = pop();
			// update DEN
			push(T);
			numerator();
			DEN = pop();
		}
	}

	// are NUM and DEN congruent sums?

	if (car(NUM) != symbol(ADD) || car(DEN) != symbol(ADD) || lengthf(NUM) != lengthf(DEN)) {
		// no, but NUM over DEN might be simpler than p1
		push(NUM);
		push(DEN);
		divide();
		T = pop();
		if (complexity(T) < complexity(p1))
			p1 = T;
		push(p1);
		return;
	}

	push(cadr(NUM)); // push first term of numerator
	push(cadr(DEN)); // push first term of denominator
	divide();

	R = pop(); // provisional ratio

	push(R);
	push(DEN);
	multiply();

	push(NUM);
	subtract();

	T = pop();

	if (iszero(T))
		p1 = R;

	push(p1);
}

// try exponential form

void
simplify_pass2(void)
{
	struct atom *p1, *p2;

	p1 = pop();

	// already simple?

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	circexp();
	rationalize();
	evalf(); // to normalize
	p2 = pop();

	if (complexity(p1) <= complexity(p2))
		push(p1);
	else
		push(p2);
}

// try polar form

void
simplify_pass3(void)
{
	struct atom *p1, *p2;

	p1 = pop();

	if (car(p1) != symbol(ADD) || !findf(p1, imaginaryunit)) {
		push(p1);
		return;
	}

	push(p1);
	polar();
	p2 = pop();

	if (complexity(p1) <= complexity(p2))
		push(p1);
	else
		push(p2);
}

int
complexity(struct atom *p)
{
	int n = 1;

	while (iscons(p)) {
		n += complexity(car(p));
		p = cdr(p);
	}

	return n;
}
void
eval_sin(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sinfunc();
}

void
sinfunc(void)
{
	int i, n;
	double d;
	struct atom *p1, *p2, *X, *Y;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			sinfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = sin(d);
		push_double(d);
		return;
	}

	// sin(z) = -i/2 exp(i z) + i/2 exp(-i z)

	if (isdoublez(p1)) {
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(p1);
		multiply();
		expfunc();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		expfunc();
		subtract();
		multiply();
		return;
	}

	// sin(-x) = -sin(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		sinfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		sinfunc_sum(p1);
		return;
	}

	// sin(arctan(y,x)) = y (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(Y);
		push(X);
		push(X);
		multiply();
		push(Y);
		push(Y);
		multiply();
		add();
		push_rational(-1, 2);
		power();
		multiply();
		return;
	}

	// sin(arccos(x)) = sqrt(1 - x^2)

	if (car(p1) == symbol(ARCCOS)) {
		push_integer(1);
		push(cadr(p1));
		push_integer(2);
		power();
		subtract();
		push_rational(1, 2);
		power();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push(p2);
		d = pop_double();
		d = sin(d * M_PI);
		push_double(d);
		return;
	}

	push(p2); // nonnegative by sin(-x) = -sin(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 150:
		push_rational(1, 2);
		break;
	case 210:
	case 330:
		push_rational(-1, 2);
		break;
	case 45:
	case 135:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 225:
	case 315:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 60:
	case 120:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 240:
	case 300:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 90:
		push_integer(1);
		break;
	case 270:
		push_integer(-1);
		break;
	default:
		push_symbol(SIN);
		push(p1);
		list(2);
		break;
	}
}

// sin(x + n/2 pi) = sin(x) cos(n/2 pi) + cos(x) sin(n/2 pi)

void
sinfunc_sum(struct atom *p1)
{
	struct atom *p2, *p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push_integer(2);
		push(car(p2));
		multiply();
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			p3 = pop();
			push(p3);
			sinfunc();
			push(car(p2));
			cosfunc();
			multiply();
			push(p3);
			cosfunc();
			push(car(p2));
			sinfunc();
			multiply();
			add();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(SIN);
	push(p1);
	list(2);
}
void
eval_sinh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sinhfunc();
}

void
sinhfunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			sinhfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = sinh(d);
		push_double(d);
		return;
	}

	// sinh(z) = 1/2 exp(z) - 1/2 exp(-z)

	if (isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		expfunc();
		push(p1);
		negate();
		expfunc();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sinh(-x) -> -sinh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		sinhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(SINH);
	push(p1);
	list(2);
}
void
eval_sqrt(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sqrtfunc();
}

void
sqrtfunc(void)
{
	push_rational(1, 2);
	power();
}
void
eval_status(struct atom *p1)
{
	(void) p1; // silence compiler

	outbuf_init();

	snprintf(strbuf, STRBUFLEN, "block_count %d (%d%%)\n", block_count, 100 * block_count / MAXBLOCKS);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "free_count %d\n", free_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "gc_count %d\n", gc_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "bignum_count %d\n", bignum_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "ksym_count %d\n", ksym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "usym_count %d\n", usym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "string_count %d\n", string_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "tensor_count %d\n", tensor_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_eval_level %d\n", max_eval_level);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_tos %d (%d%%)\n", max_tos, 100 * max_tos / STACKSIZE);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_tof %d (%d%%)\n", max_tof, 100 * max_tof / FRAMESIZE);
	outbuf_puts(strbuf);

	printbuf(outbuf, BLACK);

	push_symbol(NIL);
}
void
eval_stop(struct atom *p1)
{
	(void) p1; // silence compiler
	stopf("stop function");
}
void
eval_subst(struct atom *p1)
{
	push(cadddr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	push(cadr(p1));
	evalf();
	subst();
	evalf(); // normalize
}

// cannot do any evalf in subst because subst is used by func defn

void
subst(void)
{
	int h, i, n;
	struct atom *p1, *p2, *p3;

	p3 = pop(); // new expr
	p2 = pop(); // old expr
	p1 = pop(); // expr

	if (p2 == symbol(NIL) || p3 == symbol(NIL)) {
		push(p1);
		return;
	}

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			push(p2);
			push(p3);
			subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (equal(p1, p2)) {
		push(p3);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	// depth first

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		push(p3);
		subst();
		p1 = cdr(p1);
	}

	list(tos - h);
}
void
eval_sum(struct atom *p1)
{
	int h, i, j, k, n;
	struct atom *p2, *p3;

	// sum over tensor elements?

	if (lengthf(p1) == 2) {
		push(cadr(p1));
		evalf();
		p1 = pop();
		if (!istensor(p1)) {
			push(p1);
			return;
		}
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			push(p1->u.tensor->elem[i]);
		add_terms(n);
		return;
	}

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("sum: index symbol err");

	push(caddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("sum: index range err");
	push(p3);
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("sum: index range err");
	push(p3);
	k = pop_integer();

	p1 = caddddr(p1);

	save_symbol(p2);

	h = tos;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		evalg();
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
		if (tos - h == 1000)
			add_terms(1000); // to prevent stack overflow
	}

	add_terms(tos - h);

	restore_symbol();
}
void
eval_tan(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	tanfunc();
}

void
tanfunc(void)
{
	int i, n;
	double d;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			tanfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = tan(d);
		push_double(d);
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		sinfunc();
		push(p1);
		cosfunc();
		divide();
		return;
	}

	// tan(-x) = -tan(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		tanfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		tanfunc_sum(p1);
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		push(cadr(p1));
		push(caddr(p1));
		divide();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(TAN);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push(p2);
		d = pop_double();
		d = tan(d * M_PI);
		push_double(d);
		return;
	}

	push(p2); // nonnegative by tan(-x) = -tan(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(TAN);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 210:
		push_rational(1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 330:
		push_rational(-1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 45:
	case 225:
		push_integer(1);
		break;
	case 135:
	case 315:
		push_integer(-1);
		break;
	case 60:
	case 240:
		push_integer(3);
		push_rational(1, 2);
		power();
		break;
	case 120:
	case 300:
		push_integer(3);
		push_rational(1, 2);
		power();
		negate();
		break;
	default:
		push_symbol(TAN);
		push(p1);
		list(2);
		break;
	}
}

// tan(x + n pi) = tan(x)

void
tanfunc_sum(struct atom *p1)
{
	struct atom *p2, *p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push(car(p2));
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			tanfunc();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(TAN);
	push(p1);
	list(2);
}
void
eval_tanh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	tanhfunc();
}

void
tanhfunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			tanhfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = tanh(d);
		push_double(d);
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		sinhfunc();
		push(p1);
		coshfunc();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		tanhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}
void
eval_taylor(struct atom *p1)
{
	int h, i, n;
	struct atom *F, *X, *A, *C;

	push(cadr(p1));
	evalf();
	F = pop();

	push(caddr(p1));
	evalf();
	X = pop();

	push(cadddr(p1));
	evalf();
	n = pop_integer();

	p1 = cddddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
	} else
		push_integer(0); // default expansion point

	A = pop();

	h = tos;

	push(F);	// f(a)
	push(X);
	push(A);
	subst();
	evalf();

	push_integer(1);
	C = pop();

	for (i = 1; i <= n; i++) {

		push(F);	// f = f'
		push(X);
		derivative();
		F = pop();

		if (iszero(F))
			break;

		push(C);	// c = c * (x - a)
		push(X);
		push(A);
		subtract();
		multiply();
		C = pop();

		push(F);	// f(a)
		push(X);
		push(A);
		subst();
		evalf();

		push(C);
		multiply();
		push_integer(i);
		factorial();
		divide();
	}

	add_terms(tos - h);
}
void
eval_tensor(struct atom *p1)
{
	int i;

	p1 = copy_tensor(p1);

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		push(p1->u.tensor->elem[i]);
		evalf();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);

	promote_tensor();
}

// tensors with elements that are also tensors get promoted to a higher rank

void
promote_tensor(void)
{
	int i, j, k, ndim1, ndim2, nelem1, nelem2;
	struct atom *p1, *p2, *p3;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim1 = p1->u.tensor->ndim;
	nelem1 = p1->u.tensor->nelem;

	// check

	p2 = p1->u.tensor->elem[0];

	for (i = 1; i < nelem1; i++) {
		p3 = p1->u.tensor->elem[i];
		if (!compatible_dimensions(p2, p3))
			stopf("tensor dimensions");
	}

	if (!istensor(p2)) {
		push(p1);
		return; // all elements are scalars
	}

	ndim2 = p2->u.tensor->ndim;
	nelem2 = p2->u.tensor->nelem;

	if (ndim1 + ndim2 > MAXDIM)
		stopf("rank exceeds max");

	// alloc

	p3 = alloc_tensor(nelem1 * nelem2);

	// merge dimensions

	k = 0;

	for (i = 0; i < ndim1; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	for (i = 0; i < ndim2; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	p3->u.tensor->ndim = ndim1 + ndim2;

	// merge elements

	k = 0;

	for (i = 0; i < nelem1; i++) {
		p2 = p1->u.tensor->elem[i];
		for (j = 0; j < nelem2; j++)
			p3->u.tensor->elem[k++] = p2->u.tensor->elem[j];
	}

	push(p3);
}

int
compatible_dimensions(struct atom *p, struct atom *q)
{
	int i, n;

	if (!istensor(p) && !istensor(q))
		return 1; // both p and q are scalars

	if (!istensor(p) || !istensor(q))
		return 0; // scalar and tensor

	n = p->u.tensor->ndim;

	if (n != q->u.tensor->ndim)
		return 0;

	for (i = 0; i < n; i++)
		if (p->u.tensor->dim[i] != q->u.tensor->dim[i])
			return 0;

	return 1;
}

struct atom *
copy_tensor(struct atom *p1)
{
	int i;
	struct atom *p2;

	p2 = alloc_tensor(p1->u.tensor->nelem);

	p2->u.tensor->ndim = p1->u.tensor->ndim;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		p2->u.tensor->dim[i] = p1->u.tensor->dim[i];

	for (i = 0; i < p1->u.tensor->nelem; i++)
		p2->u.tensor->elem[i] = p1->u.tensor->elem[i];

	return p2;
}
void
eval_test(struct atom *p1)
{
	struct atom *p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (!iscons(cdr(p1))) {
			push(car(p1)); // default case
			evalf();
			return;
		}
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push(cadr(p1));
			evalf();
			return;
		}
		p1 = cddr(p1);
	}
	push_symbol(NIL);
}

void
eval_testeq(struct atom *p1)
{
	push(cadr(p1));
	evalg();
	push(caddr(p1));
	evalg();
	subtract();
	simplify();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testge(struct atom *p1)
{
	if (cmp_args(p1) >= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testgt(struct atom *p1)
{
	if (cmp_args(p1) > 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testle(struct atom *p1)
{
	if (cmp_args(p1) <= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testlt(struct atom *p1)
{
	if (cmp_args(p1) < 0)
		push_integer(1);
	else
		push_integer(0);
}

int
cmp_args(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	subtract();
	simplify();
	floatfunc();
	p1 = pop();
	if (iszero(p1))
		return 0;
	if (!isnum(p1))
		stopf("compare err");
	if (isnegativenumber(p1))
		return -1;
	else
		return 1;
}
void
eval_transpose(struct atom *p1)
{
	int m, n;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	push(p2);

	if (!istensor(p2) || p2->u.tensor->ndim < 2)
		return;

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		n = pop_integer();

		push(cadr(p1));
		evalf();
		m = pop_integer();

		transpose(n, m);

		p1 = cddr(p1);
	}
}

void
transpose(int n, int m)
{
	int i, j, k, ndim, nelem;
	int index[MAXDIM];
	struct atom *p1, *p2;

	p1 = pop();

	ndim = p1->u.tensor->ndim;
	nelem = p1->u.tensor->nelem;

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stopf("transpose: index error");

	n--; // make zero based
	m--;

	p2 = copy_tensor(p1);

	// interchange indices n and m

	p2->u.tensor->dim[n] = p1->u.tensor->dim[m];
	p2->u.tensor->dim[m] = p1->u.tensor->dim[n];

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		k = 0;

		for (j = 0; j < ndim; j++) {
			if (j == n)
				k = k * p1->u.tensor->dim[m] + index[m];
			else if (j == m)
				k = k * p1->u.tensor->dim[n] + index[n];
			else
				k = k * p1->u.tensor->dim[j] + index[j];
		}

		p2->u.tensor->elem[k] = p1->u.tensor->elem[i];

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (++index[j] < p1->u.tensor->dim[j])
				break;
			index[j] = 0;
		}
	}

	push(p2);
}
void
eval_unit(struct atom *p1)
{
	int i, j, n;

	push(cadr(p1));
	evalf();

	n = pop_integer();

	if (n < 1)
		stopf("unit: index err");

	if (n == 1) {
		push_integer(1);
		return;
	}

	p1 = alloc_matrix(n, n);

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i == j)
				p1->u.tensor->elem[n * i + j] = one;
			else
				p1->u.tensor->elem[n * i + j] = zero;

	push(p1);
}
void
eval_user_function(struct atom *p1)
{
	int h, i;
	struct atom *FUNC_NAME, *FUNC_ARGS, *FUNC_DEFN;

	FUNC_NAME = car(p1);
	FUNC_ARGS = cdr(p1);

	FUNC_DEFN = get_usrfunc(FUNC_NAME);

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		if (FUNC_NAME == symbol(D_LOWER)) {
			expanding++;
			eval_derivative(p1);
			expanding--;
			return;
		}
		h = tos;
		push(FUNC_NAME);
		while (iscons(FUNC_ARGS)) {
			push(car(FUNC_ARGS));
			evalg(); // p1 is on frame stack, not reclaimed
			FUNC_ARGS = cdr(FUNC_ARGS);
		}
		list(tos - h);
		return;
	}

	// push FUNC_DEFN before arg eval to preclude any side effect reclaim

	push(FUNC_DEFN);

	// eval all args before changing bindings

	for (i = 0; i < 9; i++) {
		push(car(FUNC_ARGS));
		evalg(); // p1 is on frame stack, not reclaimed
		FUNC_ARGS = cdr(FUNC_ARGS);
	}

	save_symbol(symbol(ARG1));
	save_symbol(symbol(ARG2));
	save_symbol(symbol(ARG3));
	save_symbol(symbol(ARG4));
	save_symbol(symbol(ARG5));
	save_symbol(symbol(ARG6));
	save_symbol(symbol(ARG7));
	save_symbol(symbol(ARG8));
	save_symbol(symbol(ARG9));

	set_symbol(symbol(ARG9), pop(), symbol(NIL));
	set_symbol(symbol(ARG8), pop(), symbol(NIL));
	set_symbol(symbol(ARG7), pop(), symbol(NIL));
	set_symbol(symbol(ARG6), pop(), symbol(NIL));
	set_symbol(symbol(ARG5), pop(), symbol(NIL));
	set_symbol(symbol(ARG4), pop(), symbol(NIL));
	set_symbol(symbol(ARG3), pop(), symbol(NIL));
	set_symbol(symbol(ARG2), pop(), symbol(NIL));
	set_symbol(symbol(ARG1), pop(), symbol(NIL));

	evalg(); // eval FUNC_DEFN

	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
}
void
eval_user_symbol(struct atom *p1)
{
	struct atom *p2;
	p2 = get_binding(p1);
	if (p1 == p2)
		push(p1); // symbol evaluates to itself
	else {
		push(p2); // evaluate symbol binding
		evalg();
	}
}
void
eval_zero(struct atom *p1)
{
	int h, i, m, n;

	p1 = cdr(p1);
	h = tos;
	m = 1;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		dupl();
		n = pop_integer();
		if (n < 2)
			stopf("zero: dim err");
		m *= n;
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0) {
		push_integer(0); // scalar zero
		return;
	}

	if (n > MAXDIM)
		stopf("zero: rank err");

	p1 = alloc_tensor(m);

	for (i = 0; i < m; i++)
		p1->u.tensor->elem[i] = zero;

	// dim info

	p1->u.tensor->ndim = n;

	for (i = 0; i < n; i++)
		p1->u.tensor->dim[n - i - 1] = pop_integer();

	push(p1);
}
// factors N or N^M where N and M are rational numbers, returns factors on stack

void
factor_factor(void)
{
	uint32_t *numer, *denom;
	struct atom *INPUT, *BASE, *EXPO;

	INPUT = pop();

	if (car(INPUT) == symbol(POWER)) {

		BASE = cadr(INPUT);
		EXPO = caddr(INPUT);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(INPUT); // cannot factor
			return;
		}

		if (isminusone(BASE)) {
			push(INPUT); // -1 to the M
			return;
		}

		if (isnegativenumber(BASE)) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
		}

		numer = BASE->u.q.a;
		denom = BASE->u.q.b;

		if (!MEQUAL(numer, 1))
			factor_bignum(numer, EXPO);

		if (!MEQUAL(denom, 1)) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_bignum(denom, EXPO);
		}

		return;
	}

	if (!isrational(INPUT) || iszero(INPUT) || isplusone(INPUT) || isminusone(INPUT)) {
		push(INPUT);
		return;
	}

	if (isnegativenumber(INPUT))
		push_integer(-1);

	numer = INPUT->u.q.a;
	denom = INPUT->u.q.b;

	if (!MEQUAL(numer, 1))
		factor_bignum(numer, one);

	if (!MEQUAL(denom, 1))
		factor_bignum(denom, minusone);
}

// factor N, raise each factor to the power M

void
factor_bignum(uint32_t *N, struct atom *M)
{
	int h, i, n;
	struct atom *BASE, *EXPO;

	// greater than 31 bits?

	if (MLENGTH(N) > 1 || N[0] > 0x7fffffff) {
		push_bignum(MPLUS, mcopy(N), mint(1));
		if (isplusone(M))
			return;
		push_symbol(POWER);
		swap();
		push(M);
		list(3);
		return;
	}

	h = tos;

	n = N[0];

	factor_int(n);

	n = (tos - h) / 2; // number of factors on stack

	for (i = 0; i < n; i++) {

		BASE = stack[h + 2 * i + 0];
		EXPO = stack[h + 2 * i + 1];

		push(EXPO);
		push(M);
		multiply();
		EXPO = pop();

		if (isplusone(EXPO)) {
			stack[h + i] = BASE;
			continue;
		}

		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		stack[h + i] = pop();
	}

	tos = h + n; // pop all
}

#define NPRIME 4792

int primetab[NPRIME] = {
2,3,5,7,11,13,17,19,
23,29,31,37,41,43,47,53,
59,61,67,71,73,79,83,89,
97,101,103,107,109,113,127,131,
137,139,149,151,157,163,167,173,
179,181,191,193,197,199,211,223,
227,229,233,239,241,251,257,263,
269,271,277,281,283,293,307,311,
313,317,331,337,347,349,353,359,
367,373,379,383,389,397,401,409,
419,421,431,433,439,443,449,457,
461,463,467,479,487,491,499,503,
509,521,523,541,547,557,563,569,
571,577,587,593,599,601,607,613,
617,619,631,641,643,647,653,659,
661,673,677,683,691,701,709,719,
727,733,739,743,751,757,761,769,
773,787,797,809,811,821,823,827,
829,839,853,857,859,863,877,881,
883,887,907,911,919,929,937,941,
947,953,967,971,977,983,991,997,
1009,1013,1019,1021,1031,1033,1039,1049,
1051,1061,1063,1069,1087,1091,1093,1097,
1103,1109,1117,1123,1129,1151,1153,1163,
1171,1181,1187,1193,1201,1213,1217,1223,
1229,1231,1237,1249,1259,1277,1279,1283,
1289,1291,1297,1301,1303,1307,1319,1321,
1327,1361,1367,1373,1381,1399,1409,1423,
1427,1429,1433,1439,1447,1451,1453,1459,
1471,1481,1483,1487,1489,1493,1499,1511,
1523,1531,1543,1549,1553,1559,1567,1571,
1579,1583,1597,1601,1607,1609,1613,1619,
1621,1627,1637,1657,1663,1667,1669,1693,
1697,1699,1709,1721,1723,1733,1741,1747,
1753,1759,1777,1783,1787,1789,1801,1811,
1823,1831,1847,1861,1867,1871,1873,1877,
1879,1889,1901,1907,1913,1931,1933,1949,
1951,1973,1979,1987,1993,1997,1999,2003,
2011,2017,2027,2029,2039,2053,2063,2069,
2081,2083,2087,2089,2099,2111,2113,2129,
2131,2137,2141,2143,2153,2161,2179,2203,
2207,2213,2221,2237,2239,2243,2251,2267,
2269,2273,2281,2287,2293,2297,2309,2311,
2333,2339,2341,2347,2351,2357,2371,2377,
2381,2383,2389,2393,2399,2411,2417,2423,
2437,2441,2447,2459,2467,2473,2477,2503,
2521,2531,2539,2543,2549,2551,2557,2579,
2591,2593,2609,2617,2621,2633,2647,2657,
2659,2663,2671,2677,2683,2687,2689,2693,
2699,2707,2711,2713,2719,2729,2731,2741,
2749,2753,2767,2777,2789,2791,2797,2801,
2803,2819,2833,2837,2843,2851,2857,2861,
2879,2887,2897,2903,2909,2917,2927,2939,
2953,2957,2963,2969,2971,2999,3001,3011,
3019,3023,3037,3041,3049,3061,3067,3079,
3083,3089,3109,3119,3121,3137,3163,3167,
3169,3181,3187,3191,3203,3209,3217,3221,
3229,3251,3253,3257,3259,3271,3299,3301,
3307,3313,3319,3323,3329,3331,3343,3347,
3359,3361,3371,3373,3389,3391,3407,3413,
3433,3449,3457,3461,3463,3467,3469,3491,
3499,3511,3517,3527,3529,3533,3539,3541,
3547,3557,3559,3571,3581,3583,3593,3607,
3613,3617,3623,3631,3637,3643,3659,3671,
3673,3677,3691,3697,3701,3709,3719,3727,
3733,3739,3761,3767,3769,3779,3793,3797,
3803,3821,3823,3833,3847,3851,3853,3863,
3877,3881,3889,3907,3911,3917,3919,3923,
3929,3931,3943,3947,3967,3989,4001,4003,
4007,4013,4019,4021,4027,4049,4051,4057,
4073,4079,4091,4093,4099,4111,4127,4129,
4133,4139,4153,4157,4159,4177,4201,4211,
4217,4219,4229,4231,4241,4243,4253,4259,
4261,4271,4273,4283,4289,4297,4327,4337,
4339,4349,4357,4363,4373,4391,4397,4409,
4421,4423,4441,4447,4451,4457,4463,4481,
4483,4493,4507,4513,4517,4519,4523,4547,
4549,4561,4567,4583,4591,4597,4603,4621,
4637,4639,4643,4649,4651,4657,4663,4673,
4679,4691,4703,4721,4723,4729,4733,4751,
4759,4783,4787,4789,4793,4799,4801,4813,
4817,4831,4861,4871,4877,4889,4903,4909,
4919,4931,4933,4937,4943,4951,4957,4967,
4969,4973,4987,4993,4999,5003,5009,5011,
5021,5023,5039,5051,5059,5077,5081,5087,
5099,5101,5107,5113,5119,5147,5153,5167,
5171,5179,5189,5197,5209,5227,5231,5233,
5237,5261,5273,5279,5281,5297,5303,5309,
5323,5333,5347,5351,5381,5387,5393,5399,
5407,5413,5417,5419,5431,5437,5441,5443,
5449,5471,5477,5479,5483,5501,5503,5507,
5519,5521,5527,5531,5557,5563,5569,5573,
5581,5591,5623,5639,5641,5647,5651,5653,
5657,5659,5669,5683,5689,5693,5701,5711,
5717,5737,5741,5743,5749,5779,5783,5791,
5801,5807,5813,5821,5827,5839,5843,5849,
5851,5857,5861,5867,5869,5879,5881,5897,
5903,5923,5927,5939,5953,5981,5987,6007,
6011,6029,6037,6043,6047,6053,6067,6073,
6079,6089,6091,6101,6113,6121,6131,6133,
6143,6151,6163,6173,6197,6199,6203,6211,
6217,6221,6229,6247,6257,6263,6269,6271,
6277,6287,6299,6301,6311,6317,6323,6329,
6337,6343,6353,6359,6361,6367,6373,6379,
6389,6397,6421,6427,6449,6451,6469,6473,
6481,6491,6521,6529,6547,6551,6553,6563,
6569,6571,6577,6581,6599,6607,6619,6637,
6653,6659,6661,6673,6679,6689,6691,6701,
6703,6709,6719,6733,6737,6761,6763,6779,
6781,6791,6793,6803,6823,6827,6829,6833,
6841,6857,6863,6869,6871,6883,6899,6907,
6911,6917,6947,6949,6959,6961,6967,6971,
6977,6983,6991,6997,7001,7013,7019,7027,
7039,7043,7057,7069,7079,7103,7109,7121,
7127,7129,7151,7159,7177,7187,7193,7207,
7211,7213,7219,7229,7237,7243,7247,7253,
7283,7297,7307,7309,7321,7331,7333,7349,
7351,7369,7393,7411,7417,7433,7451,7457,
7459,7477,7481,7487,7489,7499,7507,7517,
7523,7529,7537,7541,7547,7549,7559,7561,
7573,7577,7583,7589,7591,7603,7607,7621,
7639,7643,7649,7669,7673,7681,7687,7691,
7699,7703,7717,7723,7727,7741,7753,7757,
7759,7789,7793,7817,7823,7829,7841,7853,
7867,7873,7877,7879,7883,7901,7907,7919,
7927,7933,7937,7949,7951,7963,7993,8009,
8011,8017,8039,8053,8059,8069,8081,8087,
8089,8093,8101,8111,8117,8123,8147,8161,
8167,8171,8179,8191,8209,8219,8221,8231,
8233,8237,8243,8263,8269,8273,8287,8291,
8293,8297,8311,8317,8329,8353,8363,8369,
8377,8387,8389,8419,8423,8429,8431,8443,
8447,8461,8467,8501,8513,8521,8527,8537,
8539,8543,8563,8573,8581,8597,8599,8609,
8623,8627,8629,8641,8647,8663,8669,8677,
8681,8689,8693,8699,8707,8713,8719,8731,
8737,8741,8747,8753,8761,8779,8783,8803,
8807,8819,8821,8831,8837,8839,8849,8861,
8863,8867,8887,8893,8923,8929,8933,8941,
8951,8963,8969,8971,8999,9001,9007,9011,
9013,9029,9041,9043,9049,9059,9067,9091,
9103,9109,9127,9133,9137,9151,9157,9161,
9173,9181,9187,9199,9203,9209,9221,9227,
9239,9241,9257,9277,9281,9283,9293,9311,
9319,9323,9337,9341,9343,9349,9371,9377,
9391,9397,9403,9413,9419,9421,9431,9433,
9437,9439,9461,9463,9467,9473,9479,9491,
9497,9511,9521,9533,9539,9547,9551,9587,
9601,9613,9619,9623,9629,9631,9643,9649,
9661,9677,9679,9689,9697,9719,9721,9733,
9739,9743,9749,9767,9769,9781,9787,9791,
9803,9811,9817,9829,9833,9839,9851,9857,
9859,9871,9883,9887,9901,9907,9923,9929,
9931,9941,9949,9967,9973,10007,10009,10037,
10039,10061,10067,10069,10079,10091,10093,10099,
10103,10111,10133,10139,10141,10151,10159,10163,
10169,10177,10181,10193,10211,10223,10243,10247,
10253,10259,10267,10271,10273,10289,10301,10303,
10313,10321,10331,10333,10337,10343,10357,10369,
10391,10399,10427,10429,10433,10453,10457,10459,
10463,10477,10487,10499,10501,10513,10529,10531,
10559,10567,10589,10597,10601,10607,10613,10627,
10631,10639,10651,10657,10663,10667,10687,10691,
10709,10711,10723,10729,10733,10739,10753,10771,
10781,10789,10799,10831,10837,10847,10853,10859,
10861,10867,10883,10889,10891,10903,10909,10937,
10939,10949,10957,10973,10979,10987,10993,11003,
11027,11047,11057,11059,11069,11071,11083,11087,
11093,11113,11117,11119,11131,11149,11159,11161,
11171,11173,11177,11197,11213,11239,11243,11251,
11257,11261,11273,11279,11287,11299,11311,11317,
11321,11329,11351,11353,11369,11383,11393,11399,
11411,11423,11437,11443,11447,11467,11471,11483,
11489,11491,11497,11503,11519,11527,11549,11551,
11579,11587,11593,11597,11617,11621,11633,11657,
11677,11681,11689,11699,11701,11717,11719,11731,
11743,11777,11779,11783,11789,11801,11807,11813,
11821,11827,11831,11833,11839,11863,11867,11887,
11897,11903,11909,11923,11927,11933,11939,11941,
11953,11959,11969,11971,11981,11987,12007,12011,
12037,12041,12043,12049,12071,12073,12097,12101,
12107,12109,12113,12119,12143,12149,12157,12161,
12163,12197,12203,12211,12227,12239,12241,12251,
12253,12263,12269,12277,12281,12289,12301,12323,
12329,12343,12347,12373,12377,12379,12391,12401,
12409,12413,12421,12433,12437,12451,12457,12473,
12479,12487,12491,12497,12503,12511,12517,12527,
12539,12541,12547,12553,12569,12577,12583,12589,
12601,12611,12613,12619,12637,12641,12647,12653,
12659,12671,12689,12697,12703,12713,12721,12739,
12743,12757,12763,12781,12791,12799,12809,12821,
12823,12829,12841,12853,12889,12893,12899,12907,
12911,12917,12919,12923,12941,12953,12959,12967,
12973,12979,12983,13001,13003,13007,13009,13033,
13037,13043,13049,13063,13093,13099,13103,13109,
13121,13127,13147,13151,13159,13163,13171,13177,
13183,13187,13217,13219,13229,13241,13249,13259,
13267,13291,13297,13309,13313,13327,13331,13337,
13339,13367,13381,13397,13399,13411,13417,13421,
13441,13451,13457,13463,13469,13477,13487,13499,
13513,13523,13537,13553,13567,13577,13591,13597,
13613,13619,13627,13633,13649,13669,13679,13681,
13687,13691,13693,13697,13709,13711,13721,13723,
13729,13751,13757,13759,13763,13781,13789,13799,
13807,13829,13831,13841,13859,13873,13877,13879,
13883,13901,13903,13907,13913,13921,13931,13933,
13963,13967,13997,13999,14009,14011,14029,14033,
14051,14057,14071,14081,14083,14087,14107,14143,
14149,14153,14159,14173,14177,14197,14207,14221,
14243,14249,14251,14281,14293,14303,14321,14323,
14327,14341,14347,14369,14387,14389,14401,14407,
14411,14419,14423,14431,14437,14447,14449,14461,
14479,14489,14503,14519,14533,14537,14543,14549,
14551,14557,14561,14563,14591,14593,14621,14627,
14629,14633,14639,14653,14657,14669,14683,14699,
14713,14717,14723,14731,14737,14741,14747,14753,
14759,14767,14771,14779,14783,14797,14813,14821,
14827,14831,14843,14851,14867,14869,14879,14887,
14891,14897,14923,14929,14939,14947,14951,14957,
14969,14983,15013,15017,15031,15053,15061,15073,
15077,15083,15091,15101,15107,15121,15131,15137,
15139,15149,15161,15173,15187,15193,15199,15217,
15227,15233,15241,15259,15263,15269,15271,15277,
15287,15289,15299,15307,15313,15319,15329,15331,
15349,15359,15361,15373,15377,15383,15391,15401,
15413,15427,15439,15443,15451,15461,15467,15473,
15493,15497,15511,15527,15541,15551,15559,15569,
15581,15583,15601,15607,15619,15629,15641,15643,
15647,15649,15661,15667,15671,15679,15683,15727,
15731,15733,15737,15739,15749,15761,15767,15773,
15787,15791,15797,15803,15809,15817,15823,15859,
15877,15881,15887,15889,15901,15907,15913,15919,
15923,15937,15959,15971,15973,15991,16001,16007,
16033,16057,16061,16063,16067,16069,16073,16087,
16091,16097,16103,16111,16127,16139,16141,16183,
16187,16189,16193,16217,16223,16229,16231,16249,
16253,16267,16273,16301,16319,16333,16339,16349,
16361,16363,16369,16381,16411,16417,16421,16427,
16433,16447,16451,16453,16477,16481,16487,16493,
16519,16529,16547,16553,16561,16567,16573,16603,
16607,16619,16631,16633,16649,16651,16657,16661,
16673,16691,16693,16699,16703,16729,16741,16747,
16759,16763,16787,16811,16823,16829,16831,16843,
16871,16879,16883,16889,16901,16903,16921,16927,
16931,16937,16943,16963,16979,16981,16987,16993,
17011,17021,17027,17029,17033,17041,17047,17053,
17077,17093,17099,17107,17117,17123,17137,17159,
17167,17183,17189,17191,17203,17207,17209,17231,
17239,17257,17291,17293,17299,17317,17321,17327,
17333,17341,17351,17359,17377,17383,17387,17389,
17393,17401,17417,17419,17431,17443,17449,17467,
17471,17477,17483,17489,17491,17497,17509,17519,
17539,17551,17569,17573,17579,17581,17597,17599,
17609,17623,17627,17657,17659,17669,17681,17683,
17707,17713,17729,17737,17747,17749,17761,17783,
17789,17791,17807,17827,17837,17839,17851,17863,
17881,17891,17903,17909,17911,17921,17923,17929,
17939,17957,17959,17971,17977,17981,17987,17989,
18013,18041,18043,18047,18049,18059,18061,18077,
18089,18097,18119,18121,18127,18131,18133,18143,
18149,18169,18181,18191,18199,18211,18217,18223,
18229,18233,18251,18253,18257,18269,18287,18289,
18301,18307,18311,18313,18329,18341,18353,18367,
18371,18379,18397,18401,18413,18427,18433,18439,
18443,18451,18457,18461,18481,18493,18503,18517,
18521,18523,18539,18541,18553,18583,18587,18593,
18617,18637,18661,18671,18679,18691,18701,18713,
18719,18731,18743,18749,18757,18773,18787,18793,
18797,18803,18839,18859,18869,18899,18911,18913,
18917,18919,18947,18959,18973,18979,19001,19009,
19013,19031,19037,19051,19069,19073,19079,19081,
19087,19121,19139,19141,19157,19163,19181,19183,
19207,19211,19213,19219,19231,19237,19249,19259,
19267,19273,19289,19301,19309,19319,19333,19373,
19379,19381,19387,19391,19403,19417,19421,19423,
19427,19429,19433,19441,19447,19457,19463,19469,
19471,19477,19483,19489,19501,19507,19531,19541,
19543,19553,19559,19571,19577,19583,19597,19603,
19609,19661,19681,19687,19697,19699,19709,19717,
19727,19739,19751,19753,19759,19763,19777,19793,
19801,19813,19819,19841,19843,19853,19861,19867,
19889,19891,19913,19919,19927,19937,19949,19961,
19963,19973,19979,19991,19993,19997,20011,20021,
20023,20029,20047,20051,20063,20071,20089,20101,
20107,20113,20117,20123,20129,20143,20147,20149,
20161,20173,20177,20183,20201,20219,20231,20233,
20249,20261,20269,20287,20297,20323,20327,20333,
20341,20347,20353,20357,20359,20369,20389,20393,
20399,20407,20411,20431,20441,20443,20477,20479,
20483,20507,20509,20521,20533,20543,20549,20551,
20563,20593,20599,20611,20627,20639,20641,20663,
20681,20693,20707,20717,20719,20731,20743,20747,
20749,20753,20759,20771,20773,20789,20807,20809,
20849,20857,20873,20879,20887,20897,20899,20903,
20921,20929,20939,20947,20959,20963,20981,20983,
21001,21011,21013,21017,21019,21023,21031,21059,
21061,21067,21089,21101,21107,21121,21139,21143,
21149,21157,21163,21169,21179,21187,21191,21193,
21211,21221,21227,21247,21269,21277,21283,21313,
21317,21319,21323,21341,21347,21377,21379,21383,
21391,21397,21401,21407,21419,21433,21467,21481,
21487,21491,21493,21499,21503,21517,21521,21523,
21529,21557,21559,21563,21569,21577,21587,21589,
21599,21601,21611,21613,21617,21647,21649,21661,
21673,21683,21701,21713,21727,21737,21739,21751,
21757,21767,21773,21787,21799,21803,21817,21821,
21839,21841,21851,21859,21863,21871,21881,21893,
21911,21929,21937,21943,21961,21977,21991,21997,
22003,22013,22027,22031,22037,22039,22051,22063,
22067,22073,22079,22091,22093,22109,22111,22123,
22129,22133,22147,22153,22157,22159,22171,22189,
22193,22229,22247,22259,22271,22273,22277,22279,
22283,22291,22303,22307,22343,22349,22367,22369,
22381,22391,22397,22409,22433,22441,22447,22453,
22469,22481,22483,22501,22511,22531,22541,22543,
22549,22567,22571,22573,22613,22619,22621,22637,
22639,22643,22651,22669,22679,22691,22697,22699,
22709,22717,22721,22727,22739,22741,22751,22769,
22777,22783,22787,22807,22811,22817,22853,22859,
22861,22871,22877,22901,22907,22921,22937,22943,
22961,22963,22973,22993,23003,23011,23017,23021,
23027,23029,23039,23041,23053,23057,23059,23063,
23071,23081,23087,23099,23117,23131,23143,23159,
23167,23173,23189,23197,23201,23203,23209,23227,
23251,23269,23279,23291,23293,23297,23311,23321,
23327,23333,23339,23357,23369,23371,23399,23417,
23431,23447,23459,23473,23497,23509,23531,23537,
23539,23549,23557,23561,23563,23567,23581,23593,
23599,23603,23609,23623,23627,23629,23633,23663,
23669,23671,23677,23687,23689,23719,23741,23743,
23747,23753,23761,23767,23773,23789,23801,23813,
23819,23827,23831,23833,23857,23869,23873,23879,
23887,23893,23899,23909,23911,23917,23929,23957,
23971,23977,23981,23993,24001,24007,24019,24023,
24029,24043,24049,24061,24071,24077,24083,24091,
24097,24103,24107,24109,24113,24121,24133,24137,
24151,24169,24179,24181,24197,24203,24223,24229,
24239,24247,24251,24281,24317,24329,24337,24359,
24371,24373,24379,24391,24407,24413,24419,24421,
24439,24443,24469,24473,24481,24499,24509,24517,
24527,24533,24547,24551,24571,24593,24611,24623,
24631,24659,24671,24677,24683,24691,24697,24709,
24733,24749,24763,24767,24781,24793,24799,24809,
24821,24841,24847,24851,24859,24877,24889,24907,
24917,24919,24923,24943,24953,24967,24971,24977,
24979,24989,25013,25031,25033,25037,25057,25073,
25087,25097,25111,25117,25121,25127,25147,25153,
25163,25169,25171,25183,25189,25219,25229,25237,
25243,25247,25253,25261,25301,25303,25307,25309,
25321,25339,25343,25349,25357,25367,25373,25391,
25409,25411,25423,25439,25447,25453,25457,25463,
25469,25471,25523,25537,25541,25561,25577,25579,
25583,25589,25601,25603,25609,25621,25633,25639,
25643,25657,25667,25673,25679,25693,25703,25717,
25733,25741,25747,25759,25763,25771,25793,25799,
25801,25819,25841,25847,25849,25867,25873,25889,
25903,25913,25919,25931,25933,25939,25943,25951,
25969,25981,25997,25999,26003,26017,26021,26029,
26041,26053,26083,26099,26107,26111,26113,26119,
26141,26153,26161,26171,26177,26183,26189,26203,
26209,26227,26237,26249,26251,26261,26263,26267,
26293,26297,26309,26317,26321,26339,26347,26357,
26371,26387,26393,26399,26407,26417,26423,26431,
26437,26449,26459,26479,26489,26497,26501,26513,
26539,26557,26561,26573,26591,26597,26627,26633,
26641,26647,26669,26681,26683,26687,26693,26699,
26701,26711,26713,26717,26723,26729,26731,26737,
26759,26777,26783,26801,26813,26821,26833,26839,
26849,26861,26863,26879,26881,26891,26893,26903,
26921,26927,26947,26951,26953,26959,26981,26987,
26993,27011,27017,27031,27043,27059,27061,27067,
27073,27077,27091,27103,27107,27109,27127,27143,
27179,27191,27197,27211,27239,27241,27253,27259,
27271,27277,27281,27283,27299,27329,27337,27361,
27367,27397,27407,27409,27427,27431,27437,27449,
27457,27479,27481,27487,27509,27527,27529,27539,
27541,27551,27581,27583,27611,27617,27631,27647,
27653,27673,27689,27691,27697,27701,27733,27737,
27739,27743,27749,27751,27763,27767,27773,27779,
27791,27793,27799,27803,27809,27817,27823,27827,
27847,27851,27883,27893,27901,27917,27919,27941,
27943,27947,27953,27961,27967,27983,27997,28001,
28019,28027,28031,28051,28057,28069,28081,28087,
28097,28099,28109,28111,28123,28151,28163,28181,
28183,28201,28211,28219,28229,28277,28279,28283,
28289,28297,28307,28309,28319,28349,28351,28387,
28393,28403,28409,28411,28429,28433,28439,28447,
28463,28477,28493,28499,28513,28517,28537,28541,
28547,28549,28559,28571,28573,28579,28591,28597,
28603,28607,28619,28621,28627,28631,28643,28649,
28657,28661,28663,28669,28687,28697,28703,28711,
28723,28729,28751,28753,28759,28771,28789,28793,
28807,28813,28817,28837,28843,28859,28867,28871,
28879,28901,28909,28921,28927,28933,28949,28961,
28979,29009,29017,29021,29023,29027,29033,29059,
29063,29077,29101,29123,29129,29131,29137,29147,
29153,29167,29173,29179,29191,29201,29207,29209,
29221,29231,29243,29251,29269,29287,29297,29303,
29311,29327,29333,29339,29347,29363,29383,29387,
29389,29399,29401,29411,29423,29429,29437,29443,
29453,29473,29483,29501,29527,29531,29537,29567,
29569,29573,29581,29587,29599,29611,29629,29633,
29641,29663,29669,29671,29683,29717,29723,29741,
29753,29759,29761,29789,29803,29819,29833,29837,
29851,29863,29867,29873,29879,29881,29917,29921,
29927,29947,29959,29983,29989,30011,30013,30029,
30047,30059,30071,30089,30091,30097,30103,30109,
30113,30119,30133,30137,30139,30161,30169,30181,
30187,30197,30203,30211,30223,30241,30253,30259,
30269,30271,30293,30307,30313,30319,30323,30341,
30347,30367,30389,30391,30403,30427,30431,30449,
30467,30469,30491,30493,30497,30509,30517,30529,
30539,30553,30557,30559,30577,30593,30631,30637,
30643,30649,30661,30671,30677,30689,30697,30703,
30707,30713,30727,30757,30763,30773,30781,30803,
30809,30817,30829,30839,30841,30851,30853,30859,
30869,30871,30881,30893,30911,30931,30937,30941,
30949,30971,30977,30983,31013,31019,31033,31039,
31051,31063,31069,31079,31081,31091,31121,31123,
31139,31147,31151,31153,31159,31177,31181,31183,
31189,31193,31219,31223,31231,31237,31247,31249,
31253,31259,31267,31271,31277,31307,31319,31321,
31327,31333,31337,31357,31379,31387,31391,31393,
31397,31469,31477,31481,31489,31511,31513,31517,
31531,31541,31543,31547,31567,31573,31583,31601,
31607,31627,31643,31649,31657,31663,31667,31687,
31699,31721,31723,31727,31729,31741,31751,31769,
31771,31793,31799,31817,31847,31849,31859,31873,
31883,31891,31907,31957,31963,31973,31981,31991,
32003,32009,32027,32029,32051,32057,32059,32063,
32069,32077,32083,32089,32099,32117,32119,32141,
32143,32159,32173,32183,32189,32191,32203,32213,
32233,32237,32251,32257,32261,32297,32299,32303,
32309,32321,32323,32327,32341,32353,32359,32363,
32369,32371,32377,32381,32401,32411,32413,32423,
32429,32441,32443,32467,32479,32491,32497,32503,
32507,32531,32533,32537,32561,32563,32569,32573,
32579,32587,32603,32609,32611,32621,32633,32647,
32653,32687,32693,32707,32713,32717,32719,32749,
32771,32779,32783,32789,32797,32801,32803,32831,
32833,32839,32843,32869,32887,32909,32911,32917,
32933,32939,32941,32957,32969,32971,32983,32987,
32993,32999,33013,33023,33029,33037,33049,33053,
33071,33073,33083,33091,33107,33113,33119,33149,
33151,33161,33179,33181,33191,33199,33203,33211,
33223,33247,33287,33289,33301,33311,33317,33329,
33331,33343,33347,33349,33353,33359,33377,33391,
33403,33409,33413,33427,33457,33461,33469,33479,
33487,33493,33503,33521,33529,33533,33547,33563,
33569,33577,33581,33587,33589,33599,33601,33613,
33617,33619,33623,33629,33637,33641,33647,33679,
33703,33713,33721,33739,33749,33751,33757,33767,
33769,33773,33791,33797,33809,33811,33827,33829,
33851,33857,33863,33871,33889,33893,33911,33923,
33931,33937,33941,33961,33967,33997,34019,34031,
34033,34039,34057,34061,34123,34127,34129,34141,
34147,34157,34159,34171,34183,34211,34213,34217,
34231,34253,34259,34261,34267,34273,34283,34297,
34301,34303,34313,34319,34327,34337,34351,34361,
34367,34369,34381,34403,34421,34429,34439,34457,
34469,34471,34483,34487,34499,34501,34511,34513,
34519,34537,34543,34549,34583,34589,34591,34603,
34607,34613,34631,34649,34651,34667,34673,34679,
34687,34693,34703,34721,34729,34739,34747,34757,
34759,34763,34781,34807,34819,34841,34843,34847,
34849,34871,34877,34883,34897,34913,34919,34939,
34949,34961,34963,34981,35023,35027,35051,35053,
35059,35069,35081,35083,35089,35099,35107,35111,
35117,35129,35141,35149,35153,35159,35171,35201,
35221,35227,35251,35257,35267,35279,35281,35291,
35311,35317,35323,35327,35339,35353,35363,35381,
35393,35401,35407,35419,35423,35437,35447,35449,
35461,35491,35507,35509,35521,35527,35531,35533,
35537,35543,35569,35573,35591,35593,35597,35603,
35617,35671,35677,35729,35731,35747,35753,35759,
35771,35797,35801,35803,35809,35831,35837,35839,
35851,35863,35869,35879,35897,35899,35911,35923,
35933,35951,35963,35969,35977,35983,35993,35999,
36007,36011,36013,36017,36037,36061,36067,36073,
36083,36097,36107,36109,36131,36137,36151,36161,
36187,36191,36209,36217,36229,36241,36251,36263,
36269,36277,36293,36299,36307,36313,36319,36341,
36343,36353,36373,36383,36389,36433,36451,36457,
36467,36469,36473,36479,36493,36497,36523,36527,
36529,36541,36551,36559,36563,36571,36583,36587,
36599,36607,36629,36637,36643,36653,36671,36677,
36683,36691,36697,36709,36713,36721,36739,36749,
36761,36767,36779,36781,36787,36791,36793,36809,
36821,36833,36847,36857,36871,36877,36887,36899,
36901,36913,36919,36923,36929,36931,36943,36947,
36973,36979,36997,37003,37013,37019,37021,37039,
37049,37057,37061,37087,37097,37117,37123,37139,
37159,37171,37181,37189,37199,37201,37217,37223,
37243,37253,37273,37277,37307,37309,37313,37321,
37337,37339,37357,37361,37363,37369,37379,37397,
37409,37423,37441,37447,37463,37483,37489,37493,
37501,37507,37511,37517,37529,37537,37547,37549,
37561,37567,37571,37573,37579,37589,37591,37607,
37619,37633,37643,37649,37657,37663,37691,37693,
37699,37717,37747,37781,37783,37799,37811,37813,
37831,37847,37853,37861,37871,37879,37889,37897,
37907,37951,37957,37963,37967,37987,37991,37993,
37997,38011,38039,38047,38053,38069,38083,38113,
38119,38149,38153,38167,38177,38183,38189,38197,
38201,38219,38231,38237,38239,38261,38273,38281,
38287,38299,38303,38317,38321,38327,38329,38333,
38351,38371,38377,38393,38431,38447,38449,38453,
38459,38461,38501,38543,38557,38561,38567,38569,
38593,38603,38609,38611,38629,38639,38651,38653,
38669,38671,38677,38693,38699,38707,38711,38713,
38723,38729,38737,38747,38749,38767,38783,38791,
38803,38821,38833,38839,38851,38861,38867,38873,
38891,38903,38917,38921,38923,38933,38953,38959,
38971,38977,38993,39019,39023,39041,39043,39047,
39079,39089,39097,39103,39107,39113,39119,39133,
39139,39157,39161,39163,39181,39191,39199,39209,
39217,39227,39229,39233,39239,39241,39251,39293,
39301,39313,39317,39323,39341,39343,39359,39367,
39371,39373,39383,39397,39409,39419,39439,39443,
39451,39461,39499,39503,39509,39511,39521,39541,
39551,39563,39569,39581,39607,39619,39623,39631,
39659,39667,39671,39679,39703,39709,39719,39727,
39733,39749,39761,39769,39779,39791,39799,39821,
39827,39829,39839,39841,39847,39857,39863,39869,
39877,39883,39887,39901,39929,39937,39953,39971,
39979,39983,39989,40009,40013,40031,40037,40039,
40063,40087,40093,40099,40111,40123,40127,40129,
40151,40153,40163,40169,40177,40189,40193,40213,
40231,40237,40241,40253,40277,40283,40289,40343,
40351,40357,40361,40387,40423,40427,40429,40433,
40459,40471,40483,40487,40493,40499,40507,40519,
40529,40531,40543,40559,40577,40583,40591,40597,
40609,40627,40637,40639,40693,40697,40699,40709,
40739,40751,40759,40763,40771,40787,40801,40813,
40819,40823,40829,40841,40847,40849,40853,40867,
40879,40883,40897,40903,40927,40933,40939,40949,
40961,40973,40993,41011,41017,41023,41039,41047,
41051,41057,41077,41081,41113,41117,41131,41141,
41143,41149,41161,41177,41179,41183,41189,41201,
41203,41213,41221,41227,41231,41233,41243,41257,
41263,41269,41281,41299,41333,41341,41351,41357,
41381,41387,41389,41399,41411,41413,41443,41453,
41467,41479,41491,41507,41513,41519,41521,41539,
41543,41549,41579,41593,41597,41603,41609,41611,
41617,41621,41627,41641,41647,41651,41659,41669,
41681,41687,41719,41729,41737,41759,41761,41771,
41777,41801,41809,41813,41843,41849,41851,41863,
41879,41887,41893,41897,41903,41911,41927,41941,
41947,41953,41957,41959,41969,41981,41983,41999,
42013,42017,42019,42023,42043,42061,42071,42073,
42083,42089,42101,42131,42139,42157,42169,42179,
42181,42187,42193,42197,42209,42221,42223,42227,
42239,42257,42281,42283,42293,42299,42307,42323,
42331,42337,42349,42359,42373,42379,42391,42397,
42403,42407,42409,42433,42437,42443,42451,42457,
42461,42463,42467,42473,42487,42491,42499,42509,
42533,42557,42569,42571,42577,42589,42611,42641,
42643,42649,42667,42677,42683,42689,42697,42701,
42703,42709,42719,42727,42737,42743,42751,42767,
42773,42787,42793,42797,42821,42829,42839,42841,
42853,42859,42863,42899,42901,42923,42929,42937,
42943,42953,42961,42967,42979,42989,43003,43013,
43019,43037,43049,43051,43063,43067,43093,43103,
43117,43133,43151,43159,43177,43189,43201,43207,
43223,43237,43261,43271,43283,43291,43313,43319,
43321,43331,43391,43397,43399,43403,43411,43427,
43441,43451,43457,43481,43487,43499,43517,43541,
43543,43573,43577,43579,43591,43597,43607,43609,
43613,43627,43633,43649,43651,43661,43669,43691,
43711,43717,43721,43753,43759,43777,43781,43783,
43787,43789,43793,43801,43853,43867,43889,43891,
43913,43933,43943,43951,43961,43963,43969,43973,
43987,43991,43997,44017,44021,44027,44029,44041,
44053,44059,44071,44087,44089,44101,44111,44119,
44123,44129,44131,44159,44171,44179,44189,44201,
44203,44207,44221,44249,44257,44263,44267,44269,
44273,44279,44281,44293,44351,44357,44371,44381,
44383,44389,44417,44449,44453,44483,44491,44497,
44501,44507,44519,44531,44533,44537,44543,44549,
44563,44579,44587,44617,44621,44623,44633,44641,
44647,44651,44657,44683,44687,44699,44701,44711,
44729,44741,44753,44771,44773,44777,44789,44797,
44809,44819,44839,44843,44851,44867,44879,44887,
44893,44909,44917,44927,44939,44953,44959,44963,
44971,44983,44987,45007,45013,45053,45061,45077,
45083,45119,45121,45127,45131,45137,45139,45161,
45179,45181,45191,45197,45233,45247,45259,45263,
45281,45289,45293,45307,45317,45319,45329,45337,
45341,45343,45361,45377,45389,45403,45413,45427,
45433,45439,45481,45491,45497,45503,45523,45533,
45541,45553,45557,45569,45587,45589,45599,45613,
45631,45641,45659,45667,45673,45677,45691,45697,
45707,45737,45751,45757,45763,45767,45779,45817,
45821,45823,45827,45833,45841,45853,45863,45869,
45887,45893,45943,45949,45953,45959,45971,45979,
45989,46021,46027,46049,46051,46061,46073,46091,
46093,46099,46103,46133,46141,46147,46153,46171,
46181,46183,46187,46199,46219,46229,46237,46261,
46271,46273,46279,46301,46307,46309,46327,46337,
};

// the next prime after 46337 is 46349

// 46349 ^ 2 = 2,148,229,801 which is greater than 2^31 - 1 = 2,147,483,648 = 0x7fffffff

// hence this table can factor all positive ints

void
factor_int(int n)
{
	int d, k, m;

	n = abs(n);

	if (n < 2)
		return;

	for (k = 0; k < NPRIME; k++) {

		d = primetab[k];

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m == 0)
			continue;

		push_integer(d);
		push_integer(m);

		if (n == 1)
			return;
	}

	push_integer(n);
	push_integer(1);
}
// automatic variables not visible to the garbage collector are reclaimed

void
gc(void)
{
	int i, j;
	struct atom *p;

	gc_count++;
	alloc_count = 0;

	// tag everything

	for (i = 0; i < block_count; i++)
		for (j = 0; j < BLOCKSIZE; j++)
			mem[i][j].tag = 1;

	// untag what's used

	untag(zero);
	untag(one);
	untag(minusone);
	untag(imaginaryunit);

	for (i = 0; i < tos; i++)
		untag(stack[i]);

	for (i = 0; i < tof; i++)
		untag(frame[i]);

	for (i = 0; i < 27 * BUCKETSIZE; i++) {
		untag(symtab[i]);
		untag(binding[i]);
		untag(usrfunc[i]);
	}

	// collect everything that's still tagged

	free_list = NULL;
	free_count = 0;

	for (i = 0; i < block_count; i++)
		for (j = 0; j < BLOCKSIZE; j++) {

			p = mem[i] + j;

			if (p->tag == 0)
				continue;

			// still tagged so it's unused, put on free list

			switch (p->atomtype) {
			case KSYM:
				free(p->u.ksym.name);
				ksym_count--;
				break;
			case USYM:
				free(p->u.usym.name);
				usym_count--;
				break;
			case RATIONAL:
				mfree(p->u.q.a);
				mfree(p->u.q.b);
				break;
			case STR:
				if (p->u.str)
					free(p->u.str);
				string_count--;
				break;
			case TENSOR:
				free(p->u.tensor);
				tensor_count--;
				break;
			default:
				break; // FREEATOM, CONS, or DOUBLE
			}

			p->atomtype = FREEATOM;
			p->u.next = free_list;

			free_list = p;
			free_count++;
		}
}

void
untag(struct atom *p)
{
	int i;

	if (p == NULL)
		return;

	while (iscons(p)) {
		if (p->tag == 0)
			return;
		p->tag = 0;
		untag(p->u.cons.car);
		p = p->u.cons.cdr;
	}

	if (p->tag == 0)
		return;

	p->tag = 0;

	if (istensor(p))
		for (i = 0; i < p->u.tensor->nelem; i++)
			untag(p->u.tensor->elem[i]);
}
struct atom *mem[MAXBLOCKS]; // an array of pointers
struct atom *free_list;

int tos; // top of stack
int tof; // top of frame

struct atom *stack[STACKSIZE];
struct atom *frame[FRAMESIZE];

struct atom *symtab[27 * BUCKETSIZE];
struct atom *binding[27 * BUCKETSIZE];
struct atom *usrfunc[27 * BUCKETSIZE];

struct atom *zero;
struct atom *one;
struct atom *minusone;
struct atom *imaginaryunit;

int eval_level;
int gc_level;
int expanding;
int drawing;
int nonstop;
int interrupt;
jmp_buf jmpbuf0;
jmp_buf jmpbuf1;
char *trace1;
char *trace2;

int alloc_count;
int block_count;
int free_count;
int gc_count;
int bignum_count;
int ksym_count;
int usym_count;
int string_count;
int tensor_count;
int max_eval_level;
int max_tos;
int max_tof;

char strbuf[STRBUFLEN];

char *outbuf;
int outbuf_index;
int outbuf_length;
void
outbuf_init(void)
{
	outbuf_index = 0;
	outbuf_puts(""); // init outbuf as empty string
}

void
outbuf_puts(char *s)
{
	int len, m;

	len = (int) strlen(s);

	// Let outbuf_index + len == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((outbuf_index + len) / 1000 + 1); // m is a multiple of 1000

	if (m > outbuf_length) {
		outbuf = realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}

	strcpy(outbuf + outbuf_index, s);
	outbuf_index += len;
}

void
outbuf_putc(int c)
{
	int m;

	// Let outbuf_index + 1 == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((outbuf_index + 1) / 1000 + 1); // m is a multiple of 1000

	if (m > outbuf_length) {
		outbuf = realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}

	outbuf[outbuf_index++] = c;
	outbuf[outbuf_index] = '\0';
}
int
iszero(struct atom *p)
{
	int i;
	if (isrational(p))
		return MZERO(p->u.q.a);
	if (isdouble(p))
		return p->u.d == 0.0;
	if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (!iszero(p->u.tensor->elem[i]))
				return 0;
		return 1;
	}
	return 0;
}

int
isequaln(struct atom *p, int n)
{
	if (isrational(p))
		return p->sign == (n < 0 ? MMINUS : MPLUS) && MEQUAL(p->u.q.a, abs(n)) && MEQUAL(p->u.q.b, 1);
	if (isdouble(p))
		return p->u.d == (double) n;
	return 0;
}

int
isequalq(struct atom *p, int a, int b)
{
	if (isrational(p))
		return p->sign == (a < 0 ? MMINUS : MPLUS) && MEQUAL(p->u.q.a, abs(a)) && MEQUAL(p->u.q.b, b);
	if (isdouble(p))
		return p->u.d == (double) a / b;
	return 0;
}

int
isplusone(struct atom *p)
{
	return isequaln(p, 1);
}

int
isminusone(struct atom *p)
{
	return isequaln(p, -1);
}

int
isinteger(struct atom *p)
{
	return isrational(p) && MEQUAL(p->u.q.b, 1);
}

int
isinteger1(struct atom *p)
{
	return isinteger(p) && isplusone(p);
}

int
isfraction(struct atom *p)
{
	return isrational(p) && !MEQUAL(p->u.q.b, 1);
}

int
isposint(struct atom *p)
{
	return isinteger(p) && !isnegativenumber(p);
}

int
iseveninteger(struct atom *p)
{
	return isinteger(p) && (p->u.q.a[0] & 1) == 0;
}

int
isradical(struct atom *p)
{
	return car(p) == symbol(POWER) && isposint(cadr(p)) && isfraction(caddr(p));
}

int
isnegativeterm(struct atom *p)
{
	return isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)));
}

int
isnegativenumber(struct atom *p)
{
	if (isrational(p))
		return p->sign == MMINUS;
	else if (isdouble(p))
		return p->u.d < 0.0;
	else
		return 0;
}

int
iscomplexnumber(struct atom *p)
{
	return isimaginarynumber(p) || (lengthf(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}

int
isimaginarynumber(struct atom *p)
{
	return isimaginaryunit(p) || (lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}

int
isimaginaryunit(struct atom *p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p)) && isequalq(caddr(p), 1, 2);
}

// p == 1/sqrt(2) ?

int
isoneoversqrttwo(struct atom *p)
{
	return car(p) == symbol(POWER) && isequaln(cadr(p), 2) && isequalq(caddr(p), -1, 2);
}

// p == -1/sqrt(2) ?

int
isminusoneoversqrttwo(struct atom *p)
{
	return lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isminusone(cadr(p)) && isoneoversqrttwo(caddr(p));
}

// x + y * (-1)^(1/2) where x and y are double?

int
isdoublez(struct atom *p)
{
	if (car(p) == symbol(ADD)) {

		if (lengthf(p) != 3)
			return 0;

		if (!isdouble(cadr(p))) // x
			return 0;

		p = caddr(p);
	}

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (lengthf(p) != 3)
		return 0;

	if (!isdouble(cadr(p))) // y
		return 0;

	p = caddr(p);

	if (car(p) != symbol(POWER))
		return 0;

	if (!isminusone(cadr(p)))
		return 0;

	if (!isequalq(caddr(p), 1, 2))
		return 0;

	return 1;
}

int
isdenominator(struct atom *p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;
	else if (isrational(p) && !MEQUAL(p->u.q.b, 1))
		return 1;
	else
		return 0;
}

int
isnumerator(struct atom *p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;
	else if (isrational(p) && MEQUAL(p->u.q.a, 1))
		return 0;
	else
		return 1;
}

int
isdoublesomewhere(struct atom *p)
{
	if (isdouble(p))
		return 1;

	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdoublesomewhere(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}

int
isdenormalpolar(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdenormalpolarterm(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return isdenormalpolarterm(p);
}

// returns 1 if term is (coeff * i * pi) and coeff < 0 or coeff >= 1/2

int
isdenormalpolarterm(struct atom *p)
{
	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (lengthf(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI))
		return 1;

	if (lengthf(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // p = coeff of term

	if (isnegativenumber(p))
		return 1; // p < 0

	push(p);
	push_rational(-1, 2);
	add();
	p = pop();

	if (!isnegativenumber(p))
		return 1; // p >= 1/2

	return 0;
}

int
issquarematrix(struct atom *p)
{
	return istensor(p) && p->u.tensor->ndim == 2 && p->u.tensor->dim[0] == p->u.tensor->dim[1];
}

int
issmallinteger(struct atom *p)
{
	if (isinteger(p))
		return MLENGTH(p->u.q.a) == 1 && p->u.q.a[0] <= 0x7fffffff;

	if (isdouble(p))
		return p->u.d == floor(p->u.d) && fabs(p->u.d) <= 0x7fffffff;

	return 0;
}
void
run(char *buf)
{
	if (setjmp(jmpbuf0))
		return;

	tos = 0;
	tof = 0;
	interrupt = 0;
	eval_level = 0;
	gc_level = 0;
	expanding = 1;
	drawing = 0;
	nonstop = 0;

	if (zero == NULL) {
		init_symbol_table();
		push_bignum(MPLUS, mint(0), mint(1));
		zero = pop();
		push_bignum(MPLUS, mint(1), mint(1));
		one = pop();
		push_bignum(MMINUS, mint(1), mint(1));
		minusone = pop();
		push_symbol(POWER);
		push_integer(-1);
		push_rational(1, 2);
		list(3);
		imaginaryunit = pop();
		run_init_script();
	}

	run_buf(buf);
}

void
run_buf(char *buf)
{
	char *s, *save_trace1, *save_trace2;
	struct atom *p1;

	save_trace1 = trace1;
	save_trace2 = trace2;

	s = buf;

	for (;;) {

		s = scan_input(s); // also updates trace1 and trace2

		if (s == NULL)
			break; // end of input

		dupl();
		evalg();

		// update last

		dupl();
		p1 = pop();

		if (p1 != symbol(NIL))
			set_symbol(symbol(LAST), p1, symbol(NIL));

		print_result();
	}

	trace1 = save_trace1;
	trace2 = save_trace2;
}

char *
scan_input(char *s)
{
	struct atom *p1;
	trace1 = s;
	s = scan(s);
	trace2 = s;
	p1 = get_binding(symbol(TRACE));
	if (p1 != symbol(TRACE) && !iszero(p1))
		print_trace(BLUE);
	return s;
}

void
print_trace(int color)
{
	char c, *s;
	if (trace1 == NULL || trace2 == NULL)
		return;
	outbuf_init();
	c = '\n';
	s = trace1;
	while (*s && s < trace2) {
		c = *s++;
		outbuf_putc(c);
	}
	if (c != '\n')
		outbuf_putc('\n');
	printbuf(outbuf, color);
}

char *init_script =
"i = sqrt(-1)\n"
"grad(f) = d(f,(x,y,z))\n"
"cross(a,b) = (dot(a[2],b[3])-dot(a[3],b[2]),dot(a[3],b[1])-dot(a[1],b[3]),dot(a[1],b[2])-dot(a[2],b[1]))\n"
"curl(u) = (d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))\n"
"div(u) = d(u[1],x)+d(u[2],y)+d(u[3],z)\n"
"laguerre(x,n,m) = (n + m)! sum(k,0,n,(-x)^k / ((n - k)! (m + k)! k!))\n"
"legendre(f,n,m,x) = eval(1 / (2^n n!) (1 - x^2)^(m/2) d((x^2 - 1)^n,x,n + m),x,f)\n"
"hermite(x,n) = (-1)^n exp(x^2) d(exp(-x^2),x,n)\n"
"binomial(n,k) = n! / k! / (n - k)!\n"
"choose(n,k) = n! / k! / (n - k)!\n"
;

void
run_init_script(void)
{
	run_buf(init_script);
}

void
stopf(char *s)
{
	if (nonstop)
		longjmp(jmpbuf1, 1);
	print_trace(RED);
	snprintf(strbuf, STRBUFLEN, "Stop: %s\n", s);
	printbuf(strbuf, RED);
	longjmp(jmpbuf0, 1);
}

void
exitf(char *s)
{
	nonstop = 0;
	stopf(s);
}
// token_str and scan_str are pointers to the input string, for example
//
//	| g | a | m | m | a |   | a | l | p | h | a |
//	  ^                   ^
//	  token_str           scan_str

#define T_INTEGER 1001
#define T_DOUBLE 1002
#define T_SYMBOL 1003
#define T_FUNCTION 1004
#define T_NEWLINE 1006
#define T_STRING 1007
#define T_GTEQ 1008
#define T_LTEQ 1009
#define T_EQ 1010
#define T_END 1011

int token;
int scan_mode;
int scan_level;

char *scan_str;
char *token_str;
char *token_buf;
int token_buf_len;

char *
scan(char *s)
{
	scan_mode = 0;
	return scan_nib(s);
}

char *
scan1(char *s)
{
	scan_mode = 1; // mode for table of integrals
	return scan_nib(s);
}

char *
scan_nib(char *s)
{
	scan_str = s;
	scan_level = 0;
	get_token_skip_newlines();
	if (token == T_END)
		return NULL;
	scan_stmt();
	if (token != T_NEWLINE && token != T_END)
		scan_error("syntax err"); // unexpected token, for example, 1:2
	return scan_str;
}

void
scan_stmt(void)
{
	scan_comparison();
	if (token == '=') {
		get_token_skip_newlines(); // get token after '='
		push_symbol(SETQ);
		swap();
		scan_comparison();
		list(3);
	}
}

void
scan_comparison(void)
{
	scan_expression();
	switch (token) {
	case T_EQ:
		push_symbol(TESTEQ); // ==
		break;
	case T_LTEQ:
		push_symbol(TESTLE);
		break;
	case T_GTEQ:
		push_symbol(TESTGE);
		break;
	case '<':
		push_symbol(TESTLT);
		break;
	case '>':
		push_symbol(TESTGT);
		break;
	default:
		return;
	}
	swap();
	get_token_skip_newlines(); // get token after rel op
	scan_expression();
	list(3);
}

void
scan_expression(void)
{
	int h = tos, t;
	t = token;
	if (token == '+' || token == '-')
		get_token_skip_newlines();
	scan_term();
	if (t == '-')
		static_negate();
	while (token == '+' || token == '-') {
		t = token;
		get_token_skip_newlines(); // get token after '+' or '-'
		scan_term();
		if (t == '-')
			static_negate();
	}
	if (tos - h > 1) {
		list(tos - h);
		push_symbol(ADD);
		swap();
		cons(); // prepend ADD to list
	}
}

int
another_factor_pending(void)
{
	switch (token) {
	case '*':
	case '/':
	case '(':
	case T_SYMBOL:
	case T_FUNCTION:
	case T_INTEGER:
	case T_DOUBLE:
	case T_STRING:
		return 1;
	default:
		break;
	}
	return 0;
}

void
scan_term(void)
{
	int h = tos, t;

	scan_power();

	while (another_factor_pending()) {

		t = token;

		if (token == '*' || token == '/')
			get_token_skip_newlines();

		scan_power();

		if (t == '/')
			static_reciprocate();
	}

	if (tos - h > 1) {
		list(tos - h);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
	}
}

void
scan_power(void)
{
	scan_factor();
	if (token == '^') {
		get_token_skip_newlines();
		push_symbol(POWER);
		swap();
		scan_power();
		list(3);
	}
}

void
scan_factor(void)
{
	int h = tos;

	switch (token) {

	case '(':
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		scan_integer();
		get_token();
		break;

	case T_DOUBLE:
		push_double(atof(token_buf));
		get_token();
		break;

	case T_STRING:
		scan_string();
		break;

	default:
		scan_error("syntax err");
		break;
	}

	// index

	if (token == '[') {
		scan_level++;
		get_token(); // get token after '['
		push_symbol(INDEX);
		swap();
		scan_expression();
		while (token == ',') {
			get_token(); // get token after ','
			scan_expression();
		}
		if (token != ']')
			scan_error("expected ']'");
		scan_level--;
		get_token(); // get token after ']'
		list(tos - h);
	}

	while (token == '!') {
		get_token(); // get token after '!'
		push_symbol(FACTORIAL);
		swap();
		list(2);
	}
}

void
scan_symbol(void)
{
	if (scan_mode && strlen(token_buf) == 1)
		switch (token_buf[0]) {
		case 'a':
			push_symbol(SA);
			break;
		case 'b':
			push_symbol(SB);
			break;
		case 'x':
			push_symbol(SX);
			break;
		default:
			push(lookup(token_buf));
			break;
		}
	else
		push(lookup(token_buf));
	get_token();
}

void
scan_string(void)
{
	push_string(token_buf);
	get_token();
}

void
scan_function_call(void)
{
	int h = tos;
	scan_level++;
	push(lookup(token_buf)); // push function name
	get_token(); // get token after function name
	get_token(); // get token after '('
	if (token == ')') {
		scan_level--;
		get_token(); // get token after ')'
		list(1); // function call with no args
		return;
	}
	scan_stmt();
	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != ')')
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	list(tos - h);
}

void
scan_integer(void)
{
	int sign;
	uint32_t *a;
	switch (*token_buf) {
	case '+':
		sign = MPLUS;
		a = mscan(token_buf + 1);
		break;
	case '-':
		sign = MMINUS;
		a = mscan(token_buf + 1);
		break;
	default:
		sign = MPLUS;
		a = mscan(token_buf);
		break;
	}
	push_bignum(sign, a, mint(1));
}

void
scan_subexpr(void)
{
	int h = tos;
	scan_level++;
	get_token(); // get token after '('
	scan_stmt();
	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != ')')
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	if (tos - h > 1)
		build_tensor(h);
}

void
get_token_skip_newlines(void)
{
	scan_level++;
	get_token();
	scan_level--;
}

void
get_token(void)
{
	get_token_nib();
	if (scan_level)
		while (token == T_NEWLINE)
			get_token_nib();
}

void
get_token_nib(void)
{
	// skip spaces

	while (*scan_str != '\0' && *scan_str != '\n' && *scan_str != '\r' && (*scan_str < 33 || *scan_str > 126))
		scan_str++;

	token_str = scan_str;

	// end of string?

	if (*scan_str == '\0') {
		token = T_END;
		return;
	}

	// newline?

	if (*scan_str == '\n' || *scan_str == '\r') {
		scan_str++;
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (*scan_str == '#' || (scan_str[0] == '-' && scan_str[1] == '-')) {
		while (*scan_str && *scan_str != '\n')
			scan_str++;
		if (*scan_str)
			scan_str++;
		token = T_NEWLINE;
		return;
	}

	// number?

	if (isdigit(*scan_str) || *scan_str == '.') {
		while (isdigit(*scan_str))
			scan_str++;
		if (*scan_str == '.') {
			scan_str++;
			while (isdigit(*scan_str))
				scan_str++;
			if (token_str + 1 == scan_str)
				scan_error("expected decimal digit"); // only a decimal point
			token = T_DOUBLE;
		} else
			token = T_INTEGER;
		update_token_buf(token_str, scan_str);
		return;
	}

	// symbol?

	if (isalpha(*scan_str)) {
		while (isalnum(*scan_str))
			scan_str++;
		if (*scan_str == '(')
			token = T_FUNCTION;
		else
			token = T_SYMBOL;
		update_token_buf(token_str, scan_str);
		return;
	}

	// string ?

	if (*scan_str == '"') {
		scan_str++;
		while (*scan_str != '"') {
			if (*scan_str == '\0' || *scan_str == '\n') {
				token_str = scan_str;
				scan_error("runaway string");
			}
			scan_str++;
		}
		scan_str++;
		token = T_STRING;
		update_token_buf(token_str + 1, scan_str - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (*scan_str == '=' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_EQ;
		return;
	}

	if (*scan_str == '<' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_LTEQ;
		return;
	}

	if (*scan_str == '>' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_GTEQ;
		return;
	}

	// single char token

	token = *scan_str++;
}

void
update_token_buf(char *a, char *b)
{
	int m, n;

	n = (int) (b - a);

	// Let n == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * (n / 1000 + 1); // m is a multiple of 1000

	if (m > token_buf_len) {
		if (token_buf)
			free(token_buf);
		token_buf = alloc_mem(m);
		token_buf_len = m;
	}

	strncpy(token_buf, a, n);
	token_buf[n] = '\0';
}

void
scan_error(char *errmsg)
{
	trace2 = scan_str;
	stopf(errmsg);
}

// There are n expressions on the stack, possibly tensors.
// This function assembles the stack expressions into a single tensor.
// For example, at the top level of the expression ((a,b),(c,d)), the vectors
// (a,b) and (c,d) would be on the stack.

void
build_tensor(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h, *p2;

	p2 = alloc_tensor(n);
	p2->u.tensor->ndim = 1;
	p2->u.tensor->dim[0] = n;
	for (i = 0; i < n; i++)
		p2->u.tensor->elem[i] = s[i];

	tos = h;

	push(p2);
}

void
static_negate(void)
{
	struct atom *p1;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		negate();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		push_symbol(MULTIPLY);
		if (isnum(cadr(p1))) {
			push(cadr(p1)); // A
			negate();
			push(cddr(p1)); // B
		} else {
			push_integer(-1); // A
			push(cdr(p1)); // B
		}
		cons(); // prepend A to B
		cons(); // prepend MULTIPLY
		return;
	}

	push_symbol(MULTIPLY);
	push_integer(-1);
	push(p1);
	list(3);
}

void
static_reciprocate(void)
{
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		if (!isinteger1(p1))
			push(p1);
		push_symbol(POWER);
		push(p2);
		push_integer(-1);
		list(3);
		return;
	}

	if (isnum(p1) && isnum(p2)) {
		push(p1);
		push(p2);
		divide();
		return;
	}

	if (isnum(p2)) {
		if (!isinteger1(p1))
			push(p1);
		push(p2);
		reciprocate();
		return;
	}

	if (car(p2) == symbol(POWER) && isnum(caddr(p2))) {
		if (!isinteger1(p1))
			push(p1);
		push_symbol(POWER);
		push(cadr(p2));
		push(caddr(p2));
		negate();
		list(3);
		return;
	}

	if (!isinteger1(p1))
		push(p1);

	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
void
push(struct atom *p)
{
	if (tos < 0 || tos >= STACKSIZE)
		exitf("stack error, circular definition?");
	stack[tos++] = p;
	if (tos > max_tos)
		max_tos = tos;
}

struct atom *
pop(void)
{
	if (tos < 1 || tos > STACKSIZE)
		exitf("stack error");
	return stack[--tos];
}

void
fpush(struct atom *p)
{
	if (tof < 0 || tof >= FRAMESIZE)
		exitf("frame error, circular definition?");
	frame[tof++] = p;
	if (tof > max_tof)
		max_tof = tof;
}

struct atom *
fpop(void)
{
	if (tof < 1 || tof > FRAMESIZE)
		exitf("frame error");
	return frame[--tof];
}

void
save_symbol(struct atom *p)
{
	fpush(p);
	fpush(get_binding(p));
	fpush(get_usrfunc(p));
}

void
restore_symbol(void)
{
	struct atom *p1, *p2, *p3;
	p3 = fpop();
	p2 = fpop();
	p1 = fpop();
	set_symbol(p1, p2, p3);
}

void
dupl(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	push(p1);
}

void
swap(void)
{
	struct atom *p1, *p2;
	p1 = pop();
	p2 = pop();
	push(p1);
	push(p2);
}

void
push_integer(int n)
{
	switch (n) {
	case 0:
		push(zero);
		break;
	case 1:
		push(one);
		break;
	case -1:
		push(minusone);
		break;
	default:
		if (n < 0)
			push_bignum(MMINUS, mint(-n), mint(1));
		else
			push_bignum(MPLUS, mint(n), mint(1));
		break;
	}
}

void
push_rational(int a, int b)
{
	if (a < 0)
		push_bignum(MMINUS, mint(-a), mint(b));
	else
		push_bignum(MPLUS, mint(a), mint(b));
}

void
push_bignum(int sign, uint32_t *a, uint32_t *b)
{
	struct atom *p;

	// normalize zero

	if (MZERO(a)) {
		sign = MPLUS;
		if (!MEQUAL(b, 1)) {
			mfree(b);
			b = mint(1);
		}
	}

	p = alloc_atom();
	p->atomtype = RATIONAL;
	p->sign = sign;
	p->u.q.a = a;
	p->u.q.b = b;

	push(p);
}

int
pop_integer(void)
{
	int n;
	struct atom *p;

	p = pop();

	if (!issmallinteger(p))
		stopf("small integer expected");

	if (isrational(p)) {
		n = p->u.q.a[0];
		if (isnegativenumber(p))
			n = -n;
	} else
		n = (int) p->u.d;

	return n;
}

void
push_double(double d)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = DOUBLE;
	p->u.d = d;
	push(p);
}

double
pop_double(void)
{
	double a, b, d;
	struct atom *p;

	p = pop();

	if (!isnum(p))
		stopf("number expected");

	if (isdouble(p))
		d = p->u.d;
	else {
		a = mfloat(p->u.q.a);
		b = mfloat(p->u.q.b);
		d = a / b;
		if (isnegativenumber(p))
			d = -d;
	}

	return d;
}

void
push_string(char *s)
{
	struct atom *p;
	p = alloc_str();
	s = strdup(s);
	if (s == NULL)
		exit(1);
	p->u.str = s;
	push(p);
}
// symbol lookup, create symbol if not found

struct atom *
lookup(char *s)
{
	int i, k;
	struct atom *p;

	if (isupper(*s))
		k = BUCKETSIZE * (*s - 'A');
	else if (islower(*s))
		k = BUCKETSIZE * (*s - 'a');
	else
		k = BUCKETSIZE * 26;

	for (i = 0; i < BUCKETSIZE; i++) {
		p = symtab[k + i];
		if (p == NULL)
			break;
		if (strcmp(s, printname(p)) == 0)
			return p;
	}

	if (i == BUCKETSIZE)
		stopf("symbol table full");

	p = alloc_atom();
	s = strdup(s);
	if (s == NULL)
		exit(1);
	p->atomtype = USYM;
	p->u.usym.name = s;
	p->u.usym.index = k + i;
	symtab[k + i] = p;
	usym_count++;

	return p;
}

char *
printname(struct atom *p)
{
	if (iskeyword(p))
		return p->u.ksym.name;
	else if (isusersymbol(p))
		return p->u.usym.name;
	else
		return "?";
}

void
set_symbol(struct atom *p1, struct atom *p2, struct atom *p3)
{
	if (!isusersymbol(p1))
		stopf("symbol error");
	binding[p1->u.usym.index] = p2;
	usrfunc[p1->u.usym.index] = p3;
}

struct atom *
get_binding(struct atom *p1)
{
	struct atom *p2;
	if (!isusersymbol(p1))
		stopf("symbol error");
	p2 = binding[p1->u.usym.index];
	if (p2 == NULL || p2 == symbol(NIL))
		p2 = p1; // symbol binds to itself
	return p2;
}

struct atom *
get_usrfunc(struct atom *p)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	p = usrfunc[p->u.usym.index];
	if (p == NULL)
		p = symbol(NIL);
	return p;
}

struct se {
	char *str;
	int index;
	void (*func)(struct atom *);
} stab[] = {

	{ "abs",		ABS,		eval_abs		},
	{ "adj",		ADJ,		eval_adj		},
	{ "and",		AND,		eval_and		},
	{ "arccos",		ARCCOS,		eval_arccos		},
	{ "arccosh",		ARCCOSH,	eval_arccosh		},
	{ "arcsin",		ARCSIN,		eval_arcsin		},
	{ "arcsinh",		ARCSINH,	eval_arcsinh		},
	{ "arctan",		ARCTAN,		eval_arctan		},
	{ "arctanh",		ARCTANH,	eval_arctanh		},
	{ "arg",		ARG,		eval_arg		},

	{ "binding",		BINDING,	eval_binding		},

	{ "C",			C_UPPER,	NULL			},
	{ "c",			C_LOWER,	NULL			},
	{ "ceiling",		CEILING,	eval_ceiling		},
	{ "check",		CHECK,		eval_check		},
	{ "circexp",		CIRCEXP,	eval_circexp		},
	{ "clear",		CLEAR,		eval_clear		},
	{ "clock",		CLOCK,		eval_clock		},
	{ "cofactor",		COFACTOR,	eval_cofactor		},
	{ "conj",		CONJ,		eval_conj		},
	{ "contract",		CONTRACT,	eval_contract		},
	{ "cos",		COS,		eval_cos		},
	{ "cosh",		COSH,		eval_cosh		},

	{ "D",			D_UPPER,	NULL			},
	{ "d",			D_LOWER,	NULL			},
	{ "defint",		DEFINT,		eval_defint		},
	{ "denominator",	DENOMINATOR,	eval_denominator	},
	{ "derivative",		DERIVATIVE,	eval_derivative		},
	{ "det",		DET,		eval_det		},
	{ "dim",		DIM,		eval_dim		},
	{ "do",			DO,		eval_do			},
	{ "dot",		DOT,		eval_inner		},
	{ "draw",		DRAW,		eval_draw		},

	{ "eigenvec",		EIGENVEC,	eval_eigenvec		},
	{ "erf",		ERF,		eval_erf		},
	{ "erfc",		ERFC,		eval_erfc		},
	{ "eval",		EVAL,		eval_eval		},
	{ "exit",		EXIT,		eval_exit		},
	{ "exp",		EXP,		eval_exp		},
	{ "expcos",		EXPCOS,		eval_expcos		},
	{ "expcosh",		EXPCOSH,	eval_expcosh		},
	{ "expsin",		EXPSIN,		eval_expsin		},
	{ "expsinh",		EXPSINH,	eval_expsinh		},
	{ "exptan",		EXPTAN,		eval_exptan		},
	{ "exptanh",		EXPTANH,	eval_exptanh		},

	{ "factorial",		FACTORIAL,	eval_factorial		},
	{ "float",		FLOATF,		eval_float		},
	{ "floor",		FLOOR,		eval_floor		},
	{ "for",		FOR,		eval_for		},

	{ "H",			H_UPPER,	NULL			},
	{ "h",			H_LOWER,	NULL			},
	{ "hadamard",		HADAMARD,	eval_hadamard		},

	{ "I",			I_UPPER,	NULL			},
	{ "i",			I_LOWER,	NULL			},
	{ "imag",		IMAG,		eval_imag		},
	{ "infixform",		INFIXFORM,	eval_infixform		},
	{ "inner",		INNER,		eval_inner		},
	{ "integral",		INTEGRAL,	eval_integral		},
	{ "inv",		INV,		eval_inv		},

	{ "J",			J_UPPER,	NULL			},
	{ "j",			J_LOWER,	NULL			},

	{ "kronecker",		KRONECKER,	eval_kronecker		},

	{ "last",		LAST,		NULL			},
	{ "log",		LOG,		eval_log		},

	{ "mag",		MAG,		eval_mag		},
	{ "minor",		MINOR,		eval_minor		},
	{ "minormatrix",	MINORMATRIX,	eval_minormatrix	},
	{ "mod",		MOD,		eval_mod		},

	{ "nil",		NIL,		eval_nil		},
	{ "noexpand",		NOEXPAND,	eval_noexpand		},
	{ "not",		NOT,		eval_not		},
	{ "nroots",		NROOTS,		eval_nroots		},
	{ "number",		NUMBER,		eval_number		},
	{ "numerator",		NUMERATOR,	eval_numerator		},

	{ "or",			OR,		eval_or			},
	{ "outer",		OUTER,		eval_outer		},

	{ "p",			P_LOWER,	NULL			},
	{ "P",			P_UPPER,	NULL			},
	{ "pi",			PI,		NULL			},
	{ "polar",		POLAR,		eval_polar		},
	{ "prefixform",		PREFIXFORM,	eval_prefixform		},
	{ "print",		PRINT,		eval_print		},
	{ "product",		PRODUCT,	eval_product		},

	{ "Q",			Q_UPPER,	NULL			},
	{ "q",			Q_LOWER,	NULL			},
	{ "quote",		QUOTE,		eval_quote		},

	{ "R",			R_UPPER,	NULL			},
	{ "r",			R_LOWER,	NULL			},
	{ "rank",		RANK,		eval_rank		},
	{ "rationalize",	RATIONALIZE,	eval_rationalize	},
	{ "real",		REAL,		eval_real		},
	{ "rect",		RECTF,		eval_rect		},
	{ "roots",		ROOTS,		eval_roots		},
	{ "rotate",		ROTATE,		eval_rotate		},
	{ "run",		RUN,		eval_run		},

	{ "S",			S_UPPER,	NULL			},
	{ "s",			S_LOWER,	NULL			},
	{ "sgn",		SGN,		eval_sgn		},
	{ "simplify",		SIMPLIFY,	eval_simplify		},
	{ "sin",		SIN,		eval_sin		},
	{ "sinh",		SINH,		eval_sinh		},
	{ "sqrt",		SQRT,		eval_sqrt		},
	{ "status",		STATUS,		eval_status		},
	{ "stop",		STOP,		eval_stop		},
	{ "subst",		SUBST,		eval_subst		},
	{ "sum",		SUM,		eval_sum		},

	{ "T",			T_UPPER,	NULL			},
	{ "t",			T_LOWER,	NULL			},
	{ "tan",		TAN,		eval_tan		},
	{ "tanh",		TANH,		eval_tanh		},
	{ "taylor",		TAYLOR,		eval_taylor		},
	{ "test",		TEST,		eval_test		},
	{ "testeq",		TESTEQ,		eval_testeq		},
	{ "testge",		TESTGE,		eval_testge		},
	{ "testgt",		TESTGT,		eval_testgt		},
	{ "testle",		TESTLE,		eval_testle		},
	{ "testlt",		TESTLT,		eval_testlt		},
	{ "trace",		TRACE,		NULL			},
	{ "transpose",		TRANSPOSE,	eval_transpose		},
	{ "tty",		TTY,		NULL			},

	{ "U",			U_UPPER,	NULL			},
	{ "u",			U_LOWER,	NULL			},
	{ "unit",		UNIT,		eval_unit		},

	{ "V",			V_UPPER,	NULL			},
	{ "v",			V_LOWER,	NULL			},

	{ "W",			W_UPPER,	NULL			},
	{ "w",			W_LOWER,	NULL			},

	{ "X",			X_UPPER,	NULL			},
	{ "x",			X_LOWER,	NULL			},

	{ "Y",			Y_UPPER,	NULL			},
	{ "y",			Y_LOWER,	NULL			},

	{ "Z",			Z_UPPER,	NULL			},
	{ "z",			Z_LOWER,	NULL			},
	{ "zero",		ZERO,		eval_zero		},

	{ "+",			ADD,		eval_add		},
	{ "*",			MULTIPLY,	eval_multiply		},
	{ "^",			POWER,		eval_power		},
	{ "[",			INDEX,		eval_index		},
	{ "=",			SETQ,		eval_setq		},
	{ "$e",			EXP1,		NULL			},
	{ "$a",			SA,		NULL			},
	{ "$b",			SB,		NULL			},
	{ "$x",			SX,		NULL			},
	{ "$1",			ARG1,		NULL			},
	{ "$2",			ARG2,		NULL			},
	{ "$3",			ARG3,		NULL			},
	{ "$4",			ARG4,		NULL			},
	{ "$5",			ARG5,		NULL			},
	{ "$6",			ARG6,		NULL			},
	{ "$7",			ARG7,		NULL			},
	{ "$8",			ARG8,		NULL			},
	{ "$9",			ARG9,		NULL			},
};

void
init_symbol_table(void)
{
	int i, n;
	char *s;
	struct atom *p;

	for (i = 0; i < 27 * BUCKETSIZE; i++) {
		symtab[i] = NULL;
		binding[i] = NULL;
		usrfunc[i] = NULL;
	}

	n = sizeof stab / sizeof (struct se);

	for (i = 0; i < n; i++) {
		p = alloc_atom();
		s = strdup(stab[i].str);
		if (s == NULL)
			exit(1);
		if (stab[i].func) {
			p->atomtype = KSYM;
			p->u.ksym.name = s;
			p->u.ksym.func = stab[i].func;
			ksym_count++;
		} else {
			p->atomtype = USYM;
			p->u.usym.name = s;
			p->u.usym.index = stab[i].index;
			usym_count++;
		}
		symtab[stab[i].index] = p;
	}
}
