function
isfactor(p)
{
	if (issymbol(p))
		return 1;

	if (isinteger(p) && p.a >= 0)
		return 1;

	if (iscons(p) && car(p) != symbol(ADD) && car(p) != symbol(MULTIPLY) && car(p) != symbol(POWER))
		return 1;

	return 0;
}
