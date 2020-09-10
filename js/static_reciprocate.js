function
static_reciprocate()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		push(p1);
		push_symbol(POWER);
		push(p2);
		push_integer(-1);
		list(3);
		return;
	}

	if (isnum(p1) && isnum(p2)) {
		divide_numbers(p1, p2);
		return;
	}

	if (isnum(p2)) {
		push(p1);
		push(p2);
		reciprocate();
		return;
	}

	push(p1);
	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
