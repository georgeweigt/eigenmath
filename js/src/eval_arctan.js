function
eval_arctan(p1)
{
	push(cadr(p1));
	evalf();
	if (iscons(cddr(p1))) {
		push(caddr(p1));
		evalf();
	} else
		push_integer(1);
	arctan();
}
