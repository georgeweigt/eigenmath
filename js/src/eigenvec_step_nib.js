function
eigenvec_step_nib(D, Q, n, p, q)
{
	var k;
	var t, theta;
	var c, cc, s, ss;

	// compute c and s

	// from Numerical Recipes (except they have a_qq - a_pp)

	theta = 0.5 * (D[n * p + p] - D[n * q + q]) / D[n * p + q];

	t = 1.0 / (Math.abs(theta) + Math.sqrt(theta * theta + 1.0));

	if (theta < 0.0)
		t = -t;

	c = 1.0 / Math.sqrt(t * t + 1.0);

	s = t * c;

	// D = GD

	// which means "add rows"

	for (k = 0; k < n; k++) {
		cc = D[n * p + k];
		ss = D[n * q + k];
		D[n * p + k] = c * cc + s * ss;
		D[n * q + k] = c * ss - s * cc;
	}

	// D = D transpose(G)

	// which means "add columns"

	for (k = 0; k < n; k++) {
		cc = D[n * k + p];
		ss = D[n * k + q];
		D[n * k + p] = c * cc + s * ss;
		D[n * k + q] = c * ss - s * cc;
	}

	// Q = GQ

	// which means "add rows"

	for (k = 0; k < n; k++) {
		cc = Q[n * p + k];
		ss = Q[n * q + k];
		Q[n * p + k] = c * cc + s * ss;
		Q[n * q + k] = c * ss - s * cc;
	}

	D[n * p + q] = 0.0;
	D[n * q + p] = 0.0;
}
