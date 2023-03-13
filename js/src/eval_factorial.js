function
eval_factorial(p1)
{
	push(cadr(p1));
	evalf();
	factorial();
}

function
factorial()
{
	var i, m, n, p1;

	p1 = pop();

	if (isposint(p1)) {
		push(p1);
		n = pop_integer();
		push_integer(1);
		for (i = 2; i <= n; i++) {
			push_integer(i);
			multiply();
		}
		return;
	}

	if (isdouble(p1) && p1.d >= 0 && Math.floor(p1.d) == p1.d) {
		push(p1);
		n = pop_integer();
		m = 1.0;
		for (i = 2; i <= n; i++)
			m *= i;
		push_double(m);
		return;
	}

	push_symbol(FACTORIAL);
	push(p1);
	list(2);
}
