#define MAXCOEFFS 100
#define DELTA 1.0e-6
#define EPSILON 1.0e-9
#define YABS(z) sqrt((z).r * (z).r + (z).i * (z).i)
#define RANDOM (4.0 * (double) rand() / (double) RAND_MAX - 2.0)

struct {
	double r, i;
} a, b, x, y, fa, fb, dx, df, c[MAXCOEFFS];

void
eval_nroots(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	nroots();
}

void
nroots(void)
{
	int h, i, k, n;
	struct atom *P, *X, *RE, *IM, *V;

	X = pop();
	P = pop();

	h = tos;

	factorpoly_coeffs(P, X); // put coeffs on stack

	n = tos - h;

	if (n < 2 || n > MAXCOEFFS)
		stop("nroots");

	// convert the coefficients to real and imaginary doubles

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		real();
		floatfunc();
		RE = pop();
		push(stack[h + i]);
		imag();
		floatfunc();
		IM = pop();
		if (!isdouble(RE) || !isdouble(IM))
			stop("nroots: coefficients?");
		c[i].r = RE->u.d;
		c[i].i = IM->u.d;
	}

	tos = h; // pop all

	// n is the number of coefficients, n = deg(p) + 1

	monic(n);

	for (k = n; k > 1; k--) {
		findroot(k);
		if (fabs(a.r) < DELTA)
			a.r = 0.0;
		if (fabs(a.i) < DELTA)
			a.i = 0.0;
		push_double(a.r);
		push_double(a.i);
		push(imaginaryunit);
		multiply();
		add();
		nroots_divpoly(k);
	}

	// now make n equal to the number of roots

	n = tos - h;

	if (n > 1) {
		sort(n);
		V = alloc_vector(n);
		for (i = 0; i < n; i++)
			V->u.tensor->elem[i] = stack[h + i];
		tos = h; // pop all
		push(V);
	}
}

// divide the polynomial by its leading coefficient

void
monic(int n)
{
	int k;
	double t;
	y = c[n - 1];
	t = y.r * y.r + y.i * y.i;
	for (k = 0; k < n - 1; k++) {
		c[k].r = (c[k].r * y.r + c[k].i * y.i) / t;
		c[k].i = (c[k].i * y.r - c[k].r * y.i) / t;
	}
	c[n - 1].r = 1.0;
	c[n - 1].i = 0.0;
}

// uses the secant method

void
findroot(int n)
{
	int j, k;
	double t;

	if (YABS(c[0]) < DELTA) {
		a.r = 0.0;
		a.i = 0.0;
		return;
	}

	for (j = 0; j < 100; j++) {

		a.r = RANDOM;
		a.i = RANDOM;

		compute_fa(n);

		b = a;
		fb = fa;

		a.r = RANDOM;
		a.i = RANDOM;

		for (k = 0; k < 1000; k++) {

			compute_fa(n);

			if (YABS(fa) < EPSILON)
				return;

			if (YABS(fa) < YABS(fb)) {
				x = a;
				a = b;
				b = x;
				x = fa;
				fa = fb;
				fb = x;
			}

			// dx = b - a

			dx.r = b.r - a.r;
			dx.i = b.i - a.i;

			// df = fb - fa

			df.r = fb.r - fa.r;
			df.i = fb.i - fa.i;

			// y = dx / df

			t = df.r * df.r + df.i * df.i;

			if (t == 0.0)
				break;

			y.r = (dx.r * df.r + dx.i * df.i) / t;
			y.i = (dx.i * df.r - dx.r * df.i) / t;

			// a = b - y * fb

			a.r = b.r - (y.r * fb.r - y.i * fb.i);
			a.i = b.i - (y.r * fb.i + y.i * fb.r);
		}
	}

	stop("nroots: convergence error");
}

void
compute_fa(int n)
{
	int k;
	double t;

	// x = a

	x.r = a.r;
	x.i = a.i;

	// fa = c0 + c1 * x

	fa.r = c[0].r + c[1].r * x.r - c[1].i * x.i;
	fa.i = c[0].i + c[1].r * x.i + c[1].i * x.r;

	for (k = 2; k < n; k++) {

		// x = a * x

		t = a.r * x.r - a.i * x.i;
		x.i = a.r * x.i + a.i * x.r;
		x.r = t;

		// fa += c[k] * x

		fa.r += c[k].r * x.r - c[k].i * x.i;
		fa.i += c[k].r * x.i + c[k].i * x.r;
	}
}

// divide the polynomial by x - a

void
nroots_divpoly(int n)
{
	int k;
	for (k = n - 1; k > 0; k--) {
		c[k - 1].r += c[k].r * a.r - c[k].i * a.i;
		c[k - 1].i += c[k].i * a.r + c[k].r * a.i;
	}
	if (YABS(c[0]) > DELTA)
		stop("nroots: residual error");
	for (k = 0; k < n - 1; k++) {
		c[k].r = c[k + 1].r;
		c[k].i = c[k + 1].i;
	}
}
