function
isinteger(p)
{
	return isrational(p) && bignum_equal(p.b, 1);
}
