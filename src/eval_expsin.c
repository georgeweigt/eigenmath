void
eval_expsin(struct atom *p1)
{
	scan("-1/2 i exp(i z) + 1/2 i exp(-i z)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
