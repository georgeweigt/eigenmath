function
eval_arctan(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	if (iscons(cddr(p1))) {
		push(caddr(p1));
		evalf();
	} else
		push_integer(1);
	arctan();
	expanding = t;
}
