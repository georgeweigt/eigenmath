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
#define NSYM 100

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
		struct {
			char *kname;
			void (*func)(void);
		} ksym;
		struct {
			char *uname;
			uint32_t index;
		} usym;
		struct {
			uint32_t *a; // rational number a over b
			uint32_t *b;
		} q;
		double d;
		char *str;
		struct tensor *tensor;
	} u;
	uint8_t k, tag, sign;
};

#define CONS		0
#define KSYM		1
#define USYM		2
#define RATIONAL	3
#define DOUBLE		4
#define STR		5
#define TENSOR		6

#define STI(x, y) ((int) ((x) - 'a') * NSYM + (y))

#define METAA		STI('a',  0)
#define ABS		STI('a',  1)
#define ADD		STI('a',  2)
#define ADJ		STI('a',  3)
#define AND		STI('a',  4)
#define ARCCOS		STI('a',  5)
#define ARCCOSH		STI('a',  6)
#define ARCSIN		STI('a',  7)
#define ARCSINH		STI('a',  8)
#define ARCTAN		STI('a',  9)
#define ARCTANH		STI('a', 10)
#define ARG		STI('a', 11)
#define ATOMIZE		STI('a', 12)

#define METAB		STI('b',  0)
#define BESSELJ		STI('b',  1)
#define BESSELY		STI('b',  2)
#define BINDING		STI('b',  3)
#define BINOMIAL	STI('b',  4)

#define CEILING		STI('c',  0)
#define CHECK		STI('c',  1)
#define CHOOSE		STI('c',  2)
#define CIRCEXP		STI('c',  3)
#define CLEAR		STI('c',  4)
#define CLOCK		STI('c',  5)
#define COEFF		STI('c',  6)
#define COFACTOR	STI('c',  7)
#define CONJ		STI('c',  8)
#define CONTRACT	STI('c',  9)
#define COS		STI('c', 10)
#define COSH		STI('c', 11)

#define SYMBOL_D	STI('d',  0)
#define DEFINT		STI('d',  1)
#define DEGREE		STI('d',  2)
#define DENOMINATOR	STI('d',  3)
#define DERIVATIVE	STI('d',  4)
#define DET		STI('d',  5)
#define DIM		STI('d',  6)
#define DO		STI('d',  7)
#define DOT		STI('d',  8)
#define DRAW		STI('d',  9)

#define EIGEN		STI('e',  0)
#define EIGENVAL	STI('e',  1)
#define EIGENVEC	STI('e',  2)
#define ERF		STI('e',  3)
#define ERFC		STI('e',  4)
#define EVAL		STI('e',  5)
#define EXIT		STI('e',  6)
#define EXP		STI('e',  7)
#define EXP1		STI('e',  8)
#define EXPAND		STI('e',  9)
#define EXPCOS		STI('e', 10)
#define EXPCOSH		STI('e', 11)
#define EXPSIN		STI('e', 12)
#define EXPSINH		STI('e', 13)
#define EXPTAN		STI('e', 14)
#define EXPTANH		STI('e', 15)

#define FACTOR		STI('f',  0)
#define FACTORIAL	STI('f',  1)
#define FILTER		STI('f',  2)
#define FLOATF		STI('f',  3)
#define FLOOR		STI('f',  4)
#define FOR		STI('f',  5)

#define GCD		STI('g',  0)

#define HERMITE		STI('h',  0)
#define HILBERT		STI('h',  1)

#define SYMBOL_I	STI('i',  0)
#define IMAG		STI('i',  1)
#define INDEX		STI('i',  2)
#define INNER		STI('i',  3)
#define INTEGRAL	STI('i',  4)
#define INV		STI('i',  5)
#define ISPRIME		STI('i',  6)

#define SYMBOL_J	STI('j',  0)

#define LAGUERRE	STI('l',  0)
#define LAST		STI('l',  1)
#define LATEX		STI('l',  2)
#define LCM		STI('l',  3)
#define LEADING		STI('l',  4)
#define LEGENDRE	STI('l',  5)
#define LISP		STI('l',  6)
#define LOG		STI('l',  7)

#define MAG		STI('m',  0)
#define MATHJAX		STI('m',  1)
#define MATHML		STI('m',  2)
#define MOD		STI('m',  3)
#define MULTIPLY	STI('m',  4)

#define NIL		STI('n',  0)
#define NOT		STI('n',  1)
#define NROOTS		STI('n',  2)
#define NUMBER		STI('n',  3)
#define NUMERATOR	STI('n',  4)

#define OR		STI('o',  0)
#define OUTER		STI('o',  1)

#define PI		STI('p',  0)
#define POLAR		STI('p',  1)
#define POWER		STI('p',  2)
#define PRIME		STI('p',  3)
#define PRINT		STI('p',  4)
#define PRODUCT		STI('p',  5)

#define QUOTE		STI('q',  0)
#define QUOTIENT	STI('q',  1)

#define RANK		STI('r',  0)
#define RATIONALIZE	STI('r',  1)
#define REAL		STI('r',  2)
#define RECTF		STI('r',  3)
#define ROOTS		STI('r',  4)
#define RUN		STI('r',  5)

#define SYMBOL_S	STI('s',  0)
#define SETQ		STI('s',  1)
#define SGN		STI('s',  2)
#define SIMPLIFY	STI('s',  3)
#define SIN		STI('s',  4)
#define SINH		STI('s',  5)
#define SQRT		STI('s',  6)
#define STATUS		STI('s',  7)
#define STOP		STI('s',  8)
#define STRING		STI('s',  9)
#define SUBST		STI('s', 10)
#define SUM		STI('s', 11)

#define SYMBOL_T	STI('t',  0)
#define TAN		STI('t',  1)
#define TANH		STI('t',  2)
#define TAYLOR		STI('t',  3)
#define TEST		STI('t',  4)
#define TESTEQ		STI('t',  5)
#define TESTGE		STI('t',  6)
#define TESTGT		STI('t',  7)
#define TESTLE		STI('t',  8)
#define TESTLT		STI('t',  9)
#define TRACE		STI('t', 10)
#define TRANSPOSE	STI('t', 11)
#define TTY		STI('t', 12)

#define UNIT		STI('u',  0)

#define SYMBOL_X	STI('x',  0)
#define METAX		STI('x',  1)
#define SPECX		STI('x',  2)

#define SYMBOL_Y	STI('y',  0)

#define SYMBOL_Z	STI('z',  0)
#define ZERO		STI('z',  1)

#define MAXPRIMETAB 10000
#define MAXDIM 24

struct tensor {
	int ndim;
	int dim[MAXDIM];
	int nelem;
	struct atom *elem[1];
};

#define symbol(x) symtab[x]
#define push_symbol(x) push(symbol(x))
#define iscons(p) ((p)->k == CONS)
#define isrational(p) ((p)->k == RATIONAL)
#define isdouble(p) ((p)->k == DOUBLE)
#define isnum(p) (isrational(p) || isdouble(p))
#define isstr(p) ((p)->k == STR)
#define istensor(p) ((p)->k == TENSOR)
#define issymbol(p) ((p)->k == KSYM || (p)->k == USYM)
#define iskeyword(p) ((p)->k == KSYM)

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
