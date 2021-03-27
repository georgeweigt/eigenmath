function
cmp_numbers(p1, p2)
{
	var d1, d2;

	if (!isnum(p1) || !isnum(p2))
		stopf("compare");

	if (isrational(p1) && isrational(p2))
		return cmp_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d1 < d2)
		return -1;

	if (d1 > d2)
		return 1;

	return 0;
}
