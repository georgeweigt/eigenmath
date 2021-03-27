function
cmp_args(p1)
{
	push(cadr(p1));
	evalf();
	floatfunc();

	push(caddr(p1));
	evalf();
	floatfunc();

	return cmpfunc();
}
