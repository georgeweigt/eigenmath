function
compare_numbers(p1, p2)
{
	var d1, d2;

	if (isrational(p1) && isrational(p2))
		return compare_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	return d1 - d2;
}
