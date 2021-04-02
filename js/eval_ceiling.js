function
eval_ceiling(p1)
{
	var t = expanding;
	expanding = 1;
	eval_ceiling_nib(p1);
	expanding = t;
}

function
eval_ceiling_nib(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (isrational(p1)) {
		push_integer(Math.ceil(p1.a / p1.b));
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
