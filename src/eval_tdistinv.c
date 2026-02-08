void
eval_tdistinv(struct atom *p1)
{
	int i;
	double a, b, c, df, x, y;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdistinv: 1st argument is not numerical");
	push(p2);
	x = pop_double();
	if (!isfinite(x))
		stopf("tdistinv: 1st argument is not finite");

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdistinv: 2nd argument is not numerical");
	push(p2);
	df = pop_double();
	if (!isfinite(df))
		stopf("tdistinv: 2nd argument is not finite");

	if (x < 1e-12) {
		push_double(-INFINITY);
		return;
	}

	if (x == 0.5) {
		push_double(0.0);
		return;
	}

	if (x > 1.0 - 1e-12) {
		push_double(INFINITY);
		return;
	}

	a = -100.0;
	b = 100.0;

	for (i = 0; i < 50; i++) {
		c = 0.5 * (a + b);
		y = tdist(c, df);
		if (!isfinite(y))
			stopf("tdistinv did not converge");
		if (y < x)
			a = c;
		else
			b = c;
	}

	push_double(c);
}
