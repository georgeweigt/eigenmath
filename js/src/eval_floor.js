function
eval_floor(p1)
{
	push(cadr(p1));
	evalf();
	floor();
}

function
floor()
{
	var a, b, d, p1;

	p1 = pop();

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	if (isrational(p1)) {
		a = bignum_div(p1.a, p1.b);
		b = bignum_int(1);
		if (isnegativenumber(p1)) {
			push_bignum(-1, a, b);
			push_integer(-1);
			add();
		} else
			push_bignum(1, a, b);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = Math.floor(d);
		push_double(d);
		return;
	}

	push_symbol(FLOOR);
	push(p1);
	list(2);
}
