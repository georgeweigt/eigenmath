function
isnumerator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;
	else if (isrational(p) && Math.abs(p.a) == 1)
		return 0;
	else
		return 1;
}
