function
eval_transpose(p1)
{
	var m, n;
	var p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	push(p2);

	if (!istensor(p2) || p2.dim.length < 2)
		return;

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		n = pop_integer();

		push(cadr(p1));
		evalf();
		m = pop_integer();

		transpose(n, m);

		p1 = cddr(p1);
	}
}

function
transpose(n, m)
{
	var i, j, k, ndim, nelem;
	var index = [];
	var p1, p2;

	p1 = pop();

	ndim = p1.dim.length;
	nelem = p1.elem.length;

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stopf("transpose: index error");

	n--; // make zero based
	m--;

	p2 = copy_tensor(p1);

	// interchange indices n and m

	p2.dim[n] = p1.dim[m];
	p2.dim[m] = p1.dim[n];

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		k = 0;

		for (j = 0; j < ndim; j++) {
			if (j == n)
				k = k * p1.dim[m] + index[m];
			else if (j == m)
				k = k * p1.dim[n] + index[n];
			else
				k = k * p1.dim[j] + index[j];
		}

		p2.elem[k] = p1.elem[i];

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (++index[j] < p1.dim[j])
				break;
			index[j] = 0;
		}
	}

	push(p2);
}
