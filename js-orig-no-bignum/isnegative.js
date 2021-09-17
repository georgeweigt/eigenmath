function
isnegative(p)
{
	return isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)));
}
