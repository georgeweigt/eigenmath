function
set_symbol(p, b, u)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	if (p == b)
		b = symbol(NIL);
	if (p == u)
		u = symbol(NIL);
	binding[p.printname] = b;
	usrfunc[p.printname] = u;
}
