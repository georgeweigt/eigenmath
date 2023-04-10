function
get_binding(p1)
{
	var p2;
	if (!isusersymbol(p1))
		stopf("symbol error");
	p2 = binding[p1.printname];
	if (p2 == undefined || p2 == symbol(NIL))
		p2 = p1; // symbol binds to itself
	return p2;
}
