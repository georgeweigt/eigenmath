function
isdenormalclock(p)
{
	var u, v;

	if (!isnum(p))
		return 0;

	if (isdouble(p))
		return p.d <= -0.5 || p.d > 0.5;

	if (equalq(p, 1, 2))
		return 0;

	if (equalq(p, -1, 2))
		return 1;

	push(p);
	push_rational(-1, 2);
	add();

	push(p);
	push_rational(1, 2);
	add();

	v = pop();
	u = pop();

	return u.a >= 0 || v.a < 0;
}
