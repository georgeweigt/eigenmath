function
cmp_args(p1)
{
	var p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	push(p2);
		floatfunc();

	push(caddr(p1));
	evalf();
	p2 = pop();
	push(p2);
		floatfunc();

	return cmpfunc();
}
