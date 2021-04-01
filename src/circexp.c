#include "defs.h"

void
eval_circexp(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	circexp();
	expanding = t;
}

void
circexp(void)
{
	circexp_subst();
	eval();
}

void
circexp_subst(void)
{
	save();
	circexp_subst_nib();
	restore();
}

void
circexp_subst_nib(void)
{
	int i, h, n;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
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

// tan(z) = (i - i * exp(2*i*z)) / (exp(2*i*z) + 1)

void
eval_exptan(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	exptan();
	expanding = t;
}

void
exptan(void)
{
	save();

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

	restore();
}

void
eval_expcosh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	expcosh();
	expanding = t;
}

void
expcosh(void)
{
	save();
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	add();
	push_rational(1, 2);
	multiply();
	restore();
}

void
eval_expsinh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	expsinh();
	expanding = t;
}

void
expsinh(void)
{
	save();
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	subtract();
	push_rational(1, 2);
	multiply();
	restore();
}

void
eval_exptanh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	exptanh();
	expanding = t;
}

void
exptanh(void)
{
	save();
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
	restore();
}
