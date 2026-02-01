function
eval_expcosh(p1)
{
	scan("1/2 exp(-z) + 1/2 exp(z)", 0);
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
