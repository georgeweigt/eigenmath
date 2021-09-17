function
isimaginarynumber(p)
{
	return isimaginaryunit(p) || (lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}
