function
compare_rationals(p1, p2)
{
	var p3;

	subtract_rationals(p1, p2);

	p3 = pop();

	if (isrational(p3))
		return p3.a;
	else
		return p3.d;
}
