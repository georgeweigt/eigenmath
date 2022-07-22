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
#define FRAMESIZE 1000
#define BLOCKSIZE 100000
#define MAXBLOCKS 100
#define NSYM 100
#define JOURNALSIZE 1000

// MAXBLOCKS * BLOCKSIZE = 10,000,000 atoms

// MAXBLOCKS * BLOCKSIZE * sizeof (struct atom) = 240,000,000 bytes

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

#define ABS		(0 * NSYM + 0)
#define ADJ		(0 * NSYM + 1)
#define AND		(0 * NSYM + 2)
#define ARCCOS		(0 * NSYM + 3)
#define ARCCOSH		(0 * NSYM + 4)
#define ARCSIN		(0 * NSYM + 5)
#define ARCSINH		(0 * NSYM + 6)
#define ARCTAN		(0 * NSYM + 7)
#define ARCTANH		(0 * NSYM + 8)
#define ARG		(0 * NSYM + 9)

#define BINDING		(1 * NSYM + 0)

#define C_UPPER		(2 * NSYM + 0)
#define C_LOWER		(2 * NSYM + 1)
#define CEILING		(2 * NSYM + 2)
#define CHECK		(2 * NSYM + 3)
#define CIRCEXP		(2 * NSYM + 4)
#define CLEAR		(2 * NSYM + 5)
#define CLOCK		(2 * NSYM + 6)
#define COFACTOR	(2 * NSYM + 7)
#define CONJ		(2 * NSYM + 8)
#define CONTRACT	(2 * NSYM + 9)
#define COS		(2 * NSYM + 10)
#define COSH		(2 * NSYM + 11)

#define D_UPPER		(3 * NSYM + 0)
#define D_LOWER		(3 * NSYM + 1)
#define DEFINT		(3 * NSYM + 2)
#define DENOMINATOR	(3 * NSYM + 3)
#define DERIVATIVE	(3 * NSYM + 4)
#define DET		(3 * NSYM + 5)
#define DIM		(3 * NSYM + 6)
#define DO		(3 * NSYM + 7)
#define DOT		(3 * NSYM + 8)
#define DRAW		(3 * NSYM + 9)

#define EIGENVEC	(4 * NSYM + 0)
#define ERF		(4 * NSYM + 1)
#define ERFC		(4 * NSYM + 2)
#define EVAL		(4 * NSYM + 3)
#define EXIT		(4 * NSYM + 4)
#define EXP		(4 * NSYM + 5)
#define EXPCOS		(4 * NSYM + 6)
#define EXPCOSH		(4 * NSYM + 7)
#define EXPSIN		(4 * NSYM + 8)
#define EXPSINH		(4 * NSYM + 9)
#define EXPTAN		(4 * NSYM + 10)
#define EXPTANH		(4 * NSYM + 11)

#define FACTOR		(5 * NSYM + 0)
#define FACTORIAL	(5 * NSYM + 1)
#define FILTER		(5 * NSYM + 2)
#define FLOATF		(5 * NSYM + 3)
#define FLOOR		(5 * NSYM + 4)
#define FOR		(5 * NSYM + 5)

#define H_UPPER		(7 * NSYM + 0)
#define H_LOWER		(7 * NSYM + 1)
#define HADAMARD	(7 * NSYM + 2)

#define I_UPPER		(8 * NSYM + 0)
#define I_LOWER		(8 * NSYM + 1)
#define IMAG		(8 * NSYM + 2)
#define INFIXFORM	(8 * NSYM + 3)
#define INNER		(8 * NSYM + 4)
#define INTEGRAL	(8 * NSYM + 5)
#define INV		(8 * NSYM + 6)

#define J_UPPER		(9 * NSYM + 0)
#define J_LOWER		(9 * NSYM + 1)

#define KRONECKER	(10 * NSYM + 0)

#define LAST		(11 * NSYM + 0)
#define LATEX		(11 * NSYM + 1)
#define LOG		(11 * NSYM + 2)

#define MAG		(12 * NSYM + 0)
#define MATHJAX		(12 * NSYM + 1)
#define MATHML		(12 * NSYM + 2)
#define MINOR		(12 * NSYM + 3)
#define MINORMATRIX	(12 * NSYM + 4)
#define MOD		(12 * NSYM + 5)

#define NIL		(13 * NSYM + 0)
#define NOEXPAND	(13 * NSYM + 1)
#define NOT		(13 * NSYM + 2)
#define NROOTS		(13 * NSYM + 3)
#define NUMBER		(13 * NSYM + 4)
#define NUMERATOR	(13 * NSYM + 5)

#define OR		(14 * NSYM + 0)
#define OUTER		(14 * NSYM + 1)

#define P_UPPER		(15 * NSYM + 0)
#define P_LOWER		(15 * NSYM + 1)
#define PI		(15 * NSYM + 2)
#define POLAR		(15 * NSYM + 3)
#define PREFIXFORM	(15 * NSYM + 4)
#define PRINT		(15 * NSYM + 5)
#define PRODUCT		(15 * NSYM + 6)

#define Q_UPPER		(16 * NSYM + 0)
#define Q_LOWER		(16 * NSYM + 1)
#define QUOTE		(16 * NSYM + 2)
#define QUOTIENT	(16 * NSYM + 3)

#define R_UPPER		(17 * NSYM + 0)
#define R_LOWER		(17 * NSYM + 1)
#define RANK		(17 * NSYM + 2)
#define RATIONALIZE	(17 * NSYM + 3)
#define REAL		(17 * NSYM + 4)
#define RECTF		(17 * NSYM + 5)
#define ROOTS		(17 * NSYM + 6)
#define ROTATE		(17 * NSYM + 7)
#define RUN		(17 * NSYM + 8)

#define S_UPPER		(18 * NSYM + 0)
#define S_LOWER		(18 * NSYM + 1)
#define SGN		(18 * NSYM + 2)
#define SIMPLIFY	(18 * NSYM + 3)
#define SIN		(18 * NSYM + 4)
#define SINH		(18 * NSYM + 5)
#define SQRT		(18 * NSYM + 6)
#define STATUS		(18 * NSYM + 7)
#define STOP		(18 * NSYM + 8)
#define SUBST		(18 * NSYM + 9)
#define SUM		(18 * NSYM + 10)

#define T_UPPER		(19 * NSYM + 0)
#define T_LOWER		(19 * NSYM + 1)
#define TAN		(19 * NSYM + 2)
#define TANH		(19 * NSYM + 3)
#define TAYLOR		(19 * NSYM + 4)
#define TEST		(19 * NSYM + 5)
#define TESTEQ		(19 * NSYM + 6)
#define TESTGE		(19 * NSYM + 7)
#define TESTGT		(19 * NSYM + 8)
#define TESTLE		(19 * NSYM + 9)
#define TESTLT		(19 * NSYM + 10)
#define TRACE		(19 * NSYM + 11)
#define TRANSPOSE	(19 * NSYM + 12)
#define TTY		(19 * NSYM + 13)

#define U_UPPER		(20 * NSYM + 0)
#define U_LOWER		(20 * NSYM + 1)
#define UNIT		(20 * NSYM + 2)

#define V_UPPER		(21 * NSYM + 0)
#define V_LOWER		(21 * NSYM + 1)

#define W_UPPER		(22 * NSYM + 0)
#define W_LOWER		(22 * NSYM + 1)

#define X_UPPER		(23 * NSYM + 0)
#define X_LOWER		(23 * NSYM + 1)

#define Y_UPPER		(24 * NSYM + 0)
#define Y_LOWER		(24 * NSYM + 1)

#define Z_UPPER		(25 * NSYM + 0)
#define Z_LOWER		(25 * NSYM + 1)
#define ZERO		(25 * NSYM + 2)

#define ADD		(26 * NSYM + 0)
#define MULTIPLY	(26 * NSYM + 1)
#define POWER		(26 * NSYM + 2)
#define INDEX		(26 * NSYM + 3)
#define SETQ		(26 * NSYM + 4)
#define EXP1		(26 * NSYM + 5)
#define SA		(26 * NSYM + 6)
#define SB		(26 * NSYM + 7)
#define SX		(26 * NSYM + 8)
#define ARG1		(26 * NSYM + 9)
#define ARG2		(26 * NSYM + 10)
#define ARG3		(26 * NSYM + 11)
#define ARG4		(26 * NSYM + 12)
#define ARG5		(26 * NSYM + 13)
#define ARG6		(26 * NSYM + 14)
#define ARG7		(26 * NSYM + 15)
#define ARG8		(26 * NSYM + 16)
#define ARG9		(26 * NSYM + 17)

#define MAXDIM 24

struct tensor {
	int ndim;
	int dim[MAXDIM];
	int nelem;
	struct atom *elem[1];
};

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
#define MEQUAL(p, n) (MLENGTH(p) == 1 && (p)[0] == ((uint32_t) n))

#define BLACK 0
#define BLUE 1
#define RED 2

#define Trace fprintf(stderr, "%s %d\n", __func__, __LINE__);
