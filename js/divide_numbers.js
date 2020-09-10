function
divide_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		divide_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a / b);
}
