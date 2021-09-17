function
promote_tensor()
{
	var i, j, k, ndim1, ndim2, nelem1, nelem2, p1, p2, p3;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim1 = p1.dim.length;
	nelem1 = p1.elem.length;

	// check

	p2 = p1.elem[0];

	for (i = 1; i < nelem1; i++) {
		p3 = p1.elem[i];
		if (!compatible_dimensions(p2, p3))
			stopf("tensor dimensions");
	}

	if (!istensor(p2)) {
		push(p1);
		return; // all elements are scalars
	}

	ndim2 = p2.dim.length;
	nelem2 = p2.elem.length;

	// alloc

	p3 = alloc_tensor();

	// merge dimensions

	k = 0;

	for (i = 0; i < ndim1; i++)
		p3.dim[k++] = p1.dim[i];

	for (i = 0; i < ndim2; i++)
		p3.dim[k++] = p2.dim[i];

	// merge elements

	k = 0;

	for (i = 0; i < nelem1; i++) {
		p2 = p1.elem[i];
		for (j = 0; j < nelem2; j++)
			p3.elem[k++] = p2.elem[j];
	}

	push(p3);
}
