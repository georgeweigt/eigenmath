#include "defs.h"

#undef PSI
#undef OPCODE
#undef PHASE
#undef T
#undef N
#undef KET0
#undef KET1

#define PSI p3
#define OPCODE p4
#define PHASE p5
#define T p6

#define N PSI->u.tensor->nelem
#define KET0 PSI->u.tensor->elem[i ^ n]
#define KET1 PSI->u.tensor->elem[i]

#define POWEROF2(x) ((x & (x - 1)) == 0)

void
eval_rotate(void)
{
	int c, m, n, t;

	t = expanding;
	expanding = 1;

	push(cadr(p1));
	eval();
	PSI = pop();

	if (!istensor(PSI) || PSI->u.tensor->ndim > 1 || PSI->u.tensor->nelem > 32768 || !POWEROF2(PSI->u.tensor->nelem))
		stop("rotate error 1 first argument is not a vector or dimension error");

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (!iscons(cdr(p1)))
			stop("rotate error 2 unexpected end of argument list");

		OPCODE = car(p1);
		push(cadr(p1));
		eval();
		n = pop_integer();
		c = n; // control bit

		if (n > 14 || (1 << n) >= PSI->u.tensor->nelem)
			stop("rotate error 3 qubit number format or range");

		p1 = cddr(p1);

		if (OPCODE == symbol(C_UPPER)) {
			if (!iscons(cdr(p1)))
				stop("rotate error 2 unexpected end of argument list");
			OPCODE = car(p1);
			push(cadr(p1));
			eval();
			n = pop_integer();
			if (n > 14 || (1 << n) >= PSI->u.tensor->nelem)
				stop("rotate error 3 qubit number format or range");
			p1 = cddr(p1);
		}

		if (OPCODE == symbol(H_UPPER)) {
			rotate_h(c, n);
			continue;
		}

		if (OPCODE == symbol(P_UPPER)) {
			if (!iscons(p1))
				stop("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			eval();
			push(imaginaryunit);
			multiply();
			expfunc();
			PHASE = pop();
			rotate_p(c, n);
			continue;
		}

		if (OPCODE == symbol(Q_UPPER)) {
			rotate_q(n);
			continue;
		}

		if (OPCODE == symbol(V_UPPER)) {
			rotate_v(n);
			continue;
		}

		if (OPCODE == symbol(W_UPPER)) {
			m = n;
			if (!iscons(p1))
				stop("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			eval();
			n = pop_integer();
			if (n > 14 || (1 << n) >= PSI->u.tensor->nelem)
				stop("rotate error 3 qubit number format or range");
			rotate_w(c, m, n);
			continue;
		}

		if (OPCODE == symbol(X_UPPER)) {
			rotate_x(c, n);
			continue;
		}

		if (OPCODE == symbol(Y_UPPER)) {
			rotate_y(c, n);
			continue;
		}

		if (OPCODE == symbol(Z_UPPER)) {
			rotate_z(c, n);
			continue;
		}

		stop("rotate error 4 unknown rotation code");
	}

	push(PSI);

	expanding = t;
}

// hadamard

void
rotate_h(int c, int n)
{
	int i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & n)) {
			push(KET0);
			push(KET1);
			add();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			push(KET0);
			push(KET1);
			subtract();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			KET1 = pop();
			KET0 = pop();
		}
}

// phase

void
rotate_p(int c, int n)
{
	int i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & n)) {
			push(KET1);
			push(PHASE);
			multiply();
			KET1 = pop();
		}
}

// swap

void
rotate_w(int c, int m, int n)
{
	int i;
	c = 1 << c;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & m) && !(i & n)) {
			push(PSI->u.tensor->elem[i]);
			push(PSI->u.tensor->elem[i ^ m ^ n]);
			PSI->u.tensor->elem[i] = pop();
			PSI->u.tensor->elem[i ^ m ^ n] = pop();
		}
}

void
rotate_x(int c, int n)
{
	int i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & n)) {
			push(KET0);
			push(KET1);
			KET0 = pop();
			KET1 = pop();
		}
}

void
rotate_y(int c, int n)
{
	int i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & n)) {
			push(imaginaryunit);
			negate();
			push(KET0);
			multiply();
			push(imaginaryunit);
			push(KET1);
			multiply();
			KET0 = pop();
			KET1 = pop();
		}
}

void
rotate_z(int c, int n)
{
	int i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & c) && (i & n)) {
			push(KET1);
			negate();
			KET1 = pop();
		}
}

// quantum fourier transform

void
rotate_q(int n)
{
	int i, j;
	for (i = n; i >= 0; i--) {
		rotate_h(i, i);
		for (j = 0; j < i; j++) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			eval();
			multiply_factors(3);
			expfunc();
			PHASE = pop();
			rotate_p(j, i);
		}
	}
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(i, i, n - i);
}

// inverse qft

void
rotate_v(int n)
{
	int i, j;
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(i, i, n - i);
	for (i = 0; i <= n; i++) {
		for (j = i - 1; j >= 0; j--) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			eval();
			multiply_factors(3);
			negate();
			expfunc();
			PHASE = pop();
			rotate_p(j, i);
		}
		rotate_h(i, i);
	}
}
