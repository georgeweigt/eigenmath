function
numerator()
{
	var p1 = pop();

	if (isrational(p1)) {
		push_rational_number(p1.sign, bignum_copy(p1.a), bignum_int(1));
		return;
	}

	while (divisor(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
