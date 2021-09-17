function
isdoublesomewhere(p)
{
	if (isdouble(p))
		return 1;

	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdoublesomewhere(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}
