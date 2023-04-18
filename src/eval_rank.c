void
eval_rank(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();
	if (istensor(p1))
		push_integer(p1->u.tensor->ndim);
	else
		push_integer(0);
}
