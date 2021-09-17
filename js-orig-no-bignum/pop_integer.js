function
pop_integer()
{
	var p1 = pop();

	if (isrational(p1) && p1.b == 1)
		return p1.a;

	if (isdouble(p1) && Math.floor(p1.d) == p1.d)
		return p1.d;

	stopf("integer expected");
}
