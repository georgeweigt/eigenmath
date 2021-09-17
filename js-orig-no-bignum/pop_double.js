function
pop_double()
{
	var p = pop();

	if (isrational(p))
		return p.a / p.b;

	if (isdouble(p))
		return p.d;

	return 0;
}
