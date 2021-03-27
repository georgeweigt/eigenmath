function
cmpfunc()
{
	var p1, p2;
	p2 = pop();
	p1 = pop();
	if (!isnum(p1) || !isnum(p2))
		stopf("compare");
	return cmp_numbers(p1, p2);
}
