/* Bessel J function

	1st arg		x

	2nd arg		n

Recurrence relation

	besselj(x,n) = (2/x) (n-1) besselj(x,n-1) - besselj(x,n-2)

	besselj(x,1/2) = sqrt(2/pi/x) sin(x)

	besselj(x,-1/2) = sqrt(2/pi/x) cos(x)

For negative n, reorder the recurrence relation as

	besselj(x,n-2) = (2/x) (n-1) besselj(x,n-1) - besselj(x,n)

Substitute n+2 for n to obtain

	besselj(x,n) = (2/x) (n+1) besselj(x,n+1) - besselj(x,n+2)

Examples

	besselj(x,3/2) = (1/x) besselj(x,1/2) - besselj(x,-1/2)

	besselj(x,-3/2) = -(1/x) besselj(x,-1/2) - besselj(x,1/2)
*/

#include "defs.h"

void
eval_besselj(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	besselj();
}

void
besselj(void)
{
	save();
	besselj_nib();
	restore();
}

#undef X
#undef N
#undef SIGN

#define X p1
#define N p2
#define SIGN p3

void
besselj_nib(void)
{
	double d;
	int n;

	N = pop();
	X = pop();

	push(N);
	n = pop_integer();

	// numerical result

	if (isdouble(X) && n != ERR) {
		d = jn(n, X->u.d);
		push_double(d);
		return;
	}

	// bessej(0,0) = 1

	if (iszero(X) && iszero(N)) {
		push_integer(1);
		return;
	}

	// besselj(0,n) = 0

	if (iszero(X) && n != ERR) {
		push_integer(0);
		return;
	}

	// half arguments

	if (N->k == RATIONAL && MEQUAL(N->u.q.b, 2)) {

		// n = 1/2

		if (N->sign == MPLUS && MEQUAL(N->u.q.a, 1)) {
			push_integer(2);
			push_symbol(PI);
			divide();
			push(X);
			divide();
			push_rational(1, 2);
			power();
			push(X);
			sine();
			multiply();
			return;
		}

		// n = -1/2

		if (N->sign == MMINUS && MEQUAL(N->u.q.a, 1)) {
			push_integer(2);
			push_symbol(PI);
			divide();
			push(X);
			divide();
			push_rational(1, 2);
			power();
			push(X);
			cosine();
			multiply();
			return;
		}

		// besselj(x,n) = (2/x) (n-sgn(n)) besselj(x,n-sgn(n)) - besselj(x,n-2*sgn(n))

		if (N->sign == MPLUS)
			push_integer(1);
		else
			push_integer(-1);

		SIGN = pop();

		push_integer(2);
		push(X);
		divide();
		push(N);
		push(SIGN);
		subtract();
		multiply();
		push(X);
		push(N);
		push(SIGN);
		subtract();
		besselj();
		multiply();
		push(X);
		push(N);
		push_integer(2);
		push(SIGN);
		multiply();
		subtract();
		besselj();
		subtract();

		return;
	}

	push_symbol(BESSELJ);
	push(X);
	push(N);
	list(3);
}
