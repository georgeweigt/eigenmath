function
prep_symbol_equals(p1, p2)
{
	push(p2);

	if (!issymbol(p1))
		return;

	if (p1 == p2)
		return; // A = A

	if (iskeyword(p1))
		return; // keyword like "float"

	if (p1 == symbol(SYMBOL_I) && isimaginaryunit(p2))
		return;

	if (p1 == symbol(SYMBOL_J) && isimaginaryunit(p2))
		return;

	p2 = pop();

	push_symbol(SETQ);
	push(p1);
	push(p2);
	list(3);
}
