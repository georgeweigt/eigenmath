function
static_reciprocate()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		if (!isinteger1(p1))
			push(p1);
		push_symbol(POWER);
		push(p2);
		push_integer(-1);
		list(3);
		return;
	}

	if (isnum(p1) && isnum(p2)) {
		push(p1);
		push(p2);
		divide();
		return;
	}

	if (isnum(p2)) {
		if (!isinteger1(p1))
			push(p1);
		push(p2);
		reciprocate();
		return;
	}

	if (car(p2) == symbol(POWER) && isnum(caddr(p2))) {
		if (!isinteger1(p1))
			push(p1);
		push_symbol(POWER);
		push(cadr(p2));
		push(caddr(p2));
		negate();
		list(3);
		return;
	}

	if (!isinteger1(p1))
		push(p1);

	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
