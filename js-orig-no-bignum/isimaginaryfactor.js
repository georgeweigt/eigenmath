function
isimaginaryfactor(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}
