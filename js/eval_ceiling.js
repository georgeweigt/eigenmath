function
eval_ceiling(p1)
{
	push(cadr(p1));
	evalf();
	ceiling();
}

function
ceiling()
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
		if (isnegativenumber(p1))
			push_bignum(-1, a, b);
		else {
			push_bignum(1, a, b);
			push_integer(1);
			add();
		}
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = Math.ceil(d);
		push_double(d);
		return;
	}

	push_symbol(CEILING);
	push(p1);
	list(2);
}
