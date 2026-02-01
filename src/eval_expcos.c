void
eval_expcos(struct atom *p1)
{
	scan("1/2 exp(i z) + 1/2 exp(-i z)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
