function
eval_arctan(p1)
{
	push(cadr(p1));
	evalf();
	if (caddr(p1) == symbol(NIL))
		push_integer(1);
	else {
		push(caddr(p1));
		evalf();
	}
	arctan();
}
