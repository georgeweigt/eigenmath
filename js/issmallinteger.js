function
issmallinteger(p)
{
	if (isinteger(p))
		return bignum_issmallnum(p.a);

	if (isdouble(p))
		return p.d == Math.floor(p.d) && Math.abs(p.d) <= 0x7fffffff;

	return 0;
}
