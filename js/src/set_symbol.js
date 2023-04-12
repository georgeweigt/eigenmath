function
set_symbol(p1, p2, p3)
{
	if (!isusersymbol(p1))
		stopf("symbol error");
	binding[p1.printname] = p2;
	usrfunc[p1.printname] = p3;
}
