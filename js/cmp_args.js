function
cmp_args(p1)
{
	push(cadr(p1));
	evalf();

	push(caddr(p1));
	evalf();

	return cmpfunc();
}
