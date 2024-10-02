function
eval_eigenvec(p1)
{
	var i, j, n, D = [], Q = [];

	push(cadr(p1));
	evalf();
	floatfunc();
	p1 = pop();

	if (!issquarematrix(p1))
		stopf("eigenvec");

	n = p1.dim[0];

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (!isdouble(p1.elem[n * i + j]))
				stopf("eigenvec");

	for (i = 0; i < n - 1; i++)
		for (j = i + 1; j < n; j++)
			if (Math.abs(p1.elem[n * i + j] - p1.elem[n * j + i]) > 1e-10)
				stopf("eigenvec");

	// initialize D

	for (i = 0; i < n; i++) {
		D[n * i + i] = p1.elem[n * i + i].d;
		for (j = i + 1; j < n; j++) {
			D[n * i + j] = p1.elem[n * i + j].d;
			D[n * j + i] = p1.elem[n * i + j].d;
		}
	}

	// initialize Q

	for (i = 0; i < n; i++) {
		Q[n * i + i] = 1.0;
		for (j = i + 1; j < n; j++) {
			Q[n * i + j] = 0.0;
			Q[n * j + i] = 0.0;
		}
	}

	eigenvec(D, Q, n);

	p1 = alloc_matrix(n, n);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			push_double(Q[n * j + i]); // transpose
			p1.elem[n * i + j] = pop();
		}
	}

	push(p1);
}

function
eigenvec(D, Q, n)
{
	var i;

	for (i = 0; i < 100; i++)
		if (eigenvec_step(D, Q, n) == 0)
			return;

	stopf("eigenvec: convergence error");
}

function
eigenvec_step(D, Q, n)
{
	var count, i, j;

	count = 0;

	// for each upper triangle "off-diagonal" component do step_nib

	for (i = 0; i < n - 1; i++) {
		for (j = i + 1; j < n; j++) {
			if (D[n * i + j] != 0.0) {
				eigenvec_step_nib(D, Q, n, i, j);
				count++;
			}
		}
	}

	return count;
}

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
