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

void
setq_indexed(void)
{
	int h;
	p4 = cadadr(p1);
	if (!issymbol(p4))
		stop("indexed assignment: symbol expected");
	h = tos;
	push(caddr(p1));
	eval();
	p2 = cdadr(p1);
	while (iscons(p2)) {
		push(car(p2));
		eval();
		p2 = cdr(p2);
	}
	set_component(tos - h);
	p3 = pop();
	set_binding(p4, p3);
}

#undef LVALUE
#undef RVALUE
#undef TMP

#define LVALUE p1
#define RVALUE p2
#define TMP p3

void
set_component(int n)
{
	save();
	set_component_nib(n);
	restore();
}

void
set_component_nib(int n)
{
	int i, k, m, ndim, t;
	struct atom **s;

	if (n < 3)
		stop("error in indexed assign");

	s = stack + tos - n;

	RVALUE = s[0];

	LVALUE = s[1];

	if (!istensor(LVALUE))
		stop("error in indexed assign");

	ndim = LVALUE->u.tensor->ndim;

	m = n - 2;

	if (m > ndim)
		stop("error in indexed assign");

	k = 0;

	for (i = 0; i < m; i++) {
		push(s[i + 2]);
		t = pop_integer();
		if (t < 1 || t > LVALUE->u.tensor->dim[i])
			stop("error in indexed assign");
		k = k * p1->u.tensor->dim[i] + t - 1;
	}

	for (i = m; i < ndim; i++)
		k = k * p1->u.tensor->dim[i] + 0;

	// copy

	TMP = alloc_tensor(LVALUE->u.tensor->nelem);

	TMP->u.tensor->ndim = LVALUE->u.tensor->ndim;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		TMP->u.tensor->dim[i] = LVALUE->u.tensor->dim[i];

	for (i = 0; i < p1->u.tensor->nelem; i++)
		TMP->u.tensor->elem[i] = LVALUE->u.tensor->elem[i];

	LVALUE = TMP;

	if (ndim == m) {
		if (istensor(RVALUE))
			stop("error in indexed assign");
		LVALUE->u.tensor->elem[k] = RVALUE;
		tos -= n;
		push(LVALUE);
		return;
	}

	// see if the rvalue matches

	if (!istensor(RVALUE))
		stop("error in indexed assign");

	if (ndim - m != RVALUE->u.tensor->ndim)
		stop("error in indexed assign");

	for (i = 0; i < RVALUE->u.tensor->ndim; i++)
		if (LVALUE->u.tensor->dim[m + i] != RVALUE->u.tensor->dim[i])
			stop("error in indexed assign");

	// copy rvalue

	for (i = 0; i < RVALUE->u.tensor->nelem; i++)
		LVALUE->u.tensor->elem[k + i] = RVALUE->u.tensor->elem[i];

	tos -= n;

	push(LVALUE);
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

#define F p3 // F points to the function name
#define A p4 // A points to the argument list
#define B p5 // B points to the function body

void
setq_userfunc(void)
{
	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!issymbol(F))
		stop("function name?");

	set_binding_and_arglist(F, B, A);
}
