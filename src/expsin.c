void
eval_expsin(struct atom *p1)
{
	push(cadr(p1));
	eval();
	expsin();
}

void
expsin(void)
{
	struct atom *p1;
	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();
}
