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
#define BREAK		(1 * BUCKETSIZE + 1)

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
#define EXPFORM		(4 * BUCKETSIZE + 8)
#define EXPSIN		(4 * BUCKETSIZE + 9)
#define EXPSINH		(4 * BUCKETSIZE + 10)
#define EXPTAN		(4 * BUCKETSIZE + 11)
#define EXPTANH		(4 * BUCKETSIZE + 12)

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
#define SUM		(18 * BUCKETSIZE + 9)

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
#define TGAMMA		(19 * BUCKETSIZE + 11)
#define TRACE		(19 * BUCKETSIZE + 12)
#define TRANSPOSE	(19 * BUCKETSIZE + 13)
#define TTY		(19 * BUCKETSIZE + 14)

#define U_UPPER		(20 * BUCKETSIZE + 0)
#define U_LOWER		(20 * BUCKETSIZE + 1)
#define UNIT		(20 * BUCKETSIZE + 2)

#define V_UPPER		(21 * BUCKETSIZE + 0)
#define V_LOWER		(21 * BUCKETSIZE + 1)

#define W_UPPER		(22 * BUCKETSIZE + 0)
#define W_LOWER		(22 * BUCKETSIZE + 1)
#define WHILE		(22 * BUCKETSIZE + 2)

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
extern struct atom *stack[STACKSIZE];
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
extern int breakflag;
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
