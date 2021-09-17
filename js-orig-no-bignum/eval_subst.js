function
eval_subst(p1)
{
	push(cadddr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	push(cadr(p1));
	evalf();
	subst();
	evalf(); // normalize
}
