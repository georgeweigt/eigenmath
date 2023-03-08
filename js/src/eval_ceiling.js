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
	var a, b, d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			ceiling();
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
