void
eval_exptan(struct atom *p1)
{
	scan("i / (exp(2 i z) + 1) - i exp(2 i z) / (exp(2 i z) + 1)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
