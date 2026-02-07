void
eval_fdist(struct atom *p1)
{
	double a, b, df1, df2, t, x;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("fdist: 1st argument is not numerical");
	push(p2);
	t = pop_double();
	if (!isfinite(t))
		stopf("fdist: 1st argument is not finite");

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("fdist: 2nd argument is not numerical");
	push(p2);
	df1 = pop_double();
	if (!isfinite(df1))
		stopf("fdist: 2nd argument is not finite");

	push(cadddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("fdist: 3rd argument is not numerical");
	push(p2);
	df2 = pop_double();
	if (!isfinite(df2))
		stopf("fdist: 3rd argument is not finite");

	if (t <= 0.0) {
		push_double(0.0);
		return;
	}

	x = t / (t + df2 / df1);
	a = 0.5 * df1;
	b = 0.5 * df2;

	x = incbeta(a, b, x);

	if (!isfinite(x))
		stopf("fdist did not converge");

	push_double(x);
}
