void
eval_exp(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expfunc();
}

void
expfunc(void)
{
	push_symbol(EXP1);
	swap();
	power();
}
