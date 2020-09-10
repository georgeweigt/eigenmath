// n is number of terms on stack

function
add_terms(n)
{
	var h;

	if (n < 2)
		return;

	if (n == 2 && isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		add_numbers(p1, p2);
		return;
	}

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
