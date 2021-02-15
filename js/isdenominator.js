function
isdenominator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;
	else if (isrational(p) && p.b != 1)
		return 1;
	else
		return 0;
}
