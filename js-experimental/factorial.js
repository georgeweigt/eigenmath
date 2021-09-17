function
factorial()
{
	var i, m, n, p;

	p = pop();

	if (isposint(p)) {
		push(p);
		n = pop_integer();
		push_integer(1);
		for (i = 2; i <= n; i++) {
			push_integer(i);
			multiply();
		}
		return;
	}

	if (isdouble(p) && p.d >= 0 && Math.floor(p.d) == p.d) {
		n = p.d;
		m = 1;
		for (i = 2; i <= n; i++)
			m *= i;
		push_double(m);
		return;
	}

	push_symbol(FACTORIAL);
	push(p);
	list(2);
}
