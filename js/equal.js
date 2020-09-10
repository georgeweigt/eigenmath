function
equal(p1, p2)
{
	if (p1 == p2)
		return 1;

	if (iscons(p1) && iscons(p2)) {
		while (iscons(p1) && iscons(p2)) {
			if (!equal(car(p1), car(p2)))
				return 0;
			p1 = cdr(p1);
			p2 = cdr(p2);
		}
		return p1 == symbol(NIL) && p2 == symbol(NIL);
	}

	if (isrational(p1) && isrational(p2))
		return p1.a == p2.a && p1.b == p2.b;

	if (isrational(p1) && isdouble(p2))
		return p1.a / p1.b == p2.d;

	if (isdouble(p1) && isrational(p2))
		return p1.d == p2.a / p2.b;

	if (isdouble(p1) && isdouble(p1))
		return p1.d == p2.d;

	if (issymbol(p1) && issymbol(p2))
		return p1.printname == p2.printname;

	if (isstring(p1) && isstring(p2))
		return p1.string == p2.string;

	return 0; // no need to compare tensors
}
