function
cmp_factors(p1, p2)
{
	var a, b, c;
	var base1, base2, expo1, expo2;

	a = order_factor(p1);
	b = order_factor(p2);

	if (a < b)
		return -1;

	if (a > b)
		return 1;

	if (car(p1) == symbol(POWER)) {
		base1 = cadr(p1);
		expo1 = caddr(p1);
	} else {
		base1 = p1;
		expo1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		base2 = cadr(p2);
		expo2 = caddr(p2);
	} else {
		base2 = p2;
		expo2 = one;
	}

	c = cmp(base1, base2);

	if (c == 0)
		c = cmp(expo2, expo1); // swapped to reverse sort order

	return c;
}
