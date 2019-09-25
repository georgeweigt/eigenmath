#include "defs.h"

// small integer tests

void
test_madd(void)
{
	int i, j, m;
	printf("test madd\n");
	m = bignum_count;
	for (i = 0; i <= 100; i++)
		for (j = 0; j <= 100; j++)
			test_maddf(i, j, i + j);
	if (m != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

void
test_maddf(int na, int nb, int nc)
{
	uint32_t *a, *b, *c, *d;

	a = mint(na);
	b = mint(nb);
	c = mint(nc);

	d = madd(a, b);

	if (mcmp(c, d) == 0) {
		mfree(a);
		mfree(b);
		mfree(c);
		mfree(d);
		return;
	}

	printf("%d %d %d %d\n", na, nb, nc, d[0]);
	exit(1);
}

void
test_msub(void)
{
	int i, j, m;
	printf("test msub\n");
	m = bignum_count;
	for (i = 0; i <= 100; i++)
		for (j = 0; j <= i; j++)
			test_msubf(i, j, i - j);
	if (m != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

void
test_msubf(int na, int nb, int nc)
{
	uint32_t *a, *b, *c, *d;

	a = mint(na);
	b = mint(nb);
	c = mint(nc);

	d = msub(a, b);

	if (mcmp(c, d) == 0) {
		mfree(a);
		mfree(b);
		mfree(c);
		mfree(d);
		return;
	}

	printf("%d %d %d %d\n", na, nb, nc, d[0]);
	exit(1);
}

void
test_mcmp(void)
{
	int i, j, k;
	uint32_t *x, *y;
	printf("testing mcmp\n");
	for (i = 0; i < 1000; i++) {
		x = mint(i);
		for (j = 0; j < 1000; j++) {
			y = mint(j);
			k = mcmp(x, y);
			if (i == j && k != 0) {
				printf("failed\n");
				exit(1);
			}
			if (i < j && k != -1) {
				printf("failed\n");
				exit(1);
			}
			if (i > j && k != 1) {
				printf("failed\n");
				exit(1);
			}
			mfree(y);
		}
		mfree(x);
	}
	printf("ok\n");
}

void
test_mgcd(void)
{
	int i, j, n;
	uint32_t *a, *b, *c, *d;
	printf("testing mgcd\n");
	n = bignum_count;
	for (i = 1; i < 100; i++) {
		a = mint(i);
		for (j = 1; j < 100; j++) {
			b = mint(j);
			c = mgcd(a, b);
			d = egcd(a, b);
			if (mcmp(c, d) != 0) {
				printf("failed a=%u b=%u c=%u d=%u\n", a[0], b[0], c[0], d[0]);
				exit(1);
			}
			mfree(b);
			mfree(c);
			mfree(d);
		}
		mfree(a);
	}
	if (n != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

// Euclid's algorithm

uint32_t *
egcd(uint32_t *a, uint32_t *b)
{
	uint32_t *c;
	if (MZERO(b))
		stop("divide by zero");
	b = mcopy(b);
	if (MZERO(a))
		return b;
	a = mcopy(a);
	while (!MZERO(b)) {
		c = mmod(a, b);
		mfree(a);
		a = b;
		b = c;
	}
	mfree(b);
	return a;
}

void
test_mmodpow(void)
{
	int mem;
	int x, n, m;
	uint32_t *xx, *nn, *mm, *y;
	mem = bignum_count;
	for (x = 1; x < 100; x++) {
		xx = mint(x);
		for (n = 1; n < 100; n++) {
			nn = mint(n);
			for (m = 1; m < 10; m++) {
				mm = mint(m);
				y = mmodpow(xx, nn, mm);
				mfree(y);
				mfree(mm);
			}
			mfree(nn);
		}
		mfree(xx);
	}
	if (mem != bignum_count) {
		printf("mmodpow memory leak %d %d\n", mem, bignum_count);
		exit(1);
	}
}

// small integer tests

void
test_mmul(void)
{
	int i, j, m;
	printf("test mmul\n");
	m = bignum_count;
	for (i = 0; i <= 100; i++)
		for (j = 0; j <= 100; j++)
			test_mmulf(i, j, i * j);
	if (m != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

void
test_mmulf(int na, int nb, int nc)
{
	uint32_t *a, *b, *c, *d;

	a = mint(na);
	b = mint(nb);
	c = mint(nc);

	d = mmul(a, b);

	if (mcmp(c, d) == 0) {
		mfree(a);
		mfree(b);
		mfree(c);
		mfree(d);
		return;
	}

	printf("%d %d %d %d\n", na, nb, nc, d[0]);
	exit(1);
}

void
test_mdiv(void)
{
	int i, j, m;
	printf("test mdiv\n");
	m = bignum_count;
	for (i = 0; i <= 100; i++)
		for (j = 0; j <= 100; j++)
			if (j)
				test_mdivf(i, j, i / j);
	if (m != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

void
test_mdivf(int na, int nb, int nc)
{
	uint32_t *a, *b, *c, *d;

	a = mint(na);
	b = mint(nb);
	c = mint(nc);

	d = mdiv(a, b);

	if (mcmp(c, d) == 0) {
		mfree(a);
		mfree(b);
		mfree(c);
		mfree(d);
		return;
	}

	printf("%d %d %d %d\n", na, nb, nc, d[0]);
	exit(1);
}

void
test_mmod(void)
{
	int i, j, m;
	printf("test mmod\n");
	m = bignum_count;
	for (i = 0; i <= 100; i++)
		for (j = 0; j <= 100; j++)
			if (j)
				test_mmodf(i, j, i % j);
	if (m != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}
	printf("ok\n");
}

void
test_mmodf(int na, int nb, int nc)
{
	uint32_t *a, *b, *c, *d;

	a = mint(na);
	b = mint(nb);
	c = mint(nc);

	d = mmod(a, b);

	if (mcmp(c, d) == 0) {
		mfree(a);
		mfree(b);
		mfree(c);
		mfree(d);
		return;
	}

	printf("%d %d %d %d\n", na, nb, nc, d[0]);
	exit(1);
}

void
test_mpow(void)
{
	int i, j, mem, x;
	uint32_t *a, *b, *c, *d;

	printf("testing mpow\n");

	mem = bignum_count;

	for (i = 0; i < 10; i++) {
		a = mint(i);
		x = 1;
		for (j = 0; j < 10; j++) {
			b = mint(j);
			c = mpow(a, b);
			d = mint(x);
			if (mcmp(c, d) != 0) {
				printf("failed a=%d b=%d c=%d\n", a[0], b[0], c[0]);
				exit(1);
			}
			mfree(b);
			mfree(c);
			mfree(d);
			x *= i;
		}
		mfree(a);
	}

	if (mem != bignum_count) {
		printf("memory leak\n");
		exit(1);
	}

	printf("ok\n");
}

void
test_mroot(void)
{
	int i, j, mem;
	uint32_t *a, *b, *c, *d;

	printf("testing mroot\n");

	mem = bignum_count;

	// small numbers

	for (i = 0; i < 10; i++) {
		a = mint(i);
		for (j = 1; j < 10; j++) {
			b = mint(j);
			c = mpow(a, b);
			d = mroot(c, b);
			if (d == NULL || mcmp(a, d) != 0) {
				printf("failed a=%d b=%d c=%d\n", a[0], b[0], c[0]);
				exit(1);
			}
			mfree(b);
			mfree(c);
			mfree(d);
		}
		mfree(a);
	}

	a = mint(12345);

	for (i = 1; i < 10; i++) {
		b = mint(i);
		c = mpow(a, b);
		d = mroot(c, b);
		if (d == NULL || mcmp(a, d) != 0) {
			printf("failed\n");
			exit(1);
		}
		mfree(b);
		mfree(c);
		mfree(d);
	}

	mfree(a);

	if (bignum_count != mem) {
		printf("memory leak\n");
		exit(1);
	}

	printf("ok\n");
}

void
test_quickfactor(void)
{
	int base, expo, i, j, h;
	printf("testing quickfactor\n");
	for (i = 2; i < 10001; i++) {
		base = i;
		push_integer(base);
		push_integer(1);
		h = tos;
		j = 0;
		while (base > 1) {
			expo = 0;
			while (base % primetab[j] == 0) {
				base /= primetab[j];
				expo++;
			}
			if (expo) {
				push_integer(primetab[j]);
				push_integer(expo);
//				quickpower();
			}
			j++;
		}
		multiply_factors(tos - h);
		p2 = pop();
		p1 = pop();
		if (!equal(p1, p2)) {
			printf("failed\n");
			print_lisp(p1);
			print_lisp(p2);
			exit(1);
		}
	}
	printf("ok\n");
}

void
test_all(void)
{
	test_madd();
	test_msub();
	test_mcmp();
	test_mgcd();
	test_mmodpow();
	test_mmul();
	test_mdiv();
	test_mmod();
	test_mpow();
	test_mroot();
	test_quickfactor();
}
