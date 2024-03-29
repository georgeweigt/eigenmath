function
cmp(p1, p2)
{
	var t;

	if (p1 == p2)
		return 0;

	if (p1 == symbol(NIL))
		return -1;

	if (p2 == symbol(NIL))
		return 1;

	if (isnum(p1) && isnum(p2))
		return cmp_numbers(p1, p2);

	if (isnum(p1))
		return -1;

	if (isnum(p2))
		return 1;

	if (isstring(p1) && isstring(p2))
		return cmp_strings(p1.string, p2.string);

	if (isstring(p1))
		return -1;

	if (isstring(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return cmp_strings(printname(p1), printname(p2));

	if (issymbol(p1))
		return -1;

	if (issymbol(p2))
		return 1;

	if (istensor(p1) && istensor(p2))
		return cmp_tensors(p1, p2);

	if (istensor(p1))
		return -1;

	if (istensor(p2))
		return 1;

	while (iscons(p1) && iscons(p2)) {
		t = cmp(car(p1), car(p2));
		if (t)
			return t;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

	return 0;
}
