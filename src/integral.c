#include "defs.h"

void
eval_integral(void)
{
	int i, n;

	// evaluate 1st arg to get function F

	p1 = cdr(p1);
	push(car(p1));
	eval();

	// check for single arg

	if (cdr(p1) == symbol(NIL)) {
		guess();
		integral();
		return;
	}

	p1 = cdr(p1);

	while (iscons(p1)) {

		// next arg should be a symbol

		push(car(p1)); // have to eval in case of $METAX
		eval();
		p3 = pop();

		if (!issymbol(p3))
			stop("integral: symbol expected");

		p1 = cdr(p1);

		// if next arg is a number then use it

		n = 1;

		if (isnum(car(p1))) {
			push(car(p1));
			n = pop_integer();
			if (n < 1)
				stop("nth integral: check n");
			p1 = cdr(p1);
		}

		for (i = 0; i < n; i++) {
			push(p3);
			integral();
		}
	}
}

void
integral(void)
{
	save();

	p2 = pop(); // x
	p1 = pop(); // f(x)

	if (car(p1) == symbol(ADD))
		integral_of_sum();
	else if (car(p1) == symbol(MULTIPLY))
		integral_of_product();
	else
		integral_of_form();

	restore();
}

void
integral_of_sum(void)
{
	int h = tos;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		integral();
		p1 = cdr(p1);
	}

	add_terms(tos - h);
}

void
integral_of_product(void)
{
	push(p1); // f(x)
	push(p2); // x
	partition(); // pushes const part, then pushes var part
	p1 = pop(); // pop var part
	integral_of_form();
	multiply(); // multiply by const part
}

void
integral_of_form(void)
{
	push(p1); // f(x)
	push(p2); // x
	transform();
}

/*	F	input expression

	X	free variable, i.e. F of X

	A	template expression

	B	result expression

	C	list of conditional expressions
*/

// p1 and p2 are tmps

#undef F
#undef X
#undef A
#undef B
#undef C

#define F p3
#define X p4
#define A p5
#define B p6
#define C p7

void
transform(void)
{
	int h;
	char **s;

	save();

	X = pop();
	F = pop();

	// save symbol context in case eval(B) below calls transform

	save_binding(symbol(METAA));
	save_binding(symbol(METAB));
	save_binding(symbol(METAX));

	set_binding(symbol(METAX), X);

	// put constants in F(X) on the stack

	h = tos;
	push(one);
	push(F);
	push(X);
	transform_terms(); // collect coefficients of x, x^2, etc.
	push(X);
	decomp_nib();

	s = itab;

	for (;;) {

		if (*s == NULL)
			stop("integral could not find a solution");

		scan(*s++, 1);

		p1 = pop();

		A = cadr(p1);
		B = caddr(p1);
		C = cdddr(p1);

		if (f_equals_a(h))
			break;
	}

	tos = h; // pop all

	push(B);
	eval();

	restore_binding(symbol(METAX));
	restore_binding(symbol(METAB));
	restore_binding(symbol(METAA));

	restore();
}

// search for a METAA and METAB such that F = A

int
f_equals_a(int h)
{
	int i, j;
	for (i = h; i < tos; i++) {
		set_binding(symbol(METAA), stack[i]);
		for (j = h; j < tos; j++) {
			set_binding(symbol(METAB), stack[j]);
			p1 = C;				// are conditions ok?
			while (iscons(p1)) {
				push(car(p1));
				eval();
				p2 = pop();
				if (iszero(p2))
					break;
				p1 = cdr(p1);
			}
			if (iscons(p1))			// no, try next j
				continue;
			push(F);			// F = A?
			push(A);
			eval();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}
	return 0;					// no
}

// returns constant expresions on the stack

void
decomp_nib(void)
{
	save();

	p2 = pop();
	p1 = pop();

	// is the entire expression constant?

	if (find(p1, p2) == 0) {
		push(p1);
		restore();
		return;
	}

	// sum?

	if (isadd(p1)) {
		decomp_sum();
		restore();
		return;
	}

	// product?

	if (car(p1) == symbol(MULTIPLY)) {
		decomp_product();
		restore();
		return;
	}

	// naive decomp if not sum or product

	p3 = cdr(p1);
	while (iscons(p3)) {
		push(car(p3));
		push(p2);
		decomp_nib();
		p3 = cdr(p3);
	}

	restore();
}

void
decomp_sum(void)
{
	int h;

	// decomp terms involving x

	p3 = cdr(p1);

	while (iscons(p3)) {
		if (find(car(p3), p2)) {
			push(car(p3));
			push(p2);
			decomp_nib();
		}
		p3 = cdr(p3);
	}

	// add together all constant terms

	h = tos;

	p3 = cdr(p1);

	while (iscons(p3)) {
		if (find(car(p3), p2) == 0)
			push(car(p3));
		p3 = cdr(p3);
	}

	if (tos - h) {
		add_terms(tos - h);
		p3 = pop();
		push(p3);
		push(p3);
		negate();	// need both +a, -a for some integrals
	}
}

void
decomp_product(void)
{
	int h;

	// decomp factors involving x

	p3 = cdr(p1);

	while (iscons(p3)) {
		if (find(car(p3), p2)) {
			push(car(p3));
			push(p2);
			decomp_nib();
		}
		p3 = cdr(p3);
	}

	// multiply together all constant factors

	h = tos;

	p3 = cdr(p1);

	while (iscons(p3)) {
		if (find(car(p3), p2) == 0)
			push(car(p3));
		p3 = cdr(p3);
	}

	if (tos - h)
		multiply_factors(tos - h);
}

// for example,  a x + b x  ->  (a + b) x

void
transform_terms(void)
{
	save();
	transform_terms_nib();
	restore();
}

void
transform_terms_nib(void)
{
	int h, i, j, n;
	struct atom **s;

	p2 = pop(); // x
	p1 = pop(); // expr

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	h = tos;
	s = stack + tos;

	// depth first

	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		transform_terms();
		p1 = cdr(p1);
	}
	list(tos - h);
	p1 = pop();

	if (car(p1) != symbol(ADD)) {
		push(p1);
		return;
	}

	// partition terms

	p1 = cdr(p1);

	while (iscons(p1)) {
		p3 = car(p1);
		if (car(p3) == symbol(MULTIPLY)) {
			push(p3);
			push(p2);
			partition(); // pushes const part then pushes var part
		} else if (find(p3, p2)) {
			push(one); // const part
			push(p3); // var part
		} else {
			push(p3); // const part
			push(one); // var part
		}
		p1 = cdr(p1);
	}

	// sort by var part

	n = tos - h;

	qsort(s, n / 2, 2 * sizeof (struct atom *), transform_sort_func);

	// combine const parts of matching var parts

	for (i = 0; i < n - 2; i += 2) {
		if (equal(s[i + 1], s[i + 3])) {
			push(s[0]);
			push(s[2]);
			add();
			s[0] = pop();
			for (j = i + 2; j < n; j++)
				s[j] = s[j + 2];
			n -= 2;
			tos -= 2;
			i -= 2; // use the same index again
		}
	}

	// combine all the parts

	n = tos - h;

	expanding = 0;

	for (i = 0; i < n; i += 2) {
		push(s[i]); // const part
		push(s[i + 1]); // var part
		multiply();
		s[i / 2] = pop();
	}

	tos -= n / 2;

	add_terms(tos - h);

	expanding = 1;
}

int
transform_sort_func(const void *q1, const void *q2)
{
	return cmp_terms(((struct atom **) q1)[1], ((struct atom **) q2)[1]);
}
