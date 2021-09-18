function
numerator()
{
	var p = pop();

	if (isrational(p)) {
		push_bignum(p.sign, bignum_copy(p.a), bignum_int(1));
		return;
	}

	while (divisor(p)) {
		push(p);
		cancel_factor();
		p = pop();
	}

	push(p);
}
