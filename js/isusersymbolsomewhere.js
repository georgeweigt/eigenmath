function
isusersymbolsomewhere(p)
{
	if (isusersymbol(p) && p != symbol(PI) && p != symbol(EXP1))
		return 1;

	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isusersymbolsomewhere(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}
