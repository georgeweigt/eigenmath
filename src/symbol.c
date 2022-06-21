#include "defs.h"

// symbol lookup, create symbol if not found

struct atom *
lookup(char *s)
{
	int c, i, k;
	char *t;
	struct atom *p;

	c = tolower(*s) - 'a';

	if (c < 0 || c > 25)
		c = 26;

	k = NSYM * c;

	for (i = 0; i < NSYM; i++) {
		p = symtab[k + i];
		if (p == NULL)
			break;
		if (p->k == KSYM)
			t = p->u.ksym.name;
		else
			t = p->u.usym.name;
		if (strcmp(s, t) == 0)
			return p;
	}

	if (i == NSYM)
		stop("symbol table full");

	p = alloc();
	s = strdup(s);

	if (s == NULL)
		malloc_kaput();

	p->k = USYM;
	p->u.usym.name = s;
	p->u.usym.index = k + i;

	symtab[k + i] = p;

	binding[k + i] = symbol(NIL);
	usrfunc[k + i] = symbol(NIL);

	usym_count++;

	return p;
}

char *
printname(struct atom *p)
{
	if (iskeyword(p))
		return p->u.ksym.name;

	if (isusersymbol(p))
		return p->u.usym.name;

	stop("symbol error");

	return "?";
}

void
set_symbol(struct atom *p, struct atom *b, struct atom *u)
{
	int k;

	if (!isusersymbol(p))
		stop("symbol error");

	k = p->u.usym.index;

	if (journaling) {
		if (toj + 3 > JOURNALSIZE)
			kaput("journal error");
		journal[toj + 0] = p;
		journal[toj + 1] = binding[k];
		journal[toj + 2] = usrfunc[k];
		toj += 3;
		if (toj > max_journal)
			max_journal = toj;
	}

	binding[k] = b;
	usrfunc[k] = u;
}

// restore symbol table

void
undo(void)
{
	int k;
	struct atom *p;
	while (toj > 0) {
		toj -= 3;
		p = journal[toj + 0];
		k = p->u.usym.index;
		binding[k] = journal[toj + 1];
		usrfunc[k] = journal[toj + 2];
	}
}

struct atom *
get_binding(struct atom *p)
{
	if (!isusersymbol(p))
		stop("symbol error");
	return binding[p->u.usym.index];
}

struct atom *
get_usrfunc(struct atom *p)
{
	if (!isusersymbol(p))
		stop("symbol error");
	return usrfunc[p->u.usym.index];
}

struct se {
	char *str;
	int index;
	void (*func)(struct atom *);
};

struct se stab[] = {

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
	{ "atomize",		ATOMIZE,	eval_atomize		},

	{ "besselj",		BESSELJ,	eval_besselj		},
	{ "bessely",		BESSELY,	eval_bessely		},
	{ "binding",		BINDING,	eval_binding		},

	{ "C",			C_UPPER,	NULL			},
	{ "c",			C_LOWER,	NULL			},
	{ "ceiling",		CEILING,	eval_ceiling		},
	{ "check",		CHECK,		eval_check		},
	{ "circexp",		CIRCEXP,	eval_circexp		},
	{ "clear",		CLEAR,		eval_clear		},
	{ "clock",		CLOCK,		eval_clock		},
	{ "coeff",		COEFF,		eval_coeff		},
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

	{ "eigen",		EIGEN,		eval_eigen		},
	{ "eigenval",		EIGENVAL,	eval_eigenval		},
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

	{ "factor",		FACTOR,		eval_factor		},
	{ "factorial",		FACTORIAL,	eval_factorial		},
	{ "filter",		FILTER,		eval_filter		},
	{ "float",		FLOATF,		eval_float		},
	{ "floor",		FLOOR,		eval_floor		},
	{ "for",		FOR,		eval_for		},

	{ "gcd",		GCD,		eval_gcd		},

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
	{ "isprime",		ISPRIME,	eval_isprime		},

	{ "J",			J_UPPER,	NULL			},
	{ "j",			J_LOWER,	NULL			},

	{ "kronecker",		KRONECKER,	eval_kronecker		},

	{ "last",		LAST,		NULL			},
	{ "latex",		LATEX,		eval_latex		},
	{ "lcm",		LCM,		eval_lcm		},
	{ "log",		LOG,		eval_log		},

	{ "mag",		MAG,		eval_mag		},
	{ "mathjax",		MATHJAX,	eval_mathjax		},
	{ "mathml",		MATHML,		eval_mathml		},
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
	{ "quotient",		QUOTIENT,	eval_quotient		},

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

	for (i = 0; i < 27 * NSYM; i++)
		symtab[i] = NULL;

	n = sizeof stab / sizeof (struct se);

	for (i = 0; i < n; i++) {
		p = alloc();
		s = strdup(stab[i].str);
		if (s == NULL)
			malloc_kaput();
		if (stab[i].func) {
			p->k = KSYM;
			p->u.ksym.name = s;
			p->u.ksym.func = stab[i].func;
			ksym_count++;
		} else {
			p->k = USYM;
			p->u.usym.name = s;
			p->u.usym.index = stab[i].index;
			usym_count++;
		}
		symtab[stab[i].index] = p;
	}

	clear_symbols();
}

void
clear_symbols(void)
{
	int i;
	for (i = 0; i < 27 * NSYM; i++) {
		binding[i] = symbol(NIL);
		usrfunc[i] = symbol(NIL);
	}
}
