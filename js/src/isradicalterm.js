function
isradicalterm(p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
}
