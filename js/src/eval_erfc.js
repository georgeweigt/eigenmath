function
eval_erfc(p1)
{
	push(cadr(p1));
	evalf();
	erfcfunc();
}

function
erfcfunc()
{
	var d, p1;

	p1 = pop();

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = 1.0 - erf(d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}
