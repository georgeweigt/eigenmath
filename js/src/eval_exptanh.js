function
eval_exptanh(p1)
{
	scan("-1 / (exp(2 z) + 1) + exp(2 z) / (exp(2 z) + 1)", 0);
	push_symbol(Z_LOWER);
	push(cadr(p1));
	subst();
	evalf();
}
