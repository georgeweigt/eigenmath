void
eval_expsin(struct atom *p1)
{
	scan("-1/2 sqrt(-1) exp(sqrt(-1) z) + 1/2 sqrt(-1) exp(-sqrt(-1) z)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
