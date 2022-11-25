function
get_binding(p)
{
	if (!isusersymbol(p))
		stopf("symbol error");
	return binding[p.printname];
}
