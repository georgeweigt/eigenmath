void
eval_sqrt(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sqrtfunc();
}

void
sqrtfunc(void)
{
	push_rational(1, 2);
	power();
}
