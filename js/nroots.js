const DELTA = 1e-6;
const EPSILON = 1e-9;

function
nroots()
{
	var h, i, n;
	var A, P, X, RE, IM;
	var ar, ai, mag;
	var cr = [], ci = [];
	var tr = [], ti = [];

	X = pop();
	P = pop();

	h = stack.length;

	coeffs(P, X); // put coeffs on stack

	n = stack.length - h;

	if (n == 1) {
		stack[h] = symbol(NIL); // P is just a constant, no roots
		return;
	}

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
			stopf("nroots: coeffs");

		cr[i] = RE.d;
		ci[i] = IM.d;
	}

	stack.splice(h); // pop all

	while (n > 1) {

		nfindroot(cr, ci, n, tr, ti);

		ar = tr[0];
		ai = ti[0];

		mag = zabs(ar, ai);

		if (Math.abs(ar / mag) < DELTA)
			ar = 0;

		if (Math.abs(ai / mag) < DELTA)
			ai = 0;

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

	n = stack.length - h;

	if (n == 1)
		return; // only 1 root

	sort(n);

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A.elem[i] = stack[h + i];

	stack.splice(h); // pop all

	push(A);
}

// uses secant method

function
nfindroot(cr, ci, n, ar, ai)
{
	var i, j;
	var d;
	var br, dfr, dxr, far, fbr, tr = [], xr, yr;
	var bi, dfi, dxi, fai, fbi, ti = [], xi, yi;

	// if const term is zero then root is zero

	// note: use exact zero, not "close to zero"

	// term will be exactly zero from coeffs(), no need for arbitrary cutoff

	if (cr[0] == 0 && ci[0] == 0) {
		ar[0] = 0;
		ai[0] = 0;
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
		ci[i] = yi
	}

	cr[n - 1] = 1;
	ci[n - 1] = 0;

	for (i = 0; i < 100; i++) {

		ar[0] = urandom();
		ai[0] = urandom();

		fata(cr, ci, n, ar[0], ai[0], tr, ti);

		far = tr[0];
		fai = ti[0];

		br = ar[0];
		bi = ai[0];

		fbr = far;
		fbi = fai;

		ar[0] = urandom();
		ai[0] = urandom();

		for (j = 0; j < 1000; j++) {

			fata(cr, ci, n, ar[0], ai[0], tr, ti);

			far = tr[0];
			fai = ti[0];

			if (zabs(far, fai) < EPSILON)
				return;

			if (zabs(far, fai) < zabs(fbr, fbi)) {

				xr = ar[0];
				xi = ai[0];

				ar[0] = br;
				ai[0] = bi;

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

			dxr = br - ar[0];
			dxi = bi - ai[0];

			// df = fb - fa

			dfr = fbr - far;
			dfi = fbi - fai;

			// y = dx / df

			d = dfr * dfr + dfi * dfi;

			if (d == 0)
				break;

			yr = (dxr * dfr + dxi * dfi) / d;
			yi = (dxi * dfr - dxr * dfi) / d;

			// a = b - y * fb

			ar[0] = br - (yr * fbr - yi * fbi);
			ai[0] = bi - (yr * fbi + yi * fbr);
		}
	}

	stopf("nroots: convergence error");
}

// compute f at a

function
fata(cr, ci, n, ar, ai, far, fai)
{
	var k;
	var xr, xi, yr, yi;

	yr = cr[n - 1];
	yi = ci[n - 1];

	for (k = n - 2; k >= 0; k--) {

		// x = a * y

		xr = ar * yr - ai * yi;
		xi = ar * yi + ai * yr;

		// y = x + c

		yr = xr + cr[k];
		yi = xi + ci[k];
	}

	far[0] = yr;
	fai[0] = yi;
}

// divide by x - a

function
nreduce(cr, ci, n, ar, ai)
{
	var k;

	// divide

	for (k = n - 1; k > 0; k--) {
		cr[k - 1] += cr[k] * ar - ci[k] * ai;
		ci[k - 1] += ci[k] * ar + cr[k] * ai;
	}

	// check

	if (zabs(cr[0], ci[0]) > DELTA)
		stopf("nroots: residual error"); // not a root

	// shift

	for (k = 0; k < n - 1; k++) {
		cr[k] = cr[k + 1];
		ci[k] = ci[k + 1];
	}
}

function
zabs(r, i)
{
	return Math.sqrt(r * r + i * i);
}

function
urandom()
{
	return 4.0 * Math.random() - 2.0;
}
