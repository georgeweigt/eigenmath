function
add_tensors(p1, p2)
{
	var i, n;

	if (!compatible_dimensions(p1, p2))
		stopf("incompatible tensor arithmetic");

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2.elem[i]);
		add();
		p1.elem[i] = pop();
	}

	push(p1);
}
