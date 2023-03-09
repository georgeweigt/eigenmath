function
real()
{
	var i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			real();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rect();
	p1 = pop();
	push(p1);
	push(p1);
	conjfunc();
	add();
	push_rational(1, 2);
	multiply();
}
