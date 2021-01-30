function
lookup(s)
{
	if (!(s in symtab))
		symtab[s] = {printname:s, func:eval_symbol};
	if (!(s in binding))
		binding[s] = symbol(NIL);
	if (!(s in arglist))
		arglist[s] = symbol(NIL);
	return symtab[s];
}
