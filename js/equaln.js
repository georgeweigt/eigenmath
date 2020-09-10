function
equaln(p, n)
{
	if (isrational(p))
		return p.a == n && p.b == 1;
	else if (isdouble(p))
		return p.d == n;
	else
		return 0;
}
