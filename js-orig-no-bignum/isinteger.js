function
isinteger(p)
{
	return isrational(p) && p.b == 1;
}
