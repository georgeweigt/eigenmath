function
compare_rationals(p1, p2)
{
	var a, b;

	a = p1.a * p2.b;
	b = p2.a * p1.b;

	return a - b;
}
