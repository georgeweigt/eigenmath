function
get_usrfunc(p)
{
	if (p.printname in usrfunc)
		return usrfunc[p.printname];
	else
		return symbol(NIL);
}
