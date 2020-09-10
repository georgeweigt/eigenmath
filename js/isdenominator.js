function
isdenominator(p)
{
	return car(p) == symbol(POWER) && cadr(p) != symbol(EXP1) && isnegativeterm(caddr(p));
}
