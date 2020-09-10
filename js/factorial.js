function
factorial()
{
	var i, n = -1, p1;

	p1 = pop();

	if (isrational(p1) && p1.b == 1)
		n = p.a;

	if (isdouble(p1) && Math.floor(p.d) == p.d)
		n = p.d;

	if (n < 0) {
		push_symbol(FACTORIAL);
		push(p1);
		list(2);
		return;
	}

	push_integer(1);

	for (i = 2; i <= n; i++) {
		push_integer(i);
		multiply();
	}

	if (isdouble(p1))
		floatf();
}
