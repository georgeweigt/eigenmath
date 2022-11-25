function
get_usrfunc(p)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	p = usrfunc[p.printname];
	if (p == undefined)
		p = symbol(NIL); // set_symbol() hasn't been called after clear
	return p;
}
