void
eval_expsinh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expsinh();
}

void
expsinh(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	subtract();
	push_rational(1, 2);
	multiply();
}
