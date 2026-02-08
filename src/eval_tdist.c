void
eval_tdist(struct atom *p1)
{
	double df, t, x;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 1st argument is not numerical");
	push(p2);
	t = pop_double();
	if (!isfinite(t))
		stopf("tdist: 1st argument is not finite");

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 2nd argument is not numerical");
	push(p2);
	df = pop_double();
	if (!isfinite(df))
		stopf("tdist: 2nd argument is not finite");

	x = tdist(t, df);

	if (!isfinite(x))
		stopf("tdist did not converge");

	push_double(x);
}

double
tdist(double t, double df)
{
	double x, a, b;
	x = 0.5 * (t + sqrt(t * t + df)) / sqrt(t * t + df);
	a = 0.5 * df;
	b = 0.5 * df;
	return incbeta(a, b, x);
}
