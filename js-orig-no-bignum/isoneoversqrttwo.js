function
isoneoversqrttwo(p)
{
	return car(p) == symbol(POWER) && equaln(cadr(p), 2) && equalq(caddr(p), -1, 2);
}
