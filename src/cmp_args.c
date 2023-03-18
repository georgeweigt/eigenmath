int
cmp_args(struct atom *p1)
{
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	push(p2);
	if (!isnum(p2))
		floatfunc();

	push(caddr(p1));
	evalf();
	p2 = pop();
	push(p2);
	if (!isnum(p2))
		floatfunc();

	return cmpfunc();
}
