function
eval_roots(p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
	} else
		push_symbol(X_LOWER);

	roots();
}
