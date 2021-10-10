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
	var a, b, p;

	p = pop();

	if (isinteger(p)) {
		push(p);
		return;
	}

	if (isrational(p)) {
		a = bignum_div(p.a, p.b);
		b = bignum_int(1);
		push_bignum(p.sign, a, b);
		if (isnegativenumber(p)) {
			push_integer(-1);
			add();
		}
		return;
	}

	if (isdouble(p)) {
		push_double(Math.floor(p.d));
		return;
	}

	push_symbol(FLOOR);
	push(p);
	list(2);
}
