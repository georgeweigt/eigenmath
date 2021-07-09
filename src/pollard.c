// Factor using the Pollard rho method

#include "defs.h"

uint32_t *global_n;

void
factor_number(void)
{
	save();

	p1 = pop();

	// 0 or 1?

	if (equaln(p1, 0) || equaln(p1, 1) || equaln(p1, -1)) {
		push(p1);
		restore();
		return;
	}

	global_n = mcopy(p1->u.q.a);

	if (p1->sign == MMINUS)
		push_integer(-1);

	factor_a();

	restore();
}

// factor using table look-up, then switch to rho method if necessary

// From TAOCP Vol. 2 by Knuth, p. 380 (Algorithm A)

void
factor_a(void)
{
	int k;

	for (k = 0; k < 10000; k++) {

		try_kth_prime(k);

		// if n is 1 then we're done

		if (MEQUAL(global_n, 1)) {
			mfree(global_n);
			return;
		}
	}

	factor_b();
}

void
try_kth_prime(int k)
{
	int count;
	uint32_t *d, *q, *r;

	d = mint(primetab[k]);

	count = 0;

	while (1) {

		// if n is 1 then we're done

		if (MEQUAL(global_n, 1)) {
			if (count)
				push_factor(d, count);
			else
				mfree(d);
			return;
		}

		q = mdiv(global_n, d);
		r = mmod(global_n, d);

		// continue looping while remainder is zero

		if (MZERO(r)) {
			count++;
			mfree(r);
			mfree(global_n);
			global_n = q;
		} else {
			mfree(r);
			break;
		}
	}

	if (count)
		push_factor(d, count);

	// q = n/d, hence if q < d then n < d^2 so n is prime

	if (mcmp(q, d) == -1) {
		push_factor(global_n, 1);
		global_n = mint(1);
	}

	if (count == 0)
		mfree(d);

	mfree(q);
}

// From TAOCP Vol. 2 by Knuth, p. 385 (Algorithm B)

int
factor_b(void)
{
	int k, l;
	uint32_t *g, *k1, *t, *x, *xprime;

	k1 = mint(1);
	x = mint(5);
	xprime = mint(2);

	k = 1;
	l = 1;

	while (1) {

		if (mprime(global_n)) {
			push_factor(global_n, 1);
			mfree(k1);
			mfree(x);
			mfree(xprime);
			return 0;
		}

		while (1) {

			if (interrupt) {
				mfree(k1);
				mfree(global_n);
				mfree(x);
				mfree(xprime);
				kaput("interrupt");
			}

			// g = gcd(x' - x, n)

			if (mcmp(xprime, x) < 0)
				t = msub(x, xprime);
			else
				t = msub(xprime, x);
			g = mgcd(t, global_n);
			mfree(t);

			if (MEQUAL(g, 1)) {
				mfree(g);
				if (--k == 0) {
					mfree(xprime);
					xprime = mcopy(x);
					l *= 2;
					k = l;
				}

				// x = (x ^ 2 + 1) mod n

				t = mmul(x, x);
				mfree(x);
				x = madd(t, k1);
				mfree(t);
				t = mmod(x, global_n);
				mfree(x);
				x = t;

				continue;
			}

			push_factor(g, 1);

			if (mcmp(g, global_n) == 0) {
				mfree(k1);
				mfree(global_n);
				mfree(x);
				mfree(xprime);
				return -1;
			}

			// n = n / g

			t = mdiv(global_n, g);
			mfree(global_n);
			global_n = t;

			// x = x mod n

			t = mmod(x, global_n);
			mfree(x);
			x = t;

			// xprime = xprime mod n

			t = mmod(xprime, global_n);
			mfree(xprime);
			xprime = t;

			break;
		}
	}
}

void
push_factor(uint32_t *d, int count)
{
	if (count < 2)
		push_rational_number(MPLUS, d, mint(1));
	else {
		push_symbol(POWER);
		push_rational_number(MPLUS, d, mint(1));
		push_rational_number(MPLUS, mint(count), mint(1));
		list(3);
	}
}
