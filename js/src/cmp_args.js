function
cmp_args(p1)
{
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	subtract();
	simplify();
	floatfunc();
	p1 = pop();
	if (iszero(p1))
		return 0;
	if (!isnum(p1))
		stopf("compare err");
	if (isnegativenumber(p1))
		return -1;
	else
		return 1;
}
