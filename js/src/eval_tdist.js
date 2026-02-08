function
eval_tdist(p1)
{
	var df, t, x, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 1st argument is not numerical");
	push(p2);
	t = pop_double();
	if (!isFinite(t))
		stopf("tdist: 1st argument is not finite");

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("tdist: 2nd argument is not numerical");
	push(p2);
	df = pop_double();
	if (!isFinite(df))
		stopf("tdist: 2nd argument is not finite");

	x = tdist(t, df);

	if (!isFinite(x))
		stopf("tdist did not converge");

	push_double(x);
}

function
tdist(t, df)
{
	var x, a, b;
	x = 0.5 * (t + Math.sqrt(t * t + df)) / Math.sqrt(t * t + df);
	a = 0.5 * df;
	b = 0.5 * df;
	return incbeta(a, b, x);
}
