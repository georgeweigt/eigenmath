function
eval_expsin(p1)
{
	scan("-1/2 i exp(i z) + 1/2 i exp(-i z)", 0);
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
