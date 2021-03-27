function
isdenormalclock(p)
{
	var t;

	if (!isnum(p))
		return 0;

	if (isdouble(p))
		return p.d <= -0.5 || p.d > 0.5;

	push(p);
	push_rational(1, 2);
	t = cmpfunc();

	if (t > 0)
		return 1; // p > 1/2

	push(p);
	push_rational(-1, 2);
	t = cmpfunc();

	if (t <= 0)
		return 1; // p <= -1/2

	return 0;
}
