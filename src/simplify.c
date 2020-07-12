#include "defs.h"

void
eval_simplify(void)
{
	push(cadr(p1));
	eval();
	simplify();
}

void
simplify(void)
{
	save();
	simplify_nib();
	restore();
}

void
simplify_nib(void)
{
	int h;

	p1 = pop();

	if (istensor(p1)) {
		simplify_tensor();
		return;
	}

	if (find(p1, symbol(FACTORIAL))) {
		push(p1);
		simplify_factorial();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		// simplify each term
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			simplify_expr();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		p1 = pop();
		if (car(p1) == symbol(ADD)) {
			push(p1);
			simplify_expr(); // try rationalizing
			p1 = pop();
		}
		push(p1);
		if (find(p1, symbol(SIN)) || find(p1, symbol(COS)) || find(p1, symbol(TAN))
		|| find(p1, symbol(SINH)) || find(p1, symbol(COSH)) || find(p1, symbol(TANH)))
			simplify_trig();
		return;
	}

	// p1 is a term (factor or product of factors)

	push(p1);
	simplify_expr();
	p1 = pop();
	push(p1);

	if (find(p1, symbol(SIN)) || find(p1, symbol(COS)) || find(p1, symbol(TAN))
	|| find(p1, symbol(SINH)) || find(p1, symbol(COSH)) || find(p1, symbol(TANH)))
		simplify_trig();
}

void
simplify_tensor(void)
{
	int i, n;
	push(p1);
	copy_tensor();
	p1 = pop();
	n = p1->u.tensor->nelem;
	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		simplify();
		p1->u.tensor->elem[i] = pop();
	}
	push(p1);
}

void
simplify_factorial(void)
{
	save();

	p1 = pop();

	push(p1);
	simfac();
	p2 = pop();

	push(p1);
	rationalize(); // try rationalizing first
	simfac();
	p3 = pop();

	if (weight(p2) < weight(p3))
		push(p2);
	else
		push(p3);

	restore();
}

//Example 1:
//
//? -3*A*x/(A-B)+3*B*x/(A-B)
//   3 A x     3 B x
//- ------- + -------
//   A - B     A - B
//? simplify
//-3 x
//
//Example 2:
//
//? -y/(x^2*(y^2/x^2+1))
//         y
//- ---------------
//         2
//    2   y
//   x  (---- + 1)
//         2
//        x
//? simplify
//      y
//- ---------
//    2    2
//   x  + y
//
//Example 3:
//
//? 1/(x*(y^2/x^2+1))
//      1
//--------------
//      2
//     y
// x (---- + 1)
//      2
//     x
//? simplify
//    x
//---------
//  2    2
// x  + y

void
simplify_expr(void)
{
	save();
	simplify_expr_nib();
	restore();
}

#undef NUM
#undef DEN
#undef R
#undef T

#define NUM p2
#define DEN p3
#define R p4
#define T p5

void
simplify_expr_nib(void)
{
	p1 = pop();

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
	eval(); // to expand denominator
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
			eval(); // to expand denominator
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

	if (car(NUM) != symbol(ADD) || car(DEN) != symbol(ADD) || length(NUM) != length(DEN)) {
		// no, but NUM over DEN might be simpler than p1
		push(NUM);
		push(DEN);
		divide();
		T = pop();
		if (weight(T) < weight(p1))
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

	if (equaln(T, 0))
		p1 = R;

	push(p1);
}

void
simplify_trig(void)
{
	save();
	simplify_trig_nib();
	restore();
}

void
simplify_trig_nib(void)
{
	p1 = pop();

	if (car(p1) == symbol(ADD) && find(p1, symbol(SIN))) {
		push(p1);
		simplify_sin();
		p1 = pop();
	}

	if (car(p1) == symbol(ADD) && find(p1, symbol(COS))) {
		push(p1);
		simplify_cos();
		p1 = pop();
	}

	push(p1);
	circexp();
	simplify_expr();
	p2 = pop();

	if (weight(p2) < weight(p1))
		p1 = p2;

	push(p1);
}

void
simplify_sin(void)
{
	save();
	p1 = pop();
	push(p1);
	replace_sin();
	eval();
	p2 = pop();
	if (car(p2) != symbol(ADD) || length(p2) < length(p1))
		p1 = p2;
	push(p1);
	restore();
}

void
simplify_cos(void)
{
	save();
	p1 = pop();
	push(p1);
	replace_cos();
	eval();
	p2 = pop();
	if (car(p2) != symbol(ADD) || length(p2) < length(p1))
		p1 = p2;
	push(p1);
	restore();
}

#undef BASE
#undef EXPO

#define BASE p2
#define EXPO p3

// sin(x)^(2*n) is replaced with (1 - cos(x)^2)^n

void
replace_sin(void)
{
	save();
	replace_sin_nib();
	restore();
}

void
replace_sin_nib(void)
{
	int h;
	p1 = pop();
	if (!iscons(p1)) {
		push(p1);
		return;
	}
	if (car(p1) == symbol(POWER) && caadr(p1) == symbol(SIN) && iseveninteger(caddr(p1))) {
		BASE = cadr(p1);
		EXPO = caddr(p1);
		push_integer(1);
		push(cadr(BASE));
		scos();
		push_integer(2);
		power();
		subtract();
		push(EXPO);
		push_rational(1, 2);
		multiply();
		power();
	} else {
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			replace_sin();
			p1 = cdr(p1);
		}
		list(tos - h);
	}
}

// cos(x)^(2*n) is replaced with (1 - sin(x)^2)^n

void
replace_cos(void)
{
	save();
	replace_cos_nib();
	restore();
}

void
replace_cos_nib(void)
{
	int h;
	p1 = pop();
	if (!iscons(p1)) {
		push(p1);
		return;
	}
	if (car(p1) == symbol(POWER) && caadr(p1) == symbol(COS) && iseveninteger(caddr(p1))) {
		BASE = cadr(p1);
		EXPO = caddr(p1);
		push_integer(1);
		push(cadr(BASE));
		ssin();
		push_integer(2);
		power();
		subtract();
		push(EXPO);
		push_rational(1, 2);
		multiply();
		power();
	} else {
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			replace_cos();
			p1 = cdr(p1);
		}
		list(tos - h);
	}
}
