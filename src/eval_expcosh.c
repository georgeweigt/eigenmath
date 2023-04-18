void
eval_expcosh(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expcosh();
}

void
expcosh(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	add();
	push_rational(1, 2);
	multiply();
}
