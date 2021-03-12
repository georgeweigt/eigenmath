function
hadamard()
{
	var i, n, p1, p2;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1.dim.length != p2.dim.length)
		stopf("hadamard");

	n = p1.dim.length;

	for (i = 0; i < n; i++)
		if (p1.dim[i] != p2.dim[i])
			stopf("hadamard");

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2.elem[i]);
		multiply();
		p1.elem[i] = pop();
	}

	push(p1);
}
