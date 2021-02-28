function
get_binding(p)
{
	p = binding[p.printname];
	if (p == undefined)
		p = symbol(NIL);
	return p;
}
