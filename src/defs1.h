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

#define STACKSIZE 1000000 // evaluation stack
#define FRAMESIZE 10000 // limits recursion depth, prevents seg fault
#define BLOCKSIZE 100000
#define MAXBLOCKS 250
#define NSYM 1000

// MAXBLOCKS * BLOCKSIZE * sizeof (struct atom) = 600,000,000 bytes

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
//
// Programming notes:
//
// 1. Automatic C variables of type "struct atom *" should not be used.
//    Use the global variables p0-p9 instead.
//    Call save() before using p0-p9, then call restore() when finished.
//    The use of p0-p9 is required by the garbage collector.
//
// 2. Do not use NULL for a car or cdr.
//    Use the pointer returned by symbol(NIL) instead.
//    The code does not test for NULL.

struct atom {
	union {
		struct {
			struct atom *car;
			struct atom *cdr;
		} cons;
		char *printname;
		char *str;
		struct tensor *tensor;
		struct {
			uint32_t *a; // rational number a over b
			uint32_t *b;
		} q;
		double d;
	} u;
	uint8_t k, tag, sign;
};

// the following enum is for struct atom, member k

enum {
	CONS,
	RATIONAL,
	DOUBLE,
	STR,
	TENSOR,
	SYM,
};

// the following enum is for indexing the symbol table

enum {
	ABS,
	ADD,
	ADJ,
	AND,
	ARCCOS,
	ARCCOSH,
	ARCSIN,
	ARCSINH,
	ARCTAN,
	ARCTANH,
	ARG,
	ATOMIZE,
	BESSELJ,
	BESSELY,
	BINDING,
	BINOMIAL,
	CEILING,
	CHECK,
	CHOOSE,
	CIRCEXP,
	CLEAR,
	CLOCK,
	COEFF,
	COFACTOR,
	CONJ,
	CONTRACT,
	COS,
	COSH,
	DEFINT,
	DEGREE,
	DENOMINATOR,
	DERIVATIVE,
	DET,
	DIM,
	DIVISORS,
	DO,
	DOT,
	DRAW,
	EIGEN,
	EIGENVAL,
	EIGENVEC,
	ERF,
	ERFC,
	EVAL,
	EXIT,
	EXP,
	EXPAND,
	EXPCOS,
	EXPCOSH,
	EXPSIN,
	EXPSINH,
	EXPTAN,
	EXPTANH,
	FACTOR,
	FACTORIAL,
	FILTER,
	FLOATF,
	FLOOR,
	FOR,
	GCD,
	HERMITE,
	HILBERT,
	IMAG,
	INDEX,
	INNER,
	INTEGRAL,
	INV,
	ISPRIME,
	LAGUERRE,
	LATEX,
	LCM,
	LEADING,
	LEGENDRE,
	LISP,
	LOG,
	MAG,
	MATHJAX,
	MATHML,
	MOD,
	MULTIPLY,
	NOT,
	NROOTS,
	NUMBER,
	NUMERATOR,
	OR,
	OUTER,
	POLAR,
	POWER,
	PRIME,
	PRINT,
	PRODUCT,
	QUOTE,
	QUOTIENT,
	RANK,
	RATIONALIZE,
	REAL,
	RECTF,
	ROOTS,
	RUN,
	SETQ,
	SGN,
	SIMPLIFY,
	SIN,
	SINH,
	SQRT,
	STATUS,
	STOP,
	STRING,
	SUBST,
	SUM,
	TAN,
	TANH,
	TAYLOR,
	TEST,
	TESTEQ,
	TESTGE,
	TESTGT,
	TESTLE,
	TESTLT,
	TRANSPOSE,
	UNIT,
	ZERO,

	MARK1,	// boundary (symbols above are functions)

	EXP1,	// natural number
	NIL,
	PI,

	MARK2,	// boundary (symbols above cannot be bound)

	METAA,
	METAB,
	METAX,
	SPECX,

	AUTOEXPAND,
	LAST,
	TRACE,
	TTY,

	MARK3,	// start of user defined symbols

	SYMBOL_D,
	SYMBOL_I,
	SYMBOL_J,
	SYMBOL_S,
	SYMBOL_T,
	SYMBOL_X,
	SYMBOL_Y,
	SYMBOL_Z,
};

#define MAXPRIMETAB 10000
#define MAXDIM 24

struct tensor {
	int ndim;
	int dim[MAXDIM];
	int nelem;
	struct atom *elem[1];
};

#define symbol(x) (symtab + (x))
#define push_symbol(x) push(symbol(x))
#define iscons(p) ((p)->k == CONS)
#define isrational(p) ((p)->k == RATIONAL)
#define isdouble(p) ((p)->k == DOUBLE)
#define isnum(p) (isrational(p) || isdouble(p))
#define isstr(p) ((p)->k == STR)
#define istensor(p) ((p)->k == TENSOR)
#define issymbol(p) ((p)->k == SYM)
#define iskeyword(p) ((p)->k == SYM && (p) - symtab < MARK1)
#define isadd(p) (car(p) == symbol(ADD))

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
#define cdaddr(p) cdr(car(cdr(cdr(p))))
#define cadddr(p) car(cdr(cdr(cdr(p))))
#define cddddr(p) cdr(cdr(cdr(cdr(p))))
#define caddddr(p) car(cdr(cdr(cdr(cdr(p)))))
#define cadaddr(p) car(cdr(car(cdr(cdr(p)))))
#define cddaddr(p) cdr(cdr(car(cdr(cdr(p)))))
#define caddadr(p) car(cdr(cdr(car(cdr(p)))))
#define cdddaddr(p) cdr(cdr(cdr(car(cdr(cdr(p))))))
#define caddaddr(p) car(cdr(cdr(car(cdr(cdr(p))))))

#define ERR ((int) 0x80000000)

#define MPLUS 0
#define MMINUS 1
#define MLENGTH(p) (((int *) (p))[-1])
#define MZERO(p) (MLENGTH(p) == 1 && (p)[0] == 0)
#define MEQUAL(p, n) (MLENGTH(p) == 1 && (p)[0] == (n))

#define BLACK 0
#define BLUE 1
#define RED 2

#define Trace printf("%s %d\n", __func__, __LINE__);

extern int primetab[MAXPRIMETAB];
