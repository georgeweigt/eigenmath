function
isminusoneoversqrttwo(p)
{
	return length(p) == 3 && car(p) == symbol(MULTIPLY) && isminusone(cadr(p)) && isoneoversqrttwo(caddr(p));
}
