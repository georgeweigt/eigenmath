function
get_usrfunc(p)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	return usrfunc[p.printname];
}
