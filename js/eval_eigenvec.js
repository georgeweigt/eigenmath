function
eval_eigenvec(p1)
{
	var i, j, n, D = [], Q = [];

	push(cadr(p1));
	evalf();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("eigenvec: square matrix expected");

	n = p1.dim[0];

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (!isdouble(p1.elem[n * i + j]))
				stopf("eigenvec: numerical matrix expected");

	for (i = 0; i < n - 1; i++)
		for (j = i + 1; j < n; j++)
			if (Math.abs(p1.elem[n * i + j] - p1.elem[n * j + i]) > 1e-10)
				stopf("eigenvec: symmetrical matrix expected");

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

	p1 = alloc_tensor();

	p1.dim[0] = n;
	p1.dim[1] = n;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {
			push_double(Q[n * j + i]); // transpose
			p1.elem[n * i + j] = pop();
		}

	push(p1);
}
