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
	var a, b, p;

	p = pop();

	if (isrational(p)) {
		a = bignum_div(p.a, p.b);
		b = bignum_int(1);
		push_bignum(p.sign, a, b);
		if (!isnegativenumber(p)) {
			push_integer(1);
			add();
		}
		return;
	}

	if (isdouble(p)) {
		push_double(Math.ceil(p.d));
		return;
	}

	push_symbol(CEILING);
	push(p);
	list(2);
}
