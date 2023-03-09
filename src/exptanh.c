void
eval_exptanh(struct atom *p1)
{
	push(cadr(p1));
	eval();
	exptanh();
}

void
exptanh(void)
{
	struct atom *p1;
	push_integer(2);
	multiply();
	expfunc();
	p1 = pop();
	push(p1);
	push_integer(1);
	subtract();
	push(p1);
	push_integer(1);
	add();
	divide();
}
