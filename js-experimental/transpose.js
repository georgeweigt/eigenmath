function
transpose(n, m)
{
	var i, j, k, ndim, p1, p2;
	var index = [];

	p1 = pop();

	ndim = p1.dim.length;

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stopf("transpose: index error");

	n--; // make zero based
	m--;

	p2 = alloc_tensor();

	for (i = 0; i < ndim; i++)
		p2.dim[i] = p1.dim[i];

	// interchange indices n and m

	p2.dim[n] = p1.dim[m];
	p2.dim[m] = p1.dim[n];

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < p1.elem.length; i++) {

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
