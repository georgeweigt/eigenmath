function
eval_inner(p1)
{
	var h = stack.length;

	// evaluate from right to left

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		p1 = cdr(p1);
	}

	if (h == stack.length)
		stopf("dot");

	evalf();

	while (stack.length - h > 1) {
		p1 = pop();
		evalf();
		push(p1);
		inner();
	}
}

function
inner()
{
	var i, j, k, n, mcol, mrow, ncol, ndim, nrow, p1, p2, p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		p2 = copy_tensor(p2);
		n = p2.elem.length;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2.elem[i]);
			multiply();
			p2.elem[i] = pop();
		}
		push(p2);
		return;
	}

	k = p1.dim.length - 1;

	ncol = p1.dim[k];
	mrow = p2.dim[0];

	if (ncol != mrow)
		stopf("inner: dimension err");

	ndim = p1.dim.length + p2.dim.length - 2;

	//	nrow is the number of rows in p1
	//
	//	mcol is the number of columns p2
	//
	//	Example:
	//
	//	A[3][3][4] B[4][4][3]
	//
	//	  3  3				nrow = 3 * 3 = 9
	//
	//	                4  3		mcol = 4 * 3 = 12

	nrow = p1.elem.length / ncol;
	mcol = p2.elem.length / mrow;

	p3 = alloc_tensor();

	for (i = 0; i < nrow; i++) {
		for (j = 0; j < mcol; j++) {
			for (k = 0; k < ncol; k++) {
				push(p1.elem[i * ncol + k]);
				push(p2.elem[k * mcol + j]);
				multiply();
			}
			add_terms(ncol);
			p3.elem[i * mcol + j] = pop();
		}
	}

	if (ndim == 0) {
		push(p3.elem[0]); // scalar result
		return;
	}

	// dim info

	k = 0;

	n = p1.dim.length - 1;

	for (i = 0; i < n; i++)
		p3.dim[k++] = p1.dim[i];

	n = p2.dim.length;

	for (i = 1; i < n; i++)
		p3.dim[k++] = p2.dim[i];

	push(p3);
}
