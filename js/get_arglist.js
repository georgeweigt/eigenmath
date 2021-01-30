function
get_arglist(p)
{
	if (p.printname in arglist)
		return arglist[p.printname];
	else
		return symbol(NIL); // symbol has no arglist
}
