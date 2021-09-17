function
lookup(s)
{
	var p = symtab[s];
	if (p == undefined) {
		p = {printname:s, func:eval_user_symbol};
		symtab[s] = p;
	}
	return p;
}
