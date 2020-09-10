function
set_binding(p, b)
{
	if ("printname" in p) {
		binding[p.printname] = b;
		arglist[p.printname] = symbol(NIL);
	}
}
