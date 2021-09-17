function
isimaginaryunit(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p)) && equalq(caddr(p), 1, 2);
}
