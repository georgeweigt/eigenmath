function
get_binding(p)
{
	if (p.printname in binding)
		return binding[p.printname];
	else
		return symbol(NIL); // symbol has no binding
}
