void
eval_testeq(struct atom *p1)
{
	push(cadr(p1));
	evalg();
	push(caddr(p1));
	evalg();
	subtract();
	simplify();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}
