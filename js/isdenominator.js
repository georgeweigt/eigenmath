function
isdenominator(p)
{
	return car(p) == symbol(POWER) && isnegativenumber(caddr(p));
}
