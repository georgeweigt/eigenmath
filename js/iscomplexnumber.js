function
iscomplexnumber(p)
{
	return isimaginarynumber(p) || (length(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}
