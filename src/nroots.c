#define DELTA 1e-6
#define EPSILON 1e-9

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
	int h, i, n;
	struct atom *A, *P, *X, *RE, *IM;
	double ar, ai, mag;
	static double *cr, *ci;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	n = tos - h;

	if (n == 1) {
		stack[h] = symbol(NIL); // P is just a constant, no roots
		return;
	}

	if (cr)
		free(cr);
	if (ci)
		free(ci);

	cr = malloc(n * sizeof (double));
	ci = malloc(n * sizeof (double));

	if (cr == NULL || ci == NULL)
		exit(1);

	// convert coeffs to doubles

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
			stop("nroots: coeffs");

		cr[i] = RE->u.d;
		ci[i] = IM->u.d;
	}

	tos = h; // pop all

	while (n > 1) {

		nfindroot(cr, ci, n, &ar, &ai);

		mag = zabs(ar, ai);

		if (fabs(ar / mag) < DELTA)
			ar = 0.0;

		if (fabs(ai / mag) < DELTA)
			ai = 0.0;

		// push root

		push_double(ar);
		push_double(ai);
		push(imaginaryunit);
		multiply();
		add();

		// divide by X - A

		nreduce(cr, ci, n, ar, ai);

		n--;
	}

	n = tos - h;

	if (n == 1)
		return; // only 1 root

	sort(n);

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[h + i];

	tos = h; // pop all

	push(A);
}

// uses secant method

void
nfindroot(double cr[], double ci[], int n, double *ar, double *ai)
{
	int i, j;
	double d;
	double br, dfr, dxr, far, fbr, xr, yr;
	double bi, dfi, dxi, fai, fbi, xi, yi;

	// if const term is zero then root is zero

	if (cr[0] == 0.0 && ci[0] == 0.0) {
		*ar = 0.0;
		*ai = 0.0;
		return;
	}

	// divide by leading coeff

	xr = cr[n - 1];
	xi = ci[n - 1];

	d = xr * xr + xi * xi;

	for (i = 0; i < n - 1; i++) {
		yr = (cr[i] * xr + ci[i] * xi) / d;
		yi = (ci[i] * xr - cr[i] * xi) / d;
		cr[i] = yr;
		ci[i] = yi;
	}

	cr[n - 1] = 1.0;
	ci[n - 1] = 0.0;

	for (i = 0; i < 100; i++) {

		*ar = urandom();
		*ai = urandom();

		fata(cr, ci, n, *ar, *ai, &far, &fai);

		br = *ar;
		bi = *ai;

		fbr = far;
		fbi = fai;

		*ar = urandom();
		*ai = urandom();

		for (j = 0; j < 1000; j++) {

			fata(cr, ci, n, *ar, *ai, &far, &fai);

			if (zabs(far, fai) < EPSILON)
				return;

			if (zabs(far, fai) < zabs(fbr, fbi)) {

				xr = *ar;
				xi = *ai;

				*ar = br;
				*ai = bi;

				br = xr;
				bi = xi;

				xr = far;
				xi = fai;

				far = fbr;
				fai = fbi;

				fbr = xr;
				fbi = xi;
			}

			// dx = b - a

			dxr = br - *ar;
			dxi = bi - *ai;

			// df = fb - fa

			dfr = fbr - far;
			dfi = fbi - fai;

			// y = dx / df

			d = dfr * dfr + dfi * dfi;

			if (d == 0.0)
				break;

			yr = (dxr * dfr + dxi * dfi) / d;
			yi = (dxi * dfr - dxr * dfi) / d;

			// a = b - y * fb

			*ar = br - (yr * fbr - yi * fbi);
			*ai = bi - (yr * fbi + yi * fbr);
		}
	}

	stop("nroots: convergence error");
}

// compute f at a

void
fata(double cr[], double ci[], int n, double ar, double ai, double *far, double *fai)
{
	int k;
	double t, xr, xi;

	// x = a

	xr = ar;
	xi = ai;

	// fa = c0 + c1 * x

	*far = cr[0] + cr[1] * xr - ci[1] * xi;
	*fai = ci[0] + cr[1] * xi + ci[1] * xr;

	for (k = 2; k < n; k++) {

		// x = a * x

		t = ar * xr - ai * xi;
		xi = ar * xi + ai * xr;
		xr = t;

		// fa += c[k] * x

		*far += cr[k] * xr - ci[k] * xi;
		*fai += cr[k] * xi + ci[k] * xr;
	}
}

// divide by x - a

void
nreduce(double cr[], double ci[], int n, double ar, double ai)
{
	int k;

	// divide

	for (k = n - 1; k > 0; k--) {
		cr[k - 1] += cr[k] * ar - ci[k] * ai;
		ci[k - 1] += ci[k] * ar + cr[k] * ai;
	}

	if (zabs(cr[0], ci[0]) > DELTA)
		stop("nroots: residual error"); // not a root

	// shift

	for (k = 0; k < n - 1; k++) {
		cr[k] = cr[k + 1];
		ci[k] = ci[k + 1];
	}
}

double
zabs(double r, double i)
{
	return sqrt(r * r + i * i);
}

double
urandom(void)
{
	return 4.0 * ((double) random() / (double) 0x7fffffff) - 2.0;
}
