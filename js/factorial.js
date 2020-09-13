function
factorial()
{
	var i, m, n, p;

	p = pop();

	if (isrational(p) && p.a >= 0 && p.b == 1)
		n = p.a;
	else if (isdouble(p) && p.d >= 0 && Math.floor(p.d) == p.d)
		n = p.d;
	else {
		push_symbol(FACTORIAL);
		push(p);
		list(2);
		return;
	}

	m = 1;

	for (i = 2; i <= n; i++)
		m *= i;

	if (isrational(p))
		push_integer(m);
	else
		push_double(m);
}
