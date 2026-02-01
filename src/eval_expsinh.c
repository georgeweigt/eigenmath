void
eval_expsinh(struct atom *p1)
{
	scan("-1/2 exp(-z) + 1/2 exp(z)");
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
