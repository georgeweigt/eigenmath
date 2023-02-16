function
get_usrfunc(p)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	p = usrfunc[p.printname];
	if (p == undefined)
		p = symbol(NIL); // no calls to set_symbol() since eval_clear()
	return p;
}
