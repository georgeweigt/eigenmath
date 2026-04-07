void
eval_not(struct atom *p1)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iseqzero(p1))
		push_integer(1);
	else
		push_integer(0);
}
