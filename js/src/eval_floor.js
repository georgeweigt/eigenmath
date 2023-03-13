function
eval_floor(p1)
{
	push(cadr(p1));
	evalf();
	floorfunc();
}

function
floorfunc()
{
	var a, b, d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			floorfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

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
