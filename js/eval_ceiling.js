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
	var a, b, p1;

	p1 = pop();

	if (isrational(p1)) {
		a = bignum_div(p1.a, p1.b);
		b = bignum_int(1);
		push_rational_number(p1.sign, a, b);
		if (!isnegativenumber(p1)) {
			push_integer(1);
			add();
		}
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.ceil(p1.d));
		return;
	}

	push_symbol(CEILING);
	push(p1);
	list(2);
}
