#include "defs.h"

void
eval_rationalize(void)
{
	push(cadr(p1));
	eval();
	rationalize();
}

void
rationalize(void)
{
	int t = expanding;
	expanding = 0;

	save();

	p1 = pop();

	if (istensor(p1))
		rationalize_tensor();
	else
		rationalize_scalar();

	restore();

	expanding = t;
}

#undef T
#undef NUM
#undef DEN
#undef DIV

#define T p3
#define NUM p4
#define DEN p5
#define DIV p6

void
rationalize_tensor(void)
{
	int i, n;

	push(p1);
	copy_tensor();
	T = pop();

	n = T->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		p1 = T->u.tensor->elem[i];
		rationalize_scalar();
		T->u.tensor->elem[i] = pop();
	}

	push(T);
}

void
rationalize_scalar(void)
{
	NUM = p1;
	DEN = one;

	move_divisors_num_to_den();
	move_divisors_den_to_num();

	push(NUM);
	expand_expr();
	push(DEN);
	divide();
}

void
move_divisors_num_to_den(void)
{
	int h;

	while (get_divisor(NUM)) {

		if (car(NUM) == symbol(ADD)) {
			p1 = cdr(NUM);
			h = tos;
			while (iscons(p1)) {
				push(car(p1));
				push(DIV);
				multiply();
				p1 = cdr(p1);
			}
			add_terms(tos - h);
		} else {
			push(NUM);
			push(DIV);
			multiply();
		}

		NUM = pop();

		push(DEN);
		push(DIV);
		multiply();
		DEN = pop();
	}
}

void
move_divisors_den_to_num(void)
{
	int h;

	while (get_divisor(DEN)) {

		push(NUM);
		push(DIV);
		multiply();
		NUM = pop();

		if (car(DEN) == symbol(ADD)) {
			p1 = cdr(DEN);
			h = tos;
			while (iscons(p1)) {
				push(car(p1));
				push(DIV);
				multiply();
				p1 = cdr(p1);
			}
			add_terms(tos - h);
		} else {
			push(DEN);
			push(DIV);
			multiply();
		}

		DEN = pop();
	}
}

int
get_divisor(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (get_divisor_from_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return get_divisor_from_term(p);
}

int
get_divisor_from_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (get_divisor_from_factor(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return get_divisor_from_factor(p);
}

int
get_divisor_from_factor(struct atom *p)
{
	if (isfraction(p)) {
		push_rational_number(MPLUS, mcopy(p->u.q.b), mint(1));
		DIV = pop();
		return 1;
	}

	if (car(p) == symbol(POWER) && isnegativeterm(caddr(p))) {
		push(p);
		reciprocate();
		DIV = pop();
		return 1;
	}

	return 0;
}
