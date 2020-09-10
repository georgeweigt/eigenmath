function
equalq(p, a, b)
{
	if (isrational(p))
		return p1.a == a && p1.b == b;
	else if (isdouble(p))
		return p.d == a / b;
	else
		return 0;
}
