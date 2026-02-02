void
eval_exptan(struct atom *p1)
{
	scan("sqrt(-1) / (exp(2 sqrt(-1) z) + 1) - sqrt(-1) exp(2 sqrt(-1) z) / (exp(2 sqrt(-1) z) + 1)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
