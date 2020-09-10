function
set_binding_and_arglist(p, b, a)
{
	if ("printname" in p) {
		binding[p.printname] = b;
		arglist[p.printname] = a;
	}
}
