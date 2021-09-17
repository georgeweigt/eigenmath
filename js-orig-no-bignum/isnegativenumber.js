function
isnegativenumber(p)
{
	return (isrational(p) && p.a < 0) || (isdouble(p) && p.d < 0);
}
