function
add_terms(n) // n is number of terms on stack
{
	var h;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_terms(h);

	combine_terms(h);

	n = stack.length - h;

	switch (n) {
	case 0:
		push_integer(0); // all terms canceled
		break;
	case 1:
		break;
	default:
		list(n);
		push_symbol(ADD);
		swap();
		cons();
		break;
	}
}
