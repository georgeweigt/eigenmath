function
cdr(p)
{
	if ("cdr" in p)
		return p.cdr;
	else
		return symbol(NIL);
}
