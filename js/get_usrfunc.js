function
get_usrfunc(p)
{
	p = usrfunc[p.printname];
	if (p == undefined)
		p = symbol(NIL);
	return p;
}
