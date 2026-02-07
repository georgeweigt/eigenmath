void
eval_tdist(struct atom *p1)
{
	double a, b, df, t, x;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 1st argument is not numerical");
	push(p2);
	t = pop_double();

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 2nd argument is not numerical");
	push(p2);
	df = pop_double();

	x = 0.5 * (t + sqrt(t * t + df)) / sqrt(t * t + df);
	a = 0.5 * df;
	b = 0.5 * df;

	x = incbeta(a, b, x);

	if (!isfinite(x))
		stopf("tdist did not converge");

	push_double(x);
}
