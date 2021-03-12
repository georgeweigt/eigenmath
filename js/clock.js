function
clock()
{
	var i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			clock();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	mag();

	push_integer(-1); // base

	push(p1);
	arg();
	push_symbol(PI);
	divide();

	power();

	multiply();
}
