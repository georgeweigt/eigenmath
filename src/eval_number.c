void
eval_number(struct atom *p1)
{
	push(cadr(p1));
	evalf();

	p1 = pop();

	if (isnum(p1))
		push_integer(1);
	else
		push_integer(0);
}
