function
isnegativenumber(p)
{
	return (isrational(p) && p.sign == -1) || (isdouble(p) && p.d < 0);
}
