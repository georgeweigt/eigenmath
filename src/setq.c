#include "defs.h"

void
eval_setq(void)
{
	if (caadr(p1) == symbol(INDEX))
		setq_indexed();
	else if (iscons(cadr(p1)))
		setq_userfunc();
	else {
		if (!issymbol(cadr(p1)))
			stop("assignment: symbol expected");
		push(caddr(p1));
		eval();
		p2 = pop();
		set_binding(cadr(p1), p2);
	}
	push_symbol(NIL);
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

#undef S
#undef LVAL
#undef RVAL

#define S p3
#define LVAL p4
#define RVAL p5

void
setq_indexed(void)
{
	int h;

	S = cadadr(p1);

	if (!issymbol(S))
		stop("symbol expected");

	push(S);
	eval();
	LVAL = pop();

	push(caddr(p1));
	eval();
	RVAL = pop();

	// eval indices

	p1 = cddadr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	set_component(h);

	set_binding(S, LVAL);
}

void
set_component(int h)
{
	save();
	set_component_nib(h);
	restore();
}

void
set_component_nib(int h)
{
	int i, k, m, n, t;

	if (!istensor(LVAL))
		stop("index error");

	// n is the number of indices

	n = tos - h;

	if (n < 1 || n > LVAL->u.tensor->ndim)
		stop("index error");

	// k is the combined index

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL->u.tensor->dim[i])
			stop("index error");
		k = k * LVAL->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (istensor(RVAL)) {
		m = RVAL->u.tensor->ndim;
		if (n + m != LVAL->u.tensor->ndim)
			stop("index error");
		for (i = 0; i < m; i++)
			if (LVAL->u.tensor->dim[n + i] != RVAL->u.tensor->dim[i])
				stop("index error");
		m = RVAL->u.tensor->nelem;
		for (i = 0; i < m; i++)
			LVAL->u.tensor->elem[m * k + i] = RVAL->u.tensor->elem[i];
	} else {
		if (n != LVAL->u.tensor->ndim)
			stop("index error");
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

#undef F
#undef A
#undef B
#undef T

#define F p3 // F points to the function name
#define A p4 // A points to the argument list
#define B p5 // B points to the function body
#define T p6

void
setq_userfunc(void)
{
	int h;

	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!isusersymbol(F))
		stop("function definition error");

	// convert args

	h = tos;
	p1 = A;

	while (iscons(p1)) {
		p2 = car(p1);
		if (!isusersymbol(p2))
			stop("function definition error");
		push(dual(p2));
		p1 = cdr(p1);
	}

	list(tos - h);
	T = pop();

	set_binding(F, B);
	set_arglist(F, T);

	// convert body

	push(B);

	p1 = A;
	p2 = T;

	while (iscons(p1)) {
		push(car(p1));
		push(car(p2));
		subst();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	B = pop();
	set_binding(dual(F), B);
}
