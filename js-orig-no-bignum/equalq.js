function
equalq(p, a, b)
{
	if (isrational(p))
		return p.a == a && p.b == b;
	else if (isdouble(p))
		return p.d == a / b;
	else
		return 0;
}
