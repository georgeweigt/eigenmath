function
cmpfunc()
{
	var p;
	subtract();
	p = pop();
	if (!isnum(p))
		stop("compare");
	if (iszero(p))
		return 0;
	if (isdouble(p)) {
		if (p.d < 0)
			return -1;
		else
			return 1;
	} else {
		if (p.a < 0)
			return -1;
		else
			return 1;
	}
}
