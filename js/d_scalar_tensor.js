function
d_scalar_tensor(p1, p2)
{
	var i, n, p3;

	p3 = copy_tensor(p2);

	n = p2.elem.length;

	for (i = 0; i < n; i++) {
		push(p1);
		push(p2.elem[i]);
		derivative();
		p3.elem[i] = pop();
	}

	push(p3);
}
