function
dts(p1, p2)
{
	var i, n, p3;

	p3 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2);
		derivative();
		p3.elem[i] = pop();
	}

	push(p3);
}
