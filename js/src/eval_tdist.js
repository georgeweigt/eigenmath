function
eval_tdist(p1)
{
	var a, b, df, t, x, p2;

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

	x = 0.5 * (t + Math.sqrt(t * t + df)) / Math.sqrt(t * t + df);
	a = 0.5 * df;
	b = 0.5 * df;

	x = incbeta(a, b, x);

	if (!isFinite(x))
		stopf("tdist did not converge");

	push_double(x);
}
